#ifndef CONCURRENT_STRONG_HASHMAP_H        
#define CONCURRENT_STRONG_HASHMAP_H   
#include "ConcurrentMap.h"
#include "Segment.h" 
#include <string>
#include <vector>
#include <mutex> 

/**
 * ConcurrentStrongHashMap: ǿһ���Բ�����ϣ�� 
 * note:  
 */
template<typename T, typename R>
class ConcurrentStrongHashMap:public ConcurrentMap<T, R>       
{
	public:
	// 1. constructor
	ConcurrentStrongHashMap(unsigned int _curr_level=16);
	ConcurrentStrongHashMap(const std::vector<T> &_keys, const std::vector<R> &_values, unsigned int _curr_level=16);
	
	// 2. copy/move_controller 
    ConcurrentStrongHashMap(const ConcurrentStrongHashMap &other) = delete;                   // ���ÿ������캯��
    ConcurrentStrongHashMap &operator= (const ConcurrentStrongHashMap &other) = delete;       // ���ÿ�����ֵ�����
    ConcurrentStrongHashMap(ConcurrentStrongHashMap &&other) = delete;                        // �����ƶ����캯��
	ConcurrentStrongHashMap &operator= (ConcurrentStrongHashMap &&other) = delete;            // �����ƶ���ֵ�����

    // 3. methods
    void insert(T key, R value) override;   	 // ���ϣ���в���ָ���ļ�ֵ�ԣ�����ϣ�����Ѿ����ڸü�������¸ü���Ӧ��ֵΪ��ֵ 
	void remove(T key) override;            	 // �ӹ�ϣ�����Ƴ�ָ�����ļ�ֵ�� 
	bool contain(T key) override;           	 // �жϹ�ϣ�����Ƿ����ָ����   
	bool get(T key, R &value) override;    		 // ���ҹ�ϣ���и�������Ӧ��ֵ  
	std::vector<T> keys() override;              // ��ù�ϣ�������м����б�  
	int length() override;                 		 // ��ȡ��ϣ��ĳ��� 
	std::string toString() override;             // �����ϣ��
	R &operator[] (T key);                       // ���ص��±������ 
	~ConcurrentStrongHashMap();                  // �������� 
    
    private:
    // 4. domains
    Segment<T, R> **segments;                   // Segment������ 
    const unsigned int capacity;                // Segment�ε��������̶�Ϊ16�� 
    unsigned int curr_level;                    // дʱ�����ȼ���ʵ���Ͼ���Segment�ĸ�����Segment�ĸ����������Ĳ���д�̸߳��� 
    
    // 5. private functions
    unsigned int jenkins_prehash(unsigned int code) const;     // Ԥ�����ϣ�㷨��Wang/Jenkins hash�㷨
    unsigned int first_hash(unsigned int code) const;          // ��һ�ι�ϣ��ȷ����Ҫ�����Segmentλ���±� 
};

/**
 * ConcurrentHashMap: ���캯��
 * param _curr_level: �����̶ȣ�ǿһ���Թ�ϣ�����֧��_curr_level�������Ķ�/д�߳̽��в�����д 
 */
template<typename T, typename R>
ConcurrentStrongHashMap<T, R>::ConcurrentStrongHashMap(unsigned int _curr_level):capacity((_curr_level>0)?_curr_level:16)
{
	segments=new Segment<T, R>* [capacity];
	
	for(int i=0;i<capacity;i++)
	{
		segments[i]=nullptr;
	}
}

/**
 * ConcurrentStrongHashMap: ���캯��
 * param _keys: ���б�
 * param _values: ����б���ƥ���ֵ�б�
 * param _curr_level: �����̶ȣ�ǿһ���Թ�ϣ�����֧��_curr_level�������Ķ�/д�߳̽��в�����д 
 */
template<typename T, typename R>
ConcurrentStrongHashMap<T, R>::ConcurrentStrongHashMap(const std::vector<T> &_keys, const std::vector<R> &_values, unsigned int _curr_level):capacity((_curr_level>0)?_curr_level:16)
{
	segments=new Segment<T, R>* [capacity];
	for(int i=0;i<capacity;i++)
	{
		segments[i]=nullptr;
	}
	for(int i=0;i<_keys.size();i++)
	{
		this->insert(_keys[i], _values[i]);
	}
}

