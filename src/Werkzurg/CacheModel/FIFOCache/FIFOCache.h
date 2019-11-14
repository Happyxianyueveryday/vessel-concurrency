#ifndef FIFO_CACHE_H 
#define FIFO_CACHE_H
#include "ZipHashMap.h"
#include <iostream> 
#include <string>

/**
 * ListNode: FIFO����Ļ��������
 * note: FIFO�����ڵײ�洢������ʹ�õ����������ListNode�ǵ��������� 
 */
template<typename T, typename R>  
struct ListNode
{
	ListNode(T _key=T(), R _value=R()):key(_key), value(_value), next(nullptr)
	{
	}
	T key;       			// ��ֵ�Եļ� 
	R value;     			// ��ֵ�Ե�ֵ 
	ListNode<T, R> *next;   // ָ���̽���ָ�� 
};

/** 
 * FIFOCache: �����ȳ�����
 * note: FIFOCache�����л��������£�����ʵ��O(1)ʱ��Ķ�ȡ��д���������δ���л��棬���д����ʱ�临�Ӷ�ȡ�����ⲿ�洢�Ķ�д�ٶȣ�����д�ط�ʽ�ϣ�֧��ѡ��д�ط�����дֱ�﷨ 
 */
template<typename T, typename R>
class FIFOCache
{
	public:
	// 1. constructor
	FIFOCache(unsigned int _max_size, bool (*_noncache_read_callback)(const T &key, R &value)=nullptr, void (*_noncache_write_callback)(const T &key, const R &value)=nullptr, bool write_mode=false);
	
	// 2. copy/move controller
	FIFOCache(const FIFOCache &other);
	FIFOCache<T, R> &operator= (const FIFOCache<T, R> &other);
	FIFOCache(FIFOCache &&other);
	FIFOCache<T, R> &operator= (FIFOCache<T, R> &&other); 
	
	// 3. methods
	void write(const T &key,const R &value);    // �򻺴�д����� 
	bool read(const T &key, R &value);     		// �ӻ����ȡ���� 
	std::vector<std::pair<T, R>> all();                   // ������������еĶ���       
	
	// 4. destructor
	~FIFOCache();
	
	private:
	// 5. domain
	unsigned int max_size;    		      // �������޴�С
	ZipHashMap<T, ListNode<T, R>*> dic;   // ��ӳ�䵽���������Ĺ�ϣ��
	bool (*noncache_read_callback)(const T &key, R &value);        // ���ô洢��ȡ�ص�����
	void (*noncache_write_callback)(const T &key, const R &value); // ���ô洢д��ص�����
	bool write_mode;                      // ����д��ģʽ��trueΪдֱ�﷨��falseΪд�ط���Ĭ��ʹ��д�ط� 
	ListNode<T, R> *head;                 // ������������׽�� 
	ListNode<T, R> *tail;                 // �����������β���
	unsigned int size;                    // ��ǰ����������� 
};

/**
 * FIFOCache: �������캯��
 * param _max_size: ���������С 
 * param _noncache_read_callback: ���ô洢��ȡ�ص���������ȡ����ʱ�������в�����Ŀ���ֵ�ԣ������øûص�������ȡ������key�Ǵ����ô洢�洢��ȡʱ�ļ�������value������������ص�������ȡ����ֵ�Ӹò������أ��������ô洢�洢��ȡ�ɹ����򷵻�true�����򷵻�false 
 * param _noncache_write_callback: ���ô洢д��ص�������д�뻺��ʱ�������øûص�������Ŀ��ֵд�����ô洢�洢�� 
 * param _write_mode: �������ô洢�洢��һ����д�뷽ʽ��trueʹ��дֱ�﷨��falseʹ��д�ط���Ĭ��ֵΪfalse 
 */
template<typename T, typename R>
FIFOCache<T, R>::FIFOCache(unsigned int _max_size, bool (*_noncache_read_callback)(const T &key, R &value), void (*_noncache_write_callback)(const T &key, const R &value), bool _write_mode): max_size(_max_size), noncache_read_callback(_noncache_read_callback), noncache_write_callback(_noncache_write_callback), write_mode(_write_mode), head(nullptr), tail(nullptr), size(0)
{
}

/**
 * FIFOCache: �������캯�� 
 */ 
template<typename T, typename R>
FIFOCache<T, R>::FIFOCache(const FIFOCache &other)
{
	// 1. �������Դ��������� 
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
FIFOCache<T, R> &FIFOCache<T, R>::operator= (const FIFOCache<T, R> &other)
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
			now=now->next;
			othnow=othnow->next;
		}
		tail=now;
	} 
	// 4. ���ص�ǰ���������
	return (*this);
}

/**
 * FIFOCache: �ƶ����캯�� 
 */
