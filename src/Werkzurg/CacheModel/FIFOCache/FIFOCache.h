#ifndef FIFO_CACHE_H 
#define FIFO_CACHE_H
#include "ZipHashMap.h"
#include <iostream> 
#include <string>

/**
 * ListNode: FIFO缓存的缓存对象结点
 * note: FIFO缓存在底层存储机制上使用单向链表，因此ListNode是单向链表结点 
 */
template<typename T, typename R>  
struct ListNode
{
	ListNode(T _key=T(), R _value=R()):key(_key), value(_value), next(nullptr)
	{
	}
	T key;       			// 键值对的键 
	R value;     			// 键值对的值 
	ListNode<T, R> *next;   // 指向后继结点的指针 
};

/** 
 * FIFOCache: 先入先出缓存
 * note: FIFOCache的命中缓存的情况下，可以实现O(1)时间的读取，写入操作，若未命中缓存，则读写操作时间复杂度取决于外部存储的读写速度；而在写回方式上，支持选用写回法或者写直达法 
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
	void write(const T &key,const R &value);    // 向缓存写入对象 
	bool read(const T &key, R &value);     		// 从缓存读取对象 
	std::vector<std::pair<T, R>> all();                   // 遍历输出缓存中的对象       
	
	// 4. destructor
	~FIFOCache();
	
	private:
	// 5. domain
	unsigned int max_size;    		      // 缓存上限大小
	ZipHashMap<T, ListNode<T, R>*> dic;   // 键映射到缓存对象结点的哈希表
	bool (*noncache_read_callback)(const T &key, R &value);        // 外置存储读取回调函数
	void (*noncache_write_callback)(const T &key, const R &value); // 外置存储写入回调函数
	bool write_mode;                      // 缓存写入模式，true为写直达法，false为写回法，默认使用写回法 
	ListNode<T, R> *head;                 // 缓存对象链表首结点 
	ListNode<T, R> *tail;                 // 缓存对象链表尾结点
	unsigned int size;                    // 当前缓存对象数量 
};

/**
 * FIFOCache: 基础构造函数
 * param _max_size: 缓存的最大大小 
 * param _noncache_read_callback: 外置存储读取回调函数，读取缓存时若缓存中不存在目标键值对，将调用该回调函数获取。参数key是从外置存储存储读取时的键，参数value是输出参数，回调函数读取到的值从该参数返回；若从外置存储存储读取成功，则返回true，否则返回false 
 * param _noncache_write_callback: 外置存储写入回调函数，写入缓存时，将调用该回调函数将目标值写回外置存储存储中 
 * param _write_mode: 决定外置存储存储的一致性写入方式，true使用写直达法，false使用写回法，默认值为false 
 */
template<typename T, typename R>
FIFOCache<T, R>::FIFOCache(unsigned int _max_size, bool (*_noncache_read_callback)(const T &key, R &value), void (*_noncache_write_callback)(const T &key, const R &value), bool _write_mode): max_size(_max_size), noncache_read_callback(_noncache_read_callback), noncache_write_callback(_noncache_write_callback), write_mode(_write_mode), head(nullptr), tail(nullptr), size(0)
{
}

/**
 * FIFOCache: 拷贝构造函数 
 */ 
