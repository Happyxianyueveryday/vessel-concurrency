#ifndef ZIP_HASH_MAP_H          
#define ZIP_HASH_MAP_H       
#include "HashMap.h"
#include <iostream>
#include <string>
#include <vector> 

/** 
 * HashEntry: ��ϣ���д洢�ļ�ֵ�� 
 * typename T: ��ϣ���������
 * typename R: ��ϣ��ֵ������ 
 */ 
template <typename T, typename R>
struct HashEntry
{
    HashEntry(T _key=T(), R _val=R()):key(_key), val(_val), next(nullptr)
    {
    }
    const T key;        // ������ֵ���еļ������޸�
    R val;              // ֵ����ֵ���е�ֵ�����޸�  
    HashEntry *next;    // ָ��������һ�����/��ֵ�Ե�ָ��
};

/**
 * ZipHashMap: �����������Ĺ�ϣ�� 
 * typename T: ��ϣ���������
 * typename R: ��ϣ��ֵ������ 
 */
template <typename T, typename R>
class ZipHashMap: public HashMap<T, R> 
{
    public:
    // 1. constructor
    ZipHashMap();                                                 // Ĭ�Ϲ��캯��������յĹ�ϣ�� 
    ZipHashMap(const std::vector<T> &_keys, const std::vector<R> &_values); // �����鹹���ϣ��

    // 2. copy/move_controller
    ZipHashMap(const ZipHashMap &other);                   // �������캯��������� 
    ZipHashMap &operator= (const ZipHashMap &other);       // ������ֵ������������ 
    ZipHashMap(ZipHashMap &&other);                        // �ƶ����캯�� ��ǳ����/�ƶ���
	ZipHashMap &operator= (ZipHashMap &&other);            // �ƶ���ֵ�������ǳ����/�ƶ��� 

    // 3. methods
    void insert(T key, R value) override;         // ���ϣ���в���ָ��ֵ
    void remove(T key) override;                  // �ӹ�ϣ�����Ƴ�ָ��ֵ
    bool contain(T key) const override;           // �жϹ�ϣ�����Ƿ����ָ��ֵ
    bool get(T key, R &value) const override;     // ���ҹ�ϣ���и�������Ӧ��ֵ 
    std::vector<T> keys() const override;              // ��ù�ϣ��������ֵ���б�
    int length() const override;                  // ��ȡ��ϣ���Ԫ�ظ��� 
    bool operator== (const ZipHashMap &other);    // ����ж������

    // 4. destructor
    ~ZipHashMap();                                // ��ϣ������������������

    private:
    // 5. domains
    HashEntry<T, R> **data;       // ��ϣ������ṹ
    unsigned int capacity;        // ��ϣ��Ͱ��������������ĳ��ȣ����������У���ȡ��Ԫ�صĹ�ϣֵ�󽫹�ϣֵģͰ�������Ϳ��Եõ�����Ĵ�ŵ�Ͱ���±�λ��
    double load_factor;           // ��ϣ��������
    unsigned int threshold;       // ��ϣ��������ֵ����ֵ=Ͱ����*�������ӣ�ͨ������������ϣ������ݲ����ǣ���ִ��insert��������Ԫ�غ󣬼���ϣ���е�Ԫ�ظ���size�Ƿ������ֵthreshold�������ڣ������resize�����������ݣ���������ݷ�����μ�resize����
    unsigned int size;            // ��ǰ��ϣ���е�Ԫ�ظ���

    // 6. private functions
    void resize();       // ��ϣ�����ݷ���
    unsigned int hash(unsigned int input) const;             // ����JenkinsԤ�����ļ��Ĺ�ϣֵ�������ڵ�Ͱ���±� 
    unsigned int jenkins_prehash(unsigned int input) const;     // Jenkins��ϣֵԤ��������hashCode()����ֵ���뾭���ò�Ԥ���� 
};

/**
 * ZipHashMap: Ĭ�Ϲ��캯��������յĹ�ϣ�� 
 */