template<typename T, typename R>
FIFOCache<T, R>::FIFOCache(FIFOCache<T, R> &&other)
{
	// 1. ǳ��������Դ��������� 
	max_size=other.max_size;
	dic=std::move(other.dic);  
	noncache_read_callback=other.noncache_read_callback;
	noncache_write_callback=other.noncache_write_callback;
	write_mode=other.write_mode;                
	head=other.head;          
	tail=other.tail;
	size=other.size;
	// 2. �ÿտ���Դ����Ļ��棬���ǲ��ÿ�Դ��������ô洢��ȡ��д��ص�����ָ�� 
	other.head=nullptr;
	other.tail=nullptr;
	other.size=0;
}


template<typename T, typename R>
FIFOCache<T, R> &FIFOCache<T, R>::operator= (FIFOCache<T, R> &&other)
{
	// 1. �ж����Ҹ�ֵ��� 
	if(this==&other)
	return (*this);
	// 2. �ͷŵ�ǰ����ռ�õĶ��ڴ棬��ʹ��д�ط���Ҫд��  
	ListNode<T, R> *now=head;
	while(now)
	{
		ListNode<T, R> *temp=now;
		now=now->next;
		if(!write_mode)	  // ���ʹ�õ���д�ط�����������������Ҫ����д��
		noncache_write_callback(temp->key, temp->value);
		delete temp;
	}
	// 3. ǳ��������Դ��������� 
	max_size=other.max_size;
	dic=std::move(other.dic);  
	noncache_read_callback=other.noncache_read_callback;
	noncache_write_callback=other.noncache_write_callback;
	write_mode=other.write_mode;                
	head=other.head;          
	tail=other.tail;
	size=other.size;
	// 4. �ÿտ���Դ����Ļ��棬���ǲ��ÿ�Դ��������ô洢��ȡ��д��ص�����ָ�� 
	other.head=nullptr;
	other.tail=nullptr;
	other.size=0;
	// 5. ���ص�ǰ��������� 
	return (*this);
}

/**
 * write: ������߸��»����еĶ���
 * param key: �����»��߱���ļ�ֵ���еļ�
 * param value: �����»��߱���ļ�ֵ���е�ֵ  
 * note 1: ��ʹ��дֱ�﷨��write�����Ļ���д���߼�����: 
 * 		   1. ��Ҫд��ļ��ڻ����У���ֱ�Ӹ��»����еĶ�Ӧֵ����ʹ����дֱ�﷨������ŵ���noncache_write_callbackд���ô洢 
 *         2. ��Ҫд��ļ����ڻ����У����ڻ����д���һ���µļ�ֵ�ԣ���Ҫʱ������һ�����뻺��ļ�ֵ�ԣ���ʹ����дֱ�﷨������ŵ���noncache_write_callbackд���ô洢����ʹ����д�ط�����ͬ����һ�����뻺��ļ�ֵ�Ե�����ֵ�����ô洢�� 
 */
template<typename T, typename R>
void FIFOCache<T, R>::write(const T &key,const R &value)
{
	// 1. �����ҵļ�ֵ���ڻ����У���д��ɹ�����true 
	ListNode<T, R> *tar; 
	if(dic.get(key, tar))    // ���ϣ���ȡ��ֵ�����洢�ڵ��������ַ 
	{
		tar->value=value;    // ���»����ж�Ӧ��ֵ���е�ֵ
		if(write_mode&&noncache_write_callback)   // ��ʹ����дֱ�﷨����ͬ��д�����ô洢
		noncache_write_callback(key, value);
	}
	// 2. �����ҵļ�ֵ�Բ��ڻ�����:
	else
	{ 
		// 2.1 �ж��Ƿ���Ҫ������һ�����뻺��ļ�ֵ��
		if(size==max_size)        
		{
			ListNode<T, R> *temp=head;
			// ���ʹ����д�ط�����������ļ�ֵ��ʱ�������µļ�ֵ��д�����ô洢
			if(!write_mode&&noncache_write_callback) 
			noncache_write_callback(temp->key, temp->value);
			// �ӹ�ϣ�����Ƴ���������ļ�-��ַӳ��
			dic.remove(temp->key);   
			// ����ֻ��һ����㣬��ɾ����β����ÿ� 
			if(head==tail)       
			tail=nullptr;
			// ����ֱ��ɾ������ͷ��� 
			head=head->next;
			--size;
			delete temp;
		}
		// 2.2 ���µļ�ֵ��д�뻺�棬������Լ��뵽����ĩβ
		if(head==nullptr)
		{
			head=new ListNode<T, R>(key, value);
			tail=head;
			dic.insert(key, tail);
		}
		else
		{
			tail->next=new ListNode<T, R>(key, value);
			tail=tail->next;
			dic.insert(key, tail);
		}
		++size;
		if(write_mode&&noncache_write_callback)   // ��ʹ����дֱ�﷨����ͬ��д�����ô洢
		noncache_write_callback(key, value);
	} 
} 