template<typename T, typename R>
FIFOCache<T, R>::FIFOCache(const FIFOCache &other)
{
	// 1. 深拷贝拷贝源对象的数据 
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
 * operator=: 拷贝赋值运算符 
 */ 
template<typename T, typename R>
FIFOCache<T, R> &FIFOCache<T, R>::operator= (const FIFOCache<T, R> &other)
{
	// 1. 判断排除自我赋值情况
	if(&other==this) 
	return (*this);
	// 2. 释放当前对象所占用的堆内存，若使用写回法需要写回 
	ListNode<T, R> *now=head;
	while(now)
	{
		ListNode<T, R> *temp=now;
		now=now->next;
		if(!write_mode)	  // 如果使用的是写回法，则析构函数中需要进行写回
		noncache_write_callback(temp->key, temp->value);
		delete temp;
	}
	// 3. 深拷贝拷贝源对象的数据 
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
	// 4. 返回当前对象的引用
	return (*this);
}

/**
 * FIFOCache: 移动构造函数 
 */
template<typename T, typename R>
FIFOCache<T, R>::FIFOCache(FIFOCache<T, R> &&other)
{
	// 1. 浅拷贝拷贝源对象的数据 
	max_size=other.max_size;
	dic=std::move(other.dic);  
	noncache_read_callback=other.noncache_read_callback;
	noncache_write_callback=other.noncache_write_callback;
	write_mode=other.write_mode;                
	head=other.head;          
	tail=other.tail;
	size=other.size;
	// 2. 置空拷贝源对象的缓存，但是不置空源对象的外置存储读取和写入回调函数指针 
	other.head=nullptr;
	other.tail=nullptr;
	other.size=0;
}


template<typename T, typename R>
FIFOCache<T, R> &FIFOCache<T, R>::operator= (FIFOCache<T, R> &&other)
{
	// 1. 判断自我赋值情况 
	if(this==&other)
	return (*this);
	// 2. 释放当前对象占用的堆内存，若使用写回法需要写回  
	ListNode<T, R> *now=head;
	while(now)
	{
		ListNode<T, R> *temp=now;
		now=now->next;
		if(!write_mode)	  // 如果使用的是写回法，则析构函数中需要进行写回
		noncache_write_callback(temp->key, temp->value);
		delete temp;
	}
	// 3. 浅拷贝拷贝源对象的数据 
	max_size=other.max_size;
	dic=std::move(other.dic);  
	noncache_read_callback=other.noncache_read_callback;
	noncache_write_callback=other.noncache_write_callback;
	write_mode=other.write_mode;                
	head=other.head;          
	tail=other.tail;
	size=other.size;
	// 4. 置空拷贝源对象的缓存，但是不置空源对象的外置存储读取和写入回调函数指针 
	other.head=nullptr;
	other.tail=nullptr;
	other.size=0;
	// 5. 返回当前对象的引用 
	return (*this);
}

/**
 * write: 保存或者更新缓存中的对象
 * param key: 待更新或者保存的键值对中的键
 * param value: 待更新或者保存的键值对中的值  
 * note 1: 若使用写直达法，write方法的缓存写入逻辑如下: 
 * 		   1. 若要写入的键在缓存中，则直接更新缓存中的对应值，若使用了写直达法，则接着调用noncache_write_callback写外置存储 
 *         2. 若要写入的键不在缓存中，则在缓存中创建一个新的键值对，必要时换出第一个进入缓存的键值对，若使用了写直达法，则接着调用noncache_write_callback写外置存储；若使用了写回法，则同步第一个进入缓存的键值对的最新值到外置存储中 
 */
template<typename T, typename R>
void FIFOCache<T, R>::write(const T &key,const R &value)
{
	// 1. 待查找的键值对在缓存中，则写入成功返回true 
	ListNode<T, R> *tar; 
	if(dic.get(key, tar))    // 查哈希表获取键值对所存储在的链表结点地址 
	{
		tar->value=value;    // 更新缓存中对应键值对中的值
		if(write_mode&&noncache_write_callback)   // 若使用了写直达法，则同步写入外置存储
		noncache_write_callback(key, value);
	}
	// 2. 待查找的键值对不在缓存中:
	else
	{ 
		// 2.1 判断是否需要换出第一个进入缓存的键值对
		if(size==max_size)        
		{
			ListNode<T, R> *temp=head;
			// 如果使用了写回法，换出缓存的键值对时，将最新的键值对写回外置存储
			if(!write_mode&&noncache_write_callback) 
			noncache_write_callback(temp->key, temp->value);
			// 从哈希表中移除换出对象的键-地址映射
			dic.remove(temp->key);   
			// 链表只有一个结点，则删除后尾结点置空 
			if(head==tail)       
			tail=nullptr;
			// 否则，直接删除链表头结点 
			head=head->next;
			--size;
			delete temp;
		}
		// 2.2 将新的键值对写入缓存，具体而言加入到链表末尾
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
		if(write_mode&&noncache_write_callback)   // 若使用了写直达法，则同步写入外置存储
		noncache_write_callback(key, value);
	} 
} 

/**
 * read: 从缓存中读取对象
 * param key: 待读取的键值对中的值 
 * param value: 输出参数，读取到的对应键的值
 * note 1: read方法的缓存读取逻辑如下:
 * 		   1. 若要读取的键在缓存中，则直接返回缓存中的对应值，读取成功返回true 
 *		   2. 若要读取的键不在缓存中: 
 * 		       (1). 若noncache_read_callback函数指针非空，则通过传入的回调函数指针调用noncache_read_callback方法获取键值对
 *       	        a. 若调用noncache_read_callback也没有查找到键的对应值，则读取失败，返回false
 *				    b. 若调用noncache_read_callback查找到了键的对应值，且缓存有空位，则直接写入缓存，读取成功返回true；若缓存无空位，则置换出去最先读入缓存的对象，然后写入缓存，读取成功返回true 
 *       	   (2). 若noncache_read_callback函数指针为空，则读取失败，返回false
 */
template<typename T, typename R>
bool FIFOCache<T, R>::read(const T &key, R &value)
{ 
	// 1. 待查找的键值对在缓存中，则读取成功返回true 
	ListNode<T, R> *tar; 
	if(dic.get(key, tar))    // 查哈希表获取键值对所存储在的链表结点地址 
	{
		value=tar->value;
		return true;
	}
	// 2. 待查找的键值对不在缓存中，且外置存储读取回调函数为空，则读取失败返回false 
	else if(!noncache_read_callback)
	return false;
	// 3. 待查找的键值对不在缓存中，且外置存储读取回调函数非空，从外置存储读取回调函数中读取值失败，则读取失败返回false 
	else if(!noncache_read_callback(key, value))
	return false;
	// 4. 待查找的键值对不在缓存中，且外置存储读取回调函数非空，从外置存储读取回调函数中读取值成功，则将读取的键值对加入缓存中，若缓存已经满，置换出最早的键值对 
	else
	{
		// 缓存已经满，换出缓存链表中的第一个元素，也即最早进入缓存的元素
		if(size==max_size)        
		{
			ListNode<T, R> *temp=head;
			// 如果使用了写回法，换出缓存的键值对时，将最新的键值对写回外置存储
			if(!write_mode&&noncache_write_callback) 
			noncache_write_callback(temp->key, temp->value);
			// 从哈希表中移除换出对象的键-地址映射
			dic.remove(temp->key);   
			// 链表只有一个结点，则删除后尾结点置空 
			if(head==tail)       
			tail=nullptr;
			// 否则，直接删除链表头结点 
			head=head->next;
			--size;
			delete temp;
		}
		// 插入新换入的结点
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
 * all: 遍历输出缓存中的键值对 
 * return: 键值对列表 
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
 * ~FIFOCache: 析构函数
 * note: 若使用写回法，最后释放时缓存中的键值对必须调用回递函数noncache_write_callback同步到外置存储中 
 */
template<typename T, typename R>
FIFOCache<T, R>::~FIFOCache()
{
	ListNode<T, R> *now=head;
	while(now)
	{
		ListNode<T, R> *temp=now;
		now=now->next;
		if(!write_mode)	  // 如果使用的是写回法，则析构函数中需要进行写回
		noncache_write_callback(temp->key, temp->value);
		delete temp;
	}
}

#endif