template<typename T, typename R>
ZipHashMap<T, R>::ZipHashMap()
{
	size=0;    									// ��ʼ��Ԫ�ظ���Ϊ0
	capacity=16;        						// Ĭ��Ͱ��������ʼֵΪ16��16Ϊ2���ݣ�����ÿ�����ݾ�����2����Ͱ������Ϊ2���������� 
	load_factor=0.75;   						// Ĭ�ϵĸ������ӳ�ʼֵΪ0.75
	threshold=capacity*load_factor;        		// ��ϣ��������ֵ����ֵʼ�յ���Ͱ�������Ը�������
	data=new HashEntry<T, R>* [capacity];     	// ��ʼ��Ͱ 
	
	for(int i=0;i<capacity;i++)
	{
		data[i]=nullptr;     // ��ʼ��ÿ��Ͱ�е�����Ϊ�� 
	}
}

/**
 * ZipHashMap: �����鹹���ϣ���� 
 * param data: ���ڹ����ԭʼ�������� 
 */
template<typename T, typename R>
ZipHashMap<T, R>::ZipHashMap(const std::vector<T> &_keys, const std::vector<R> &_values)
{
	size=0;    									// ��ʼ��Ԫ�ظ���Ϊ0
	capacity=16;        						// Ĭ��Ͱ��������ʼֵΪ16��16Ϊ2���ݣ�����ÿ�����ݾ�����2����Ͱ������Ϊ2���������� 
	load_factor=0.75;   						// Ĭ�ϵĸ������ӳ�ʼֵΪ0.75
	threshold=capacity*load_factor;        		// ��ϣ��������ֵ����ֵʼ�յ���Ͱ�������Ը�������
	data=new HashEntry<T, R>* [capacity];     	// ��ʼ��Ͱ
	
	for(int i=0;i<capacity;i++)
	{
		data[i]=nullptr;     // ��ʼ��ÿ��Ͱ�е�����Ϊ�� 
	}
	
	for(int i=0;i<_keys.size();i++)
	{
		this->insert(_keys[i], _values[i]);
	}
}

/**
 * ZipHashMap: �������캯�� 
 */
template<typename T, typename R>
ZipHashMap<T, R>::ZipHashMap(const ZipHashMap &other)
{
	size=other.size;    									
	capacity=other.capacity;        						
	load_factor=other.load_factor;   						
	threshold=capacity*load_factor;
	data=new HashEntry<T, R>* [capacity];     		
	
	for(int i=0;i<capacity;i++)
	{
		if(other.data[i])
		{
			data[i]=new HashEntry<T, R> (other.data[i]->key, other.data[i]->val);
			HashEntry<T, R> *now=data[i], *othnow=other.data[i]->next;
			while(othnow)
			{
				now->next=new HashEntry<T, R> (othnow->key, othnow->val);
				now=now->next;
				othnow=othnow->next; 
			}
		}
		else
		data[i]=nullptr;
	}
}

/**
 * operator=: ������ֵ����� 
 */
template<typename T, typename R>
ZipHashMap<T, R> &ZipHashMap<T, R>::operator= (const ZipHashMap &other)
{
	if(this==&other)
	return (*this);
	
	for(int i=0;i<capacity;i++)
	{
		HashEntry<T, R> *now=data[i];
		while(now)
		{
			HashEntry<T, R> *temp=now;
			now=now->next;
			delete temp;
		}
	}
	delete data;
	
	size=other.size;    									
	capacity=other.capacity;        						
	load_factor=other.load_factor;   						
	threshold=capacity*load_factor;
	data=new HashEntry<T, R>* [capacity];     		
	
	for(int i=0;i<capacity;i++)
	{
		if(other.data[i])
		{
			data[i]=new HashEntry<T, R> (other.data[i]->key, other.data[i]->val);
			HashEntry<T, R> *now=data[i], *othnow=other.data[i]->next;
			while(othnow)
			{
				now->next=new HashEntry<T, R> (othnow->key, othnow->val);
				now=now->next;
				othnow=othnow->next; 
			}
		}
		else
		data[i]=nullptr;
	}
	
	return (*this);
}

/**
 * ZipHashMap: �ƶ����캯�� 
 */
template<typename T, typename R>
ZipHashMap<T, R>::ZipHashMap(ZipHashMap<T, R> &&other) 
{
	// 1. �ӿ���Դ����ǳ������Ա������ֵ����ǰ���� 
	size=other.size;           // ��ϣ��Ԫ�ظ��� 
	capacity=other.capacity;   // ��ϣ��Ͱ����
	load_factor=other.load_factor;   // װ������
	threshold=capacity*load_factor;  // ������ֵ 
	data=other.data;                 // �ײ�洢�ṹָ��
	
	// 2. ������Դ����ĳ�Ա������ֵ�趨ΪĬ��ֵ����Ĭ�Ϲ��캯���е�ȡֵ��
	other.size=0;    								
	other.capacity=16;        					
	other.load_factor=0.75;   					
	other.threshold=capacity*load_factor;        		
	other.data=new HashEntry<T, R>* [capacity];     			
	
	for(int i=0;i<capacity;i++)
	{
		other.data[i]=nullptr;
	}
}

