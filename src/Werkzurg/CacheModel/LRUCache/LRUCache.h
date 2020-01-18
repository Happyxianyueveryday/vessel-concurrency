#ifndef LRU_CACHE_H
#define LRU_CACHE_H 
#include "ZipHashMap.h"
#include <iostream>
#include <string>
#include <utility>

/**
 * ListNode: �������ʹ�û����˫��������
 * note: ÿ���洢��㱣��һ����ֵ�ԣ��������ʹ�û�����ʹ�õĴ洢����Ϊ˫�������� 
 */
template<typename T, typename R>
struct ListNode
{
	ListNode(T _key=T(), R _value=R()):key(_key), value(_value), next(nullptr), prev(nullptr)
	{
	}
	T key;     // ���洢�ļ� 
	R value;   // ���洢��ֵ 
	ListNode<T, R> *next;   // ��һ������ָ�� 
	ListNode<T, R> *prev;   // ��һ������ָ�� 
};

/**
 * LRUCache: �������ʹ�û���
 * note: �ڻ����Ѿ���ʱ���������ʹ�û���ʹ��������ٹ����������������ʹ�õļ�ֵ�ԣ�������֧��дֱ�﷨��д�ط�����д���� 
 */
template<typename T, typename R>
class LRUCache
{
	public:
	// 0. sign
	const static bool WRITE_BACK=false;    // д�ط���־���趨����ʹ��д�ط� 
	const static bool WRITE_THROUGH=true;  // дֱ�﷨��־���趨����ʹ��дֱ�﷨ 
	
	// 1. constructor
	LRUCache(unsigned int _max_size, bool _write_mode=LRUCache::WRITE_BACK, bool (*_noncache_read_callback)(const T &key, R &value)=nullptr, void (*_noncache_write_callback)(const T &key, const R &value)=nullptr);    // ��ʼ���캯�� 
	
	// 2. copy/move controller
	LRUCache(const LRUCache<T, R> &other);               	   // �������캯�� 
	LRUCache<T, R> &operator= (const LRUCache<T, R> &other);   // ������ֵ����� 
	LRUCache(LRUCache<T, R> &&other); 				           // �ƶ����캯��
	LRUCache<T, R> &operator= (LRUCache<T, R> &&other);        // �ƶ���ֵ����� 
	
	// 3. methods
	void write(const T &key,const R &value);    // �򻺴�д����� 
	bool read(const T &key, R &value);     		// �ӻ����ȡ���� 
	std::vector<std::pair<T, R>> all();         // ������������еĶ���
	
	// 4. destructor
	~LRUCache();    // �������� 
	
	private:
	// 5. domain
	unsigned int max_size;                // �������޴�С 
	ZipHashMap<T, ListNode<T, R>*> dic;   // ��ӳ�䵽���������Ĺ�ϣ��
	bool write_mode;                      // ����д��ģʽ��WRITE_THROUGHΪдֱ�﷨��WRITE_BACKΪд�ط���Ĭ��ʹ��д�ط� 
	ListNode<T, R> *head;                 // ������������׽�� 
	ListNode<T, R> *tail;                 // �����������β���
	unsigned int size;                    // ��ǰ����������� 
	bool (*noncache_read_callback)(const T &key, R &value);        // ���ô洢��ȡ�ص�����
	void (*noncache_write_callback)(const T &key, const R &value); // ���ô洢д��ص�����  
	
	// 6. private methods
	void update(ListNode<T, R> *pos);     // �ڷ��ʽ���������ķ��ʼ�¼ 
};