/** 
 * insert: ���ϣ���в����µļ�ֵ�ԣ����߸����Ѿ����ڵļ�ֵ���е�ֵ
 * note: ��ʵ����ֻ��Ҫ����ͨ����һ�ι�ϣ��λ��Ҫ��д��Segment��Ȼ�󽫲�������ί�и�Segment�Ĳ��뷽������ 
 */
template<typename T, typename R>
void ConcurrentStrongHashMap<T, R>::insert(T key, R value)
{
	int index=first_hash(key.hashCode());
	if(!segments[index])
	segments[index]=new Segment<T, R>();
	segments[index]->insert(key, value);
}

/** 
 * remove: �ӹ�ϣ����ɾ��ָ�����ļ�ֵ��
 * note: ��ʵ���ϣ�����ͨ����һ�ι�ϣ��λ��ɾ�������ڵ�Segment��Ȼ�󽫲�������ί�и�Segment�Ĳ��뷽������ 
 */
template<typename T, typename R>
void ConcurrentStrongHashMap<T, R>::remove(T key)
{
	int index=first_hash(key.hashCode());
	if(!segments[index])
	return;
	else
	segments[index]->remove(key);
}

/**
 * contain: �ж�ĳ�����Ƿ��ڹ�ϣ����
 * return: �������ļ��ڹ�ϣ�����򷵻�true�����򷵻�false 
 */
template<typename T, typename R>
bool ConcurrentStrongHashMap<T, R>::contain(T key)
{
	int index=first_hash(key.hashCode());
	if(segments[index])
	return segments[index]->contain(key);
	else
	return false;
}

/**
 * get: ��ø�������Ӧ��ֵ����ȫ�棩
 * note: ����ϣ���д��ڸ����ļ����򷵻�true��ֵͨ���������value���أ����򣬷���false 
 */
template<typename T, typename R>
bool ConcurrentStrongHashMap<T, R>::get(T key, R &value)
{
	int index=first_hash(key.hashCode());
	if(segments[index])
	return segments[index]->get(key, value);
	else
	return false;
} 

/**
 * operator[]: ��ø�������Ӧ��ֵ������ȫ�棩
 * note: ����ϣ���д��ڸ����ļ����򷵻ض�Ӧ��ֵ��������Ϊ��δ����� 
 */
template<typename T, typename R>
R &ConcurrentStrongHashMap<T, R>::operator[] (T key)
{
	int index=first_hash(key.hashCode());
	if(segments[index])
	return (*segments[index])[key];
}

/**
 * keys: ��ȡ��ϣ��ļ��ļ��� 
 * note 1: ��ȡ��ϣ��ļ��ļ�����Ҫ�������е�Segment���������һ��Эͬ����
 * note 2: ����Эͬ����������ʹ���˺�jdk1.7���ƵĲ��ԣ�
 *         ���ֹ����׶Σ� 
 *         1. �������������е�Segment������ÿ��Segment��keys����(Segment��keys�������������ռ����б��������Segment��mod_count֮��
 *         2. �ٴμ���mod_count֮��
 *         3. ���ڶ��μ����mod_count֮�ʹ��ڵ�һ�Σ�������ִ�в���1��2���������ִ��5�β���1��2��ת����4 
 *         ���������׶Σ� 
 *         4. ���μ����������ռ� 
 */
template<typename T, typename R>
std::vector<T> ConcurrentStrongHashMap<T, R>::keys()
{
	// �ֹ����׶� 
	for(int i=0;i<6;i++)    // �ֹ����׶����ִ��6�� 
	{
		std::vector<T> res;
		int first_mod_sum=0;
		for(int k=0;k<capacity;k++)
		{
			if(segments[k])
			{
				std::vector<T> temp=segments[k]->keys();
				first_mod_sum+=segments[k]->get_mod_count();
				for(int m=0;m<temp.size();m++)
				{
					res.push_back(temp[m]);
				}
			}
		}
		
		int second_mod_sum=0;
		for(int k=0;k<capacity;k++)
		{
			if(segments[k])
			second_mod_sum+=segments[k]->get_mod_count();
		}
		
		if(first_mod_sum==second_mod_sum)
		return res;
	}
	// �������׶�
	std::vector<T> res; 
	for(int k=0;k<capacity;k++)
	{
		if(segments[k])
		{
			segments[k]->mut.lock();
			std::vector<T> temp=segments[k]->keys();
			for(int m=0;m<temp.size();m++)
			{
				res.push_back(temp[m]);
			}
		}
	}
	for(int k=0;k<capacity;k++)
	{
		segments[k]->mut.unlock();
	}
	return res;
}