/**
 * operator=: �ƶ���ֵ�����������cpp11���ƶ������ṩ��ȫ��ǳ����/�ƶ��ֶ�  
 */ 
template<typename T, typename R>
ZipHashMap<T, R> &ZipHashMap<T, R>::operator= (ZipHashMap &&other)
{
	// 1. �ж��Ƿ�Ϊ����ֵ
	if(this==&other)
	return (*this);
	
	// 2. �ͷ����������ʹ�õĶѿռ���Դ
	for(int i=0;i<capacity;i++)
	{
		HashEntry<T, R> *now=data[i];
		while(now)
		{
			HashEntry<T, R> *temp=now;
			now=now->next;
			delete temp;
		}
	}
	
	// 3. �ӿ���Դ����ǳ������Ա������ֵ����ǰ���� 
	size=other.size;           // ��ϣ��Ԫ�ظ��� 
	capacity=other.capacity;   // ��ϣ��Ͱ����
	load_factor=other.load_factor;   // װ������
	threshold=capacity*load_factor;  // ������ֵ 
	data=other.data;                 // �ײ�洢�ṹָ��
	
	// 4. ������Դ����ĳ�Ա������ֵ�趨ΪĬ��ֵ����Ĭ�Ϲ��캯���е�ȡֵ��
	other.size=0;    								
	other.capacity=16;         	 				
	other.load_factor=0.75;   					
	other.threshold=capacity*load_factor;        		
	other.data=new HashEntry<T, R>* [capacity];     			
	for(int i=0;i<capacity;i++)
	{
		other.data[i]=nullptr;
	}
	
	// 5. ���ص�ǰ���������
	return (*this);
}

/**
 * insert: ���ϣ���в����ֵ�� 
 * note: �������Ԫ��ֵ�ڹ�ϣ�������Ѿ������ظ����򲻻���в��룬ֻ�Ǹ��¸ü���Ӧ��ֵ 
 */
template<typename T, typename R>
void ZipHashMap<T, R>::insert(T key, R value) 
{
	// 1. ����Ӧ�������Ͱλ��
	unsigned int index=hash(key.hashCode()); 
	// 2. ������λ�õ�Ͱ�е�������������ͬ�ļ�������¸ü���Ӧ��ֵ��������ͬ�ļ�����������β�������µļ�ֵ�ԣ������Ӽ�ֵ�Լ�������Ҫע���ر���ͰΪ�յ���� 
	HashEntry<T, R> *now=data[index], *nowprev=nullptr;
	while(now) 
	{
		if(now->key.hashCode()==key.hashCode()&&now->key.equals(key))
		{				
			now->val=value;   // ��ϣ�����Ѿ����ڸü�������¸ü���Ӧ��ֵ���������Ӽ�ֵ�Լ��������� 
			return;
		}
		else
		{
			nowprev=now; 
			now=now->next;
		}
	}
	if(data[index])
	nowprev->next=new HashEntry<T, R> (key, value);
	else
	data[index]=new HashEntry<T, R> (key, value);
	// 3. ����Ƿ���Ҫ�������� 
	size+=1; 
	if(size>threshold)
	resize();
	return; 
}

/**
 * remove: �ӹ�ϣ������ɾ��ָ��ֵ��Ԫ��
 * note: ����ϣ�����в����ڸ�ֵ����ʵ���ϲ������ɾ�� 
 */
template<typename T, typename R>
void ZipHashMap<T, R>::remove(T key) 
{
	// 1. ����Ӧ�������Ͱλ��
	unsigned int index=hash(key.hashCode()); 
	// 2. ��ͰΪ�գ���ɾ��ʧ�ܣ��������β���Ͱ�е������ҵ�����ȵĽ�㼴����ɾ��
	if(data[index]) 
	{
		HashEntry<T, R> *now=data[index], *nowprev=nullptr;
		while(now)
		{
			if(now->key.hashCode()==key.hashCode()&&now->key.equals(key))
			{
				HashEntry<T, R> *temp=now;
				if(nowprev)
				nowprev->next=now->next;
				else
				data[index]=now->next;  
				
				delete temp;
				size-=1;
				return; 
			}
			else
			{
				nowprev=now;
				now=now->next;
			}
		}	
	}
	else
	return;
}