/**
 * LRUCache: ��ʼ���캯�� 
 * param _max_size: LRU���������ֵ������ 
 * param _write_mode: LRU�����д��ģʽ��ֵΪLRUCache::WRITE_BACK��ʹ��д�ط���ֵΪLRUCache::WRITE_THROUGH��ʹ��дֱ�﷨��Ĭ��ʹ��д�ط�
 * param _noncache_read_callback: �ǻ����ȡ�ص�����ָ�룬���ڻ����ж�ȡ����Ŀ���ֵ��ʱ�����øûص����������ô洢�ж�ȡ��Ĭ��ֵΪ��ָ�� 
 * param _noncache_write_callback: �ǻ���д��ص�����ָ�룬���򻺴���д��Ŀ���ֵ�Ե���ֵʱ������ѡ��Ĳ�ͬ��д���ԣ����ʵ���ʱ����ֵ����д�����ô洢�У�Ĭ��ֵΪ��ָ�� 
 * note: ��ʼ���캯������һ���յ�LRU���棬�ÿջ������ָ���Ĵ�С�Լ�д��ģʽ 
 */
template<typename T, typename R>
LRUCache<T, R>::LRUCache(unsigned int _max_size, bool _write_mode, bool (*_noncache_read_callback)(const T &key, R &value), void (*_noncache_write_callback)(const T &key, const R &value))
{
	head=tail=nullptr;
	size=0;
	
	max_size=_max_size;
	write_mode=_write_mode;
	noncache_read_callback=_noncache_read_callback;
	noncache_write_callback=_noncache_write_callback;
}

/**
 * LRUCache: �������캯�� 
 */ 
template<typename T, typename R>
LRUCache<T, R>::LRUCache(const LRUCache<T, R> &other)
{
	max_size=other.max_size;
	dic=std::move(other.dic);
	write_mode=other.write_mode;
	size=other.size;
	noncache_read_callback=other.noncache_read_callback;
	noncache_write_callback=other.noncache_write_callback; 
	
	head=nullptr;
	tail=nullptr;
	
	if(other.head)
	{
		head=new ListNode<T, R>(other.head->key, other.head->value);
		ListNode<T, R> *now=head, *othnow=other.head->next;
		while(othnow)
		{
			now->next=new ListNode<T, R>(othnow->key, othnow->value);
			now->next->prev=now;
			now=now->next;
			othnow=othnow->next;
		}
		tail=now;
	}
}

/** 
 * operator=: ������ֵ����� 
 */
template<typename T, typename R>
LRUCache<T, R> &LRUCache<T, R>::operator= (const LRUCache<T, R> &other)
{
	// 1. �ж��ų����Ҹ�ֵ���
	if(&other==this) 
	return (*this);
	// 2. �ͷŵ�ǰ������ռ�õĶ��ڴ棬��ʹ��д�ط���Ҫд�� 
	ListNode<T, R> *now=head;
	while(now)
	{
		ListNode<T, R> *temp=now;
		now=now->next;
		if(!write_mode)	  // ���ʹ�õ���д�ط�����������������Ҫ����д��
		noncache_write_callback(temp->key, temp->value);
		delete temp;
	}
	// 3. �������Դ��������� 
	max_size=other.max_size;
	dic=other.dic;  
	size=other.size;
	noncache_read_callback=other.noncache_read_callback;
	noncache_write_callback=other.noncache_write_callback;
	write_mode=other.write_mode; 
	head=nullptr;
	tail=nullptr;
	if(other.head)
	{
		head=new ListNode<T, R>(other.head->key, other.head->value);
		ListNode<T, R> *now=head, *othnow=other.head->next;
		while(othnow)
		{
			now->next=new ListNode<T, R>(othnow->key, othnow->value);
			now->next->prev=now;
			now=now->next;
			othnow=othnow->next;
		}
		tail=now;
	} 
	// 4. ���ص�ǰ���������
	return (*this);
}

/**
 * LRUCache: �ƶ����캯�� 
 */
template<typename T, typename R>
LRUCache<T, R>::LRUCache(LRUCache<T, R> &&other)
{
	// 1. ǳ������Ա���� 
	max_size=other.max_size;
	dic=std::move(other.dic);
	write_mode=other.write_mode;
	head=other.head;
	tail=other.tail; 
	size=other.size;
	noncache_read_callback=other.noncache_read_callback;
	noncache_write_callback=other.noncache_write_callback; 
	
	// 2. ���ÿ���Դ��Ա���� 
	other.size=0;
	other.head=nullptr;
	other.tail=nullptr;
}