/* 
 * length: ͳ�ƹ�ϣ���Ԫ������
 * note: �÷���ͬ����һ��Эͬ�������㷨ͬ�ϵ�keys���� 
 */ 
template<typename T, typename R>
int ConcurrentStrongHashMap<T, R>::length() 
{
	// �ֹ����׶� 
	for(int i=0;i<6;i++)    // �ֹ����׶����ִ��6�� 
	{
		int size=0;
		int first_mod_sum=0;
		for(int k=0;k<capacity;k++)
		{
			if(segments[k])
			{
				size+=segments[k]->length();
				first_mod_sum+=segments[k]->get_mod_count();
			}
		}
		
		int second_mod_sum=0;
		for(int k=0;k<capacity;k++)
		{
			if(segments[k])
			second_mod_sum+=segments[k]->get_mod_count();
		}
		
		if(first_mod_sum==second_mod_sum)
		return size;
	}
	// �������׶�
	int size;
	for(int k=0;k<capacity;k++)
	{
		if(segments[k])
		{
			segments[k]->mut.lock();
			size+=segments[k]->length();
		}
	}
	for(int k=0;k<capacity;k++)
	{
		segments[k]->mut.unlock();
	}
	return size;
}

/**
 * toString: �����ϣ��
 * note: �÷���ͬ����һ��Эͬ�������㷨ͬ�ϵ�keys���� 
 */ 
template<typename T, typename R>
std::string ConcurrentStrongHashMap<T, R>::toString()
{
	// �ֹ����׶� 
	for(int i=0;i<6;i++)    // �ֹ����׶����ִ��6�� 
	{
		std::string res;
		int first_mod_sum=0;
		for(int k=0;k<capacity;k++)
		{
			if(segments[k])
			{
				res=res+segments[k]->toString()+"\n";
				first_mod_sum+=segments[k]->get_mod_count();
			}
		}
		
		int second_mod_sum=0;
		for(int k=0;k<capacity;k++)
		{
			if(segments[k])
			second_mod_sum+=segments[k]->get_mod_count();
		}
		
		if(first_mod_sum==second_mod_sum)
		return res;
	}
	// �������׶�
	std::string res;
	for(int k=0;k<capacity;k++)
	{
		if(segments[k])
		{
			segments[k]->mut.lock();
			res=res+segments[k]->toString()+"\n";
		}
	}
	for(int k=0;k<capacity;k++)
	{
		segments[k]->mut.unlock();
	}
	return res;
}

/** 
 * first_hash: ��һ�ι�ϣ��һ�ι�ϣ�� 
 * note 1: ��Segment��������capacity��ֵΪ2����ʱ��ʼ���У�code%capacity==code&(capacity)���Ӷ������������� 
 * note 2: ��һ�ι�ϣ��Ŀ����ȷ��Ҫ������Segment��λ���±� 
 * note 3: ���ι�ϣ���㷨��һ������һ�ι�ϣ�Ĺ��̽�Ϊ���⣬�������ι�ϣ֮ǰ���Ƽ���jenkinsԤ���� 
 */ 
template<typename T, typename R>
unsigned int ConcurrentStrongHashMap<T, R>::first_hash(unsigned int code) const
{
	return code&(capacity-1);
}

/**
 * jenkins_prehash: ��ϣԤ����
 * note: ���ι�ϣ֮ǰ����Ҫ���й�ϣԤ����Ԥ�����Ŀ���ǳ�ֻ��ӣ��Ӷ��Ż���ϣ��Ч�� 
 */
template<typename T, typename R>
unsigned int ConcurrentStrongHashMap<T, R>::jenkins_prehash(unsigned int h) const
{
	h += (h << 15) ^ 0xffffcd7d; 
	h ^= (h >> 10);
	h += (h << 3); h ^= (h >> 6);
	h += (h << 2) + (h << 14); 
	return h ^ (h >> 16);
}

/**
 * ~ConcurrentStrongHashMap: �������� 
 */ 
template<typename T, typename R>  
ConcurrentStrongHashMap<T, R>::~ConcurrentStrongHashMap()
{
	for(int i=0;i<capacity;i++) 
	{
		if(segments[i])
		delete segments[i];
	} 
	delete [] segments;
}


#endif