/**
 * contain: �жϸ���ֵ�Ƿ��ڹ�ϣ����
 * note: ������ֵ�ڹ�ϣ�����򷵻�true�����򷵻�false 
 */
template<typename T, typename R>
bool ZipHashMap<T, R>::contain(T key) const
{
	if(!size)
	return false;
	for(int i=0;i<capacity;i++)
	{
		HashEntry<T, R> *now=data[i];
		while(now)
		{
			if(now->key.hashCode()==key.hashCode()&&now->key.equals(key))
			return true;
			else
			now=now->next;
		}
	}
	return false;
}

/**
 * get: ��ȡ��ϣ���и�������Ӧ��ֵ
 * param key: �����ҵļ�
 * output-param value: �����ҵ��˶�Ӧ�ļ�����ͨ���ò������ظü���Ӧ��ֵ
 * return: �����ҳɹ�������true��������ʧ�ܣ�����false 
 */
template<typename T, typename R>
bool ZipHashMap<T, R>::get(T key, R &value) const
{
	if(!size)
	return false;
	for(int i=0;i<capacity;i++)
	{
		HashEntry<T, R> *now=data[i];
		while(now)
		{
			if(now->key.hashCode()==key.hashCode()&&now->key.equals(key))
			{
				value=now->val;
				return true;
			}
			else
			now=now->next;
		}
	}
	return false;
}

/**
 * values: ��ȡ��ϣ�����е�����ֵ���б�
 * return: ��ϣ�����е�����Ԫ�ص��б� 
 */
template<typename T, typename R>
std::vector<T> ZipHashMap<T, R>::keys() const
{
	std::vector<T> res;
	if(!size)
	return res;
	for(int i=0;i<capacity;i++)
	{
		HashEntry<T, R> *now=data[i];
		while(now)
		{
			res.push_back(now->key);
			now=now->next;
		}
	}
	return res;
}

/**
 * length: ��ȡ��ϣ���Ԫ�ظ��� 
 */
template<typename T, typename R>
int ZipHashMap<T, R>::length() const
{
	return size;
}

/** 
 * operator==: �ж�������ϣ�����Ƿ����
 * note: �ж�������������ϣ���Ƿ���ȵ��㷨���ںܴ���Ż��ռ䣬��Ҫ������ȵ��Ż������������ơ�ͬʱ��Ҫ���ѵ��ǣ���Ϊ����ɾ���Ȳ�����˳����ͬ��������ͨ��ֱ�ӱȽ�������ϣ��Ľṹ���Ƚ�������ϣ���Ƿ����
 * note: �������ʹ�õıȽ��㷨���£�
 *       1. �����жϴ����otherָ���ַ�Ƿ��thisָ����ȣ�����ȣ���ֱ�ӷ���true��������벽��2
 *		 2. Ȼ���жϴ���Ĺ�ϣ�����е�Ԫ�������Ƿ�͵�ǰ��ϣ������ȣ�������ȣ���ֱ�ӷ���true��������벽��3
 *       3. ��������жϵ�ǰ��ϣ�����е�ÿһ����ֵ���Ƿ��ڹ�ϣ����other�У��������ڣ���ֱ�ӷ���true�����򷵻�false		 
 */ 
template<typename T, typename R>
bool ZipHashMap<T, R>::operator== (const ZipHashMap &other)
{
	if(&other==this)
	return true;
	else if(other.size!=size)
	return false;
	else
	{
		for(int i=0;i<capacity;i++)
		{
			HashEntry<T, R> *now=data[i];
			while(now)
			{
				R othval;
				if(other.get(now->key, othval)&&othval.equals(now->val))
				now=now->next;
				else
				return false;
			}
		}
		return true;
	}
}