/** 
 * operator=: �ƶ���ֵ����� 
 */
template<typename T, typename R>
LRUCache<T, R> &LRUCache<T, R>::operator= (LRUCache<T, R> &&other)
{
	// 1. ������Ҹ�ֵ��� 
	if(&other==this)
	return (*this);
	
	// 2. �ͷŵ�ǰ����ĳ�Ա����
	ListNode<T, R> *now=head;
	while(now)
	{
		ListNode<T, R> *temp=now;
		now=now->next;
		delete temp;
	} 
	
	// 3. ����Դ����ĳ�Ա��������ǰ������ 
	max_size=other.max_size;
	dic=std::move(other.dic);
	write_mode=other.write_mode;
	head=other.head;
	tail=other.tail; 
	size=other.size;
	noncache_read_callback=other.noncache_read_callback;
	noncache_write_callback=other.noncache_write_callback; 
	
	// 4. ����Դ����ĳ�Ա���� 
	other.size=0;
	other.head=nullptr;
	other.tail=nullptr;
	
	// 5. ���ص�ǰ��������� 
	return (*this);
}

/**
 * update: ���½���ֵ�Եķ�����ʷ��¼ 
 * param pos: ��ǰ���ʵĽ��
 * note: �����������ʹ��Ľ���������������ṹ���׽�� 
 */
template<typename T, typename R>
void LRUCache<T, R>::update(ListNode<T, R> *pos)
{
	// 1. ������Ŀ����Ϊ�ջ�������Ŀ���㼴Ϊ������׽�� 
	if(!pos||!pos->prev)  
	return;
	// 2. ������Ŀ����ǿ��Ҳ�Ϊ�׽�㣬���ý���ԭλ���ƶ��������׽�� 
	else
	{
		ListNode<T, R> *prev=pos->prev, *next=pos->next;
		prev->next=next;
		if(next)     // next!=nullptr, ���pos����β��� 
		next->prev=prev;
		else         // next==nullptr, ���pos����β��㣬��Ҫ����β���tail����ֵ 
		tail=prev;
		
		pos->next=head;
		head->prev=pos;
		head=pos;
		return;
	}
}

/**
 * write: �򻺴�д����� 
 * param key: ��Ҫ�򻺴�д��ļ�
 * param value: ��Ҫ�򻺴�д���ֵ 
 */
template<typename T, typename R>
void LRUCache<T, R>::write(const T &key,const R &value)
{
	// 1. �ӻ����ֵ��в���Ŀ���ֵ�Եĵ�ַ�����ܹ����ҵ���Ӧ�ļ�ֵ�ԣ�����¼�ֵ���е�ֵ��д�������� 
	ListNode<T, R> *tar=nullptr;
	if(dic.get(key, tar)&&!tar->value.equals(value))
	{
		tar->value=value;
		update(tar);           // ��Ϊ�����˽��tar�����½��tar�ķ�����ʷ��¼ 
		if(write_mode==LRUCache::WRITE_THROUGH)   // ��ʹ����дֱ�﷨������Ҫ���µ�ֵд�����ô洢 
		noncache_write_callback(key, value); 
	}
	// 2. �����ܲ��ҵ���Ӧ�ļ�ֵ�ԣ���ֱ�ӽ��µ������ֵ��д�뻺�� 
	else
	{
		if(size==max_size)    // ������ռ��Ѿ�������С��max_sizeΪ1�������������ȡ�ļ�ֵ��֮ǰ������Ҫ����������ٷ��ʵļ�ֵ�� 
		{
			if(write_mode==LRUCache::WRITE_BACK)    // ��ʹ����д�ط���������������ļ�ֵ�Ե�ֵд�����ô洢 
			noncache_write_callback(tail->key, tail->value);
			
			dic.remove(tail->key);          // �����ڲ��ֵ�ӳ��
			dic.insert(key, tail);   
			
			update(tail);     // ���������ʹ�õĽ�㣨β��㣩���������׽��λ�� 
			head->key=key;    // ͨ���޸ĸý��ļ�ֵ�Ի���ԭ�ȵ��������ʹ�õĽ�� 
			head->value=value;
		}
		else    // ������ռ�δ������ֱ���򻺴�������������µĽ�㼴�� 
		{
			ListNode<T, R> *newnode=new ListNode<T, R>(key, value);
			dic.insert(key, newnode);
			newnode->next=head;
			
			if(head)
			head->prev=newnode;
			else
			tail=newnode;
			
			head=newnode;
			size+=1;
		}
		if(write_mode==LRUCache::WRITE_THROUGH)   // ��ʹ����дֱ�﷨������д��ļ�ֵ�Ե�ֵд�����ô洢 
		noncache_write_callback(key, value);
	}
}

