#ifndef LRU_CACHE_H
#define LRU_CACHE_H 
#include "ZipHashMap.h"
#include <iostream>
#include <string>
#include <utility>

/**
 * ListNode: 最近最少使用缓存的双向链表结点
 * note: 每个存储结点保存一个键值对，最近最少使用缓存中使用的存储链表为双向链表结点 
 */
template<typename T, typename R>
struct ListNode
{
	ListNode(T _key=T(), R _value=R()):key(_key), value(_value), next(nullptr), prev(nullptr)
	{
	}
	T key;     // 结点存储的键 
	R value;   // 结点存储的值 
	ListNode<T, R> *next;   // 下一个结点的指针 
	ListNode<T, R> *prev;   // 上一个结点的指针 
};

/**
 * LRUCache: 最近最少使用缓存
 * note: 在缓存已经满时，最近最少使用缓存使用最近最少规则来换出最近最少使用的键值对，本缓存支持写直达法和写回法两种写策略 
 */
template<typename T, typename R>
class LRUCache
{
	public:
	// 0. sign
	const static bool WRITE_BACK=false;    // 写回法标志：设定缓存使用写回法 
	const static bool WRITE_THROUGH=true;  // 写直达法标志：设定缓存使用写直达法 
	
	// 1. constructor
	LRUCache(unsigned int _max_size, bool _write_mode=LRUCache::WRITE_BACK, bool (*_noncache_read_callback)(const T &key, R &value)=nullptr, void (*_noncache_write_callback)(const T &key, const R &value)=nullptr);    // 初始构造函数 
	
	// 2. copy/move controller
	LRUCache(const LRUCache<T, R> &other);               	   // 拷贝构造函数 
	LRUCache<T, R> &operator= (const LRUCache<T, R> &other);   // 拷贝赋值运算符 
	LRUCache(LRUCache<T, R> &&other); 				           // 移动构造函数
	LRUCache<T, R> &operator= (LRUCache<T, R> &&other);        // 移动赋值运算符 
	
	// 3. methods
	void write(const T &key,const R &value);    // 向缓存写入对象 
	bool read(const T &key, R &value);     		// 从缓存读取对象 
	std::vector<std::pair<T, R>> all();         // 遍历输出缓存中的对象
	
	// 4. destructor
	~LRUCache();    // 析构函数 
	
	private:
	// 5. domain
	unsigned int max_size;                // 缓存上限大小 
	ZipHashMap<T, ListNode<T, R>*> dic;   // 键映射到缓存对象结点的哈希表
	bool write_mode;                      // 缓存写入模式，WRITE_THROUGH为写直达法，WRITE_BACK为写回法，默认使用写回法 
	ListNode<T, R> *head;                 // 缓存对象链表首结点 
	ListNode<T, R> *tail;                 // 缓存对象链表尾结点
	unsigned int size;                    // 当前缓存对象数量 
	bool (*noncache_read_callback)(const T &key, R &value);        // 外置存储读取回调函数
	void (*noncache_write_callback)(const T &key, const R &value); // 外置存储写入回调函数  
	
	// 6. private methods
	void update(ListNode<T, R> *pos);     // 在访问结点后调整结点的访问记录 
};

/**
 * LRUCache: 初始构造函数 
 * param _max_size: LRU缓存的最大键值对数量 
 * param _write_mode: LRU缓存的写入模式，值为LRUCache::WRITE_BACK则使用写回法，值为LRUCache::WRITE_THROUGH则使用写直达法，默认使用写回法
 * param _noncache_read_callback: 非缓存读取回调函数指针，当在缓存中读取不到目标键值对时，调用该回调函数从外置存储中读取，默认值为空指针 
 * param _noncache_write_callback: 非缓存写入回调函数指针，当向缓存中写入目标键值对的新值时，根据选择的不同的写策略，在适当的时候将新值重新写入外置存储中，默认值为空指针 
 * note: 初始构造函数构造一个空的LRU缓存，该空缓存具有指定的大小以及写入模式 
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
 * LRUCache: 拷贝构造函数 
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
 * operator=: 拷贝赋值运算符 
 */
template<typename T, typename R>
LRUCache<T, R> &LRUCache<T, R>::operator= (const LRUCache<T, R> &other)
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
			now->next->prev=now;
			now=now->next;
			othnow=othnow->next;
		}
		tail=now;
	} 
	// 4. 返回当前对象的引用
	return (*this);
}

/**
 * LRUCache: 移动构造函数 
 */
template<typename T, typename R>
LRUCache<T, R>::LRUCache(LRUCache<T, R> &&other)
{
	// 1. 浅拷贝成员变量 
	max_size=other.max_size;
	dic=std::move(other.dic);
	write_mode=other.write_mode;
	head=other.head;
	tail=other.tail; 
	size=other.size;
	noncache_read_callback=other.noncache_read_callback;
	noncache_write_callback=other.noncache_write_callback; 
	
	// 2. 重置拷贝源成员变量 
	other.size=0;
	other.head=nullptr;
	other.tail=nullptr;
}

/** 
 * operator=: 移动赋值运算符 
 */