/**
 * resize: ��ϣ���ݷ���
 * note 1: ��ʱ������ϣ���ݣ�
 *         ��ֻҪ��ִ�в�������󣬹�ϣ���е�Ԫ������size����Ͱ������capacity����һ����������ϵ��(load_factor)ʱ�ͷ�����ϣ���ݣ�����������Ϊ: size>capacity*load_factor 
 * note 2: ��ϣ���ݾ����㷨���裺 
 *         ��1. ��Ͱ������(capacity)����Ϊԭ�ȵ���������: capacity=2*capacity
 *			   2. ���¼�����ֵ����: threshoud=capacity*load_factor������load_factorΪ�������ӣ��̶�Ϊ0.75 
 *			   3. �����µ�Ͱ����
 *			   4. ����Ͱ�����е���������֮ǰ��ͬ�㷨���¼����ϣֵ�Ͳ���λ�ã���ʹ��ǳ�����ƶ�����Ͱ���飬��: index=(element->hashCode())%capacity 
 *			   5. �ͷž�Ͱ���飬�µ�Ͱ������Ϊ��ǰ��ϣ���Ͱ���� 
 */
template<typename T, typename R>
void ZipHashMap<T, R>::resize() 
{
	// 1. ��Ͱ����capacity�ӱ�
	unsigned int old_capacity=capacity; 
	capacity*=2;
	// 2. ���¼�����ֵthreshoud
	threshold=capacity*load_factor;
	// 3. �����µ�Ͱ����
	HashEntry<T, R> **new_data=new HashEntry<T, R>* [capacity]; 
	for(int i=0;i<capacity;i++)
	{
		new_data[i]=nullptr; 
	}
	// 4. ��ԭͰ�����е�ÿ����ֵ������ʹ����ԭ����ͬ�Ĺ�ϣ�㷨�ƶ��������ǿ��������µ�Ͱ������ 
	for(int i=0;i<old_capacity;i++)
	{
		while(data[i])
		{
			unsigned int index=hash(data[i]->key.hashCode());        // �����µĹ�ϣ��ַ��λ�� 
			HashEntry<T, R> *temp=data[i]->next;
			if(new_data[index])
			{
				data[i]->next=new_data[index];
				new_data[index]=data[i];
			}
			else
			{
				data[i]->next=nullptr;      // ע���ƶ��Ľ����Ҫ��nextָ���ÿ� 
				new_data[index]=data[i]; 
			} 
			data[i]=temp;
		}
		data[i]=nullptr;       // ԭͰ�����е�ͷ�������ƽ���ÿ� 
	}
	delete [] data;
	data=new_data;
}

/** 
 * hash: ���ݶ����hashCode���صĹ�ϣֵ�������Ͱ�е�λ�� 
 * note: �÷����õ���λ���±�Ӧ�������ܵؾ��ȷ�ɢ�����jdk��ʹ�õ�ʵ�ַ�ʽ�ǣ�
 *       1. ��ϣֵ�Ŷ�����hashCode����16λ�Ľ����ԭ����hashCode���������õ����fianlhash�����͹�ϣֵ�ĳ�ͻ�� 
 *       2. ȡģ���㣺��finalhash%capacity������capacity��2���������ݣ���˸��������ʹ��Ч�ʽϸߵ�finalhash&(capacity-1)��λ����ȡ�� 
 */ 
template<typename T, typename R>
unsigned int ZipHashMap<T, R>::hash(unsigned int code) const
{
	return jenkins_prehash(code)&(capacity-1);
}

/**
 * jenkins_prehash: ��ϣԤ����
 * note: ���ι�ϣ֮ǰ����Ҫ���й�ϣԤ����Ԥ�����Ŀ���ǳ�ֻ��ӣ��Ӷ��Ż���ϣ��Ч�� 
 */
template<typename T, typename R>
unsigned int ZipHashMap<T, R>::jenkins_prehash(unsigned int h) const
{
	h += (h << 15) ^ 0xffffcd7d; 
	h ^= (h >> 10);
	h += (h << 3); h ^= (h >> 6);
	h += (h << 2) + (h << 14); 
	return h ^ (h >> 16);
}

/**
 * ~ZipHashMap: ���������� 
 */
template<typename T, typename R>
ZipHashMap<T, R>::~ZipHashMap()
{
	for(int i=0;i<capacity;i++)
	{
		HashEntry<T, R> *now=data[i];
		while(now)
		{
			HashEntry<T, R> *temp=now;
			now=now->next;
			delete temp;
		}
	}
	delete [] data;
}

#endif