/**
 * read: �ӻ����ȡ����
 * param key: ��Ҫ�ӻ����ȡ�ļ�
 * param value: �����������ӻ����ȡ���ļ��Ķ�Ӧֵ 
 */
template<typename T, typename R>
bool LRUCache<T, R>::read(const T &key, R &value)
{
	// 1. �ӻ����ֵ��в���Ŀ���ֵ�Եĵ�ַ�����ܹ����ҵ���Ӧ�ļ�ֵ�ԣ���ֱ�ӷ��ض�Ӧ��ֵ����ȡ������� 
	ListNode<T, R> *tar=nullptr;
	if(dic.get(key, tar))
	{
		value=tar->value;
		update(tar);           // ��Ϊ�����˽��tar�����½��tar�ķ�����ʷ��¼
		return true;
	}
	// 2. �����ܲ��ҵ���Ӧ�ļ�ֵ�ԣ�����Ҫ�����ô洢��ȡ�����ܹ���ȡ�ɹ��������ô洢�еĶ�Ӧ��ֵ��д�뻺���� 
	else if(noncache_read_callback(key, value))
	{
		if(size==max_size)    // ������ռ��Ѿ�������С��max_sizeΪ1����������������ô洢��ȡ�ļ�ֵ��֮ǰ������Ҫ����������ٷ��ʵļ�ֵ�� 
		{
			if(write_mode==LRUCache::WRITE_BACK)    // ��ʹ����д�ط���������������ļ�ֵ�Ե�ֵд�����ô洢 
			noncache_write_callback(tail->key, tail->value);
			
			dic.remove(tail->key);          // �����ڲ��ֵ�ӳ��
			dic.insert(key, tail);   
			
			update(tail);     // ���������ʹ�õĽ�㣨β��㣩���������׽��λ�� 
			head->key=key;    // ͨ���޸ĸý��ļ�ֵ�Ի���ԭ�ȵ��������ʹ�õĽ�� 
			head->value=value;
		}
		else    // ������ռ�δ������ֱ���򻺴�������������µĽ�㼴�� 
		{
			ListNode<T, R> *newnode=new ListNode<T, R>(key, value);
			dic.insert(key, newnode);
			newnode->next=head;
			
			if(head)
			head->prev=newnode;
			else
			tail=newnode;
			
			head=newnode;
			size+=1;
		}
		return true;
	}
	// 3. �����ô洢��ȡʧ�ܣ����ȡ����ʧ�� 
	else
	return false;
}

/**
 * all: �������沢���
 * return: �����д洢�ļ�ֵ���б� 
 */
template<typename T, typename R>
std::vector<std::pair<T, R>> LRUCache<T, R>::all()
{
	std::vector<std::pair<T, R>> res;
	ListNode<T, R> *now=head;
	
	while(now)
	{
		res.push_back(std::make_pair(now->key, now->value));
		now=now->next;
	}
	
	return res;
}

/**
 * ~LRUCache: ���������� 
 */
template<typename T, typename R>
LRUCache<T, R>::~LRUCache()
{
	ListNode<T, R> *now=head;
	while(now)
	{
		ListNode<T, R> *temp=now;
		
		if(write_mode==LRUCache::WRITE_BACK)    // ��ʹ����д�ط���������������ļ�ֵ�Ե�ֵд�����ô洢 
		noncache_write_callback(now->key, now->value);
		
		now=now->next;
		delete temp;
	}
}

#endif