template<typename T, typename R>
LRUCache<T, R> &LRUCache<T, R>::operator= (LRUCache<T, R> &&other)
{
	// 1. 检查自我赋值情况 
	if(&other==this)
	return (*this);
	
	// 2. 释放当前对象的成员变量
	ListNode<T, R> *now=head;
	while(now)
	{
		ListNode<T, R> *temp=now;
		now=now->next;
		delete temp;
	} 
	
	// 3. 拷贝源对象的成员变量到当前对象中 
	max_size=other.max_size;
	dic=std::move(other.dic);
	write_mode=other.write_mode;
	head=other.head;
	tail=other.tail; 
	size=other.size;
	noncache_read_callback=other.noncache_read_callback;
	noncache_write_callback=other.noncache_write_callback; 
	
	// 4. 重置源对象的成员变量 
	other.size=0;
	other.head=nullptr;
	other.tail=nullptr;
	
	// 5. 返回当前对象的引用 
	return (*this);
}

/**
 * update: 更新结点键值对的访问历史记录 
 * param pos: 当前访问的结点
 * note: 本方法将访问过的结点调整到缓存链表结构的首结点 
 */
template<typename T, typename R>
void LRUCache<T, R>::update(ListNode<T, R> *pos)
{
	// 1. 若输入目标结点为空或者输入目标结点即为链表的首结点 
	if(!pos||!pos->prev)  
	return;
	// 2. 若输入目标结点非空且不为首结点，将该结点从原位置移动到链表首结点 
	else
	{
		ListNode<T, R> *prev=pos->prev, *next=pos->next;
		prev->next=next;
		if(next)     // next!=nullptr, 结点pos不是尾结点 
		next->prev=prev;
		else         // next==nullptr, 结点pos就是尾结点，需要更新尾结点tail的新值 
		tail=prev;
		
		pos->next=head;
		head->prev=pos;
		head=pos;
		return;
	}
}

/**
 * write: 向缓存写入对象 
 * param key: 需要向缓存写入的键
 * param value: 需要向缓存写入的值 
 */
template<typename T, typename R>
void LRUCache<T, R>::write(const T &key,const R &value)
{
	// 1. 从缓存字典中查找目标键值对的地址，若能够查找到对应的键值对，则更新键值对中的值，写入操作完成 
	ListNode<T, R> *tar=nullptr;
	if(dic.get(key, tar)&&!tar->value.equals(value))
	{
		tar->value=value;
		update(tar);           // 因为访问了结点tar，更新结点tar的访问历史记录 
		if(write_mode==LRUCache::WRITE_THROUGH)   // 若使用了写直达法，则需要将新的值写入外置存储 
		noncache_write_callback(key, value); 
	}
	// 2. 若不能查找到对应的键值对，则直接将新的输入键值对写入缓存 
	else
	{
		if(size==max_size)    // 若缓存空间已经满（最小的max_size为1），则在载入读取的键值对之前首先需要换出最近最少访问的键值对 
		{
			if(write_mode==LRUCache::WRITE_BACK)    // 若使用了写回法，将待换出缓存的键值对的值写回外置存储 
			noncache_write_callback(tail->key, tail->value);
			
			dic.remove(tail->key);          // 更新内部字典映射
			dic.insert(key, tail);   
			
			update(tail);     // 将最近最少使用的结点（尾结点）换到链表首结点位置 
			head->key=key;    // 通过修改该结点的键值对换出原先的最近最少使用的结点 
			head->value=value;
		}
		else    // 若缓存空间未满，则直接向缓存的链表中增加新的结点即可 
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
		if(write_mode==LRUCache::WRITE_THROUGH)   // 若使用了写直达法，将新写入的键值对的值写回外置存储 
		noncache_write_callback(key, value);
	}
}

/**
 * read: 从缓存读取对象
 * param key: 需要从缓存读取的键
 * param value: 输出向参数，从缓存读取到的键的对应值 
 */
template<typename T, typename R>
bool LRUCache<T, R>::read(const T &key, R &value)
{
	// 1. 从缓存字典中查找目标键值对的地址，若能够查找到对应的键值对，则直接返回对应的值，读取操作完成 
	ListNode<T, R> *tar=nullptr;
	if(dic.get(key, tar))
	{
		value=tar->value;
		update(tar);           // 因为访问了结点tar，更新结点tar的访问历史记录
		return true;
	}
	// 2. 若不能查找到对应的键值对，则需要从外置存储读取，若能够读取成功，则将外置存储中的对应键值对写入缓存中 
	else if(noncache_read_callback(key, value))
	{
		if(size==max_size)    // 若缓存空间已经满（最小的max_size为1），则在载入从外置存储读取的键值对之前首先需要换出最近最少访问的键值对 
		{
			if(write_mode==LRUCache::WRITE_BACK)    // 若使用了写回法，将待换出缓存的键值对的值写回外置存储 
			noncache_write_callback(tail->key, tail->value);
			
			dic.remove(tail->key);          // 更新内部字典映射
			dic.insert(key, tail);   
			
			update(tail);     // 将最近最少使用的结点（尾结点）换到链表首结点位置 
			head->key=key;    // 通过修改该结点的键值对换出原先的最近最少使用的结点 
			head->value=value;
		}
		else    // 若缓存空间未满，则直接向缓存的链表中增加新的结点即可 
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
	// 3. 从外置存储读取失败，则读取过程失败 
	else
	return false;
}

/**
 * all: 遍历缓存并输出
 * return: 缓存中存储的键值对列表 
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
 * ~LRUCache: 类析构函数 
 */
template<typename T, typename R>
LRUCache<T, R>::~LRUCache()
{
	ListNode<T, R> *now=head;
	while(now)
	{
		ListNode<T, R> *temp=now;
		
		if(write_mode==LRUCache::WRITE_BACK)    // 若使用了写回法，将待换出缓存的键值对的值写回外置存储 
		noncache_write_callback(now->key, now->value);
		
		now=now->next;
		delete temp;
	}
}

#endif
