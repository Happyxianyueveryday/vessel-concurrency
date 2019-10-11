#ifndef CONCURRENT_STRONG_HASHSET_H       
#define CONCURRENT_STRONG_HASHSET_H   
#include "ConcurrentSet.h"
#include "Segment.h" 
#include <iostream>
#include <string>
#include <vector> 

template<typename T>
class ConcurrentStrongHashSet:ConcurrentSet<T>       
{
	public:
	// 1. constructor
	ConcurrentStrongHashSet(unsigned int _curr_level=16);
	ConcurrentStrongHashSet(const vector<T> &data, unsigned int _curr_level=16);
	
	// 2. copy/move_controller
    ConcurrentStrongHashSet(const ConcurrentStrongHashSet &other) = delete;                   // ���ÿ������캯��
    ConcurrentStrongHashSet &operator= (const ConcurrentStrongHashSet &other) = delete;       // ���ÿ�����ֵ�����
    ConcurrentStrongHashSet(ConcurrentStrongHashSet &&other) = delete;                        // �����ƶ����캯��
	ConcurrentStrongHashSet &operator= (ConcurrentStrongHashSet &&other) = delete;            // �����ƶ���ֵ�����

    // 3. methods
    void insert(T value) override;              // ���ϣ���в���ָ��ֵ
    void remove(T value) override;              // �ӹ�ϣ�����Ƴ�ָ��ֵ
    bool contain(T value) override;       		// �жϹ�ϣ�����Ƿ����ָ��ֵ
    vector<T> values() override;        		// ��ù�ϣ��������ֵ���б� 
    int length() override;                      // ��ȡ��ϣ���Ԫ�ظ��� 
	string toString() override; 	            // �����ϣ��
	~ConcurrentStrongHashSet();                 // �������� 
    
    private:
    // 4. domains
    Segment<T> **segments;                      // Segment������ 
    const unsigned int capacity;                // Segment�ε��������̶�Ϊ16�� 
    unsigned int curr_level;                    // дʱ�����ȼ���ʵ���Ͼ���Segment�ĸ�����Segment�ĸ����������Ĳ���д�̸߳��� 
    
    // 5. private functions
    unsigned int jenkins_prehash(unsigned int code) const;     // Ԥ�����ϣ�㷨��Wang/Jenkins hash�㷨
    unsigned int first_hash(unsigned int code) const;          // ��һ�ι�ϣ��ȷ����Ҫ�����Segmentλ���±� 
};

template<typename T>
ConcurrentStrongHashSet<T>::ConcurrentStrongHashSet(unsigned int _curr_level):capacity((_curr_level>0)?_curr_level:16)
{
	segments=new Segment<T>* [capacity];
	
	for(int i=0;i<capacity;i++)
	{
		segments[i]=nullptr;
	}
}

template<typename T>
ConcurrentStrongHashSet<T>::ConcurrentStrongHashSet(const vector<T> &data, unsigned int _curr_level):capacity((_curr_level>0)?_curr_level:16)
{
	segments=new Segment<T>* [capacity];
	for(int i=0;i<capacity;i++)
	{
		segments[i]=nullptr;
	}
	for(int i=0;i<data.size();i++)
	{
		this->insert(data[i]);
	}
}

template<typename T>
void ConcurrentStrongHashSet<T>::insert(T value)
{
	int index=first_hash(value.hashCode());
	if(!segments[index])
	segments[index]=new Segment<T>();
	segments[index]->insert(value);
}

template<typename T>
void ConcurrentStrongHashSet<T>::remove(T value)
{
	int index=first_hash(value.hashCode());
	if(!segments[index])
	return;
	else
	segments[index]->remove(value);
}

template<typename T>
bool ConcurrentStrongHashSet<T>::contain(T value)
{
	int index=first_hash(value.hashCode());
	if(segments[index])
	return segments[index]->contain(value);
	else
	return false;
}

template<typename T>
vector<T> ConcurrentStrongHashSet<T>::values()
{
	vector<T> res;
	for(int i=0;i<capacity;i++)
	{
		if(segments[i])
		{
			vector<T> temp=segments[i]->values();
			for(int k=0;k<temp.size();k++)
			{
				res.push_back(temp[k]);
			}
		}
	}
	return res;
}

template<typename T>
int ConcurrentStrongHashSet<T>::length()
{
	int add=0;
	for(int i=0;i<capacity;i++)
	{
		if(segments[i])
		add+=segments[i]->length();
	}
	return add;
}

/**
 * toString: �����ϣ��
 * note: ���һ��Segment֮ǰ�����ø�Segment�Ļ����� 
 */ 
template<typename T>
string ConcurrentStrongHashSet<T>::toString()
{
	string res;
	for(int i=0;i<capacity;i++)
	{
		if(segments[i])
		res=res+segments[i]->toString()+"\n";
	}
	return res;
}

/** 
 * first_hash: ��һ�ι�ϣ��һ�ι�ϣ�� 
 * note 1: ��Segment��������capacity��ֵΪ2����ʱ��ʼ���У�code%capacity==code&(capacity)���Ӷ������������� 
 * note 2: ��һ�ι�ϣ��Ŀ����ȷ��Ҫ������Segment��λ���±� 
 * note 3: ���ι�ϣ���㷨��һ������һ�ι�ϣ�Ĺ��̽�Ϊ���⣬�������ι�ϣ֮ǰ���Ƽ���jenkinsԤ���� 
 */ 
template<typename T>
unsigned int ConcurrentStrongHashSet<T>::first_hash(unsigned int code) const
{
	return code&(capacity-1);
}

/**
 * jenkins_prehash: ��ϣԤ����
 * note: ���ι�ϣ֮ǰ����Ҫ���й�ϣԤ����Ԥ�����Ŀ���ǳ�ֻ��ӣ��Ӷ��Ż���ϣ��Ч�� 
 */
template<typename T>
unsigned int ConcurrentStrongHashSet<T>::jenkins_prehash(unsigned int h) const
{
	h += (h << 15) ^ 0xffffcd7d; 
	h ^= (h >> 10);
	h += (h << 3); h ^= (h >> 6);
	h += (h << 2) + (h << 14); 
	return h ^ (h >> 16);
}

/**
 * ~ConcurrentStrongHashSet: �������� 
 */ 
template<typename T>
ConcurrentStrongHashSet<T>::~ConcurrentStrongHashSet()
{
	for(int i=0;i<capacity;i++) 
	{
		if(segments[i])
		delete segments[i];
	} 
	delete [] segments;
}


#endif