/**
 * read: �ӻ����ж�ȡ����
 * param key: ����ȡ�ļ�ֵ���е�ֵ 
 * param value: �����������ȡ���Ķ�Ӧ����ֵ
 * note 1: read�����Ļ����ȡ�߼�����:
 * 		   1. ��Ҫ��ȡ�ļ��ڻ����У���ֱ�ӷ��ػ����еĶ�Ӧֵ����ȡ�ɹ�����true 
 *		   2. ��Ҫ��ȡ�ļ����ڻ�����: 
 * 		       (1). ��noncache_read_callback����ָ��ǿգ���ͨ������Ļص�����ָ�����noncache_read_callback������ȡ��ֵ��
 *       	        a. ������noncache_read_callbackҲû�в��ҵ����Ķ�Ӧֵ�����ȡʧ�ܣ�����false
 *				    b. ������noncache_read_callback���ҵ��˼��Ķ�Ӧֵ���һ����п�λ����ֱ��д�뻺�棬��ȡ�ɹ�����true���������޿�λ�����û���ȥ���ȶ��뻺��Ķ���Ȼ��д�뻺�棬��ȡ�ɹ�����true 
 *       	   (2). ��noncache_read_callback����ָ��Ϊ�գ����ȡʧ�ܣ�����false
 */
template<typename T, typename R>
bool FIFOCache<T, R>::read(const T &key, R &value)
{ 
	// 1. �����ҵļ�ֵ���ڻ����У����ȡ�ɹ�����true 
	ListNode<T, R> *tar; 
	if(dic.get(key, tar))    // ���ϣ���ȡ��ֵ�����洢�ڵ��������ַ 
	{
		value=tar->value;
		return true;
	}
	// 2. �����ҵļ�ֵ�Բ��ڻ����У������ô洢��ȡ�ص�����Ϊ�գ����ȡʧ�ܷ���false 
	else if(!noncache_read_callback)
	return false;
	// 3. �����ҵļ�ֵ�Բ��ڻ����У������ô洢��ȡ�ص������ǿգ������ô洢��ȡ�ص������ж�ȡֵʧ�ܣ����ȡʧ�ܷ���false 
	else if(!noncache_read_callback(key, value))
	return false;
	// 4. �����ҵļ�ֵ�Բ��ڻ����У������ô洢��ȡ�ص������ǿգ������ô洢��ȡ�ص������ж�ȡֵ�ɹ����򽫶�ȡ�ļ�ֵ�Լ��뻺���У��������Ѿ������û�������ļ�ֵ�� 
	else
	{
		// �����Ѿ������������������еĵ�һ��Ԫ�أ�Ҳ��������뻺���Ԫ��
		if(size==max_size)        
		{
			ListNode<T, R> *temp=head;
			// ���ʹ����д�ط�����������ļ�ֵ��ʱ�������µļ�ֵ��д�����ô洢
			if(!write_mode&&noncache_write_callback) 
			noncache_write_callback(temp->key, temp->value);
			// �ӹ�ϣ�����Ƴ���������ļ�-��ַӳ��
			dic.remove(temp->key);   
			// ����ֻ��һ����㣬��ɾ����β����ÿ� 
			if(head==tail)       
			tail=nullptr;
			// ����ֱ��ɾ������ͷ��� 
			head=head->next;
			--size;
			delete temp;
		}
		// �����»���Ľ��
		if(head==nullptr)
		{
			head=new ListNode<T, R>(key, value);
			tail=head;
			dic.insert(key, tail);
		}
		else
		{
			tail->next=new ListNode<T, R>(key, value);
			tail=tail->next;
			dic.insert(key, tail);
		}
		++size;
		return true;
	}
}
	
/**
 * all: ������������еļ�ֵ�� 
 * return: ��ֵ���б� 
 */
template<typename T, typename R>
std::vector<std::pair<T, R>> FIFOCache<T, R>::all()
{
	ListNode<T, R> *now=head;
	std::vector<std::pair<T, R>> res;
	while(now)
	{
		res.push_back(std::make_pair(now->key, now->value));
		now=now->next;
	}
	return res;
}

/** 
 * ~FIFOCache: ��������
 * note: ��ʹ��д�ط�������ͷ�ʱ�����еļ�ֵ�Ա�����ûصݺ���noncache_write_callbackͬ�������ô洢�� 
 */
template<typename T, typename R>
FIFOCache<T, R>::~FIFOCache()
{
	ListNode<T, R> *now=head;
	while(now)
	{
		ListNode<T, R> *temp=now;
		now=now->next;
		if(!write_mode)	  // ���ʹ�õ���д�ط�����������������Ҫ����д��
		noncache_write_callback(temp->key, temp->value);
		delete temp;
	}
}

#endif


