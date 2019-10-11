#ifndef CONCURRENTLINKEDSET_H              
#define CONCURRENTLINKEDSET_H              
#include "ConcurrentSet.h"
#include <iostream>
#include <string>
#include <mutex>

template<typename T>
class ListNode
{
	public:
    // 1. methods
    ListNode(T _val=T()):val(_val), next(nullptr)
    { 
    }
    
    inline void lock()
    {
    	mut.lock();
	}
    
    inline void unlock()
    {
    	mut.unlock();
	}
    
    // 2. domains
    T val;             // 结点存储的值 
    ListNode<T> *next; // 指向下一个表结点的指针 
    
    private:
    std::mutex mut;         // 结点的互斥量  
};

/**
 * ConcurrentLinkedSet: 基于链表和交替锁实现的线程安全的集合  
 * note: 使用交替锁实现线程安全时，需要十分小心各个方法中遍历访问链表结点的顺序，禁止反向加锁，即遍历时每个结点只能加锁一次解锁一次 
 */
template<typename T>
class ConcurrentLinkedSet: public ConcurrentSet<T>
{
    public:
    // 1. constructor
    ConcurrentLinkedSet();                             // 默认构造函数

    // 2. copy/move controller
    ConcurrentLinkedSet(const ConcurrentLinkedSet &other) = delete;                 		// 禁用拷贝构造函数
    ConcurrentLinkedSet &operator= (const ConcurrentLinkedSet &other) = delete;             // 禁用拷贝赋值运算符
    ConcurrentLinkedSet(ConcurrentLinkedSet &&other) = delete;                      		// 禁用移动构造函数
	ConcurrentLinkedSet &operator= (ConcurrentLinkedSet &&other) = delete;                  // 禁用移动赋值运算符 
    
    // 3. methods
    bool contain(const T &val) override;      // 判断给定元素是否在集合中 
    bool add(const T &val) override;          // 向集合中增加指定元素 
    bool remove(const T &val) override;       // 链表指定位置删除元素
    int length() override;                    // 获取集合的大小 
    std::string toString() override;               // 输出当前集合

    // 4. destructor
    ~ConcurrentLinkedSet();

    private:
    ListNode<T> *head;         // 用于保存元素的链表，按照hashCode从小到大排列 
    
    // 5. private methods
    void safe_lock(ListNode<T> *node);
    void safe_unlock(ListNode<T> *node);
};

/**
 * ConcurrentLinkedSet: 默认构造函数
 */
template<typename T>
ConcurrentLinkedSet<T>::ConcurrentLinkedSet() 
{
    head=new ListNode<T>();    // 创建链表头部哑结点
}

/**
 * contain: 判断集合中是否存在给定元素 
 * param val: 给定需要判断的元素 
 * return: 若集合中存在给定元素，则返回true；否则返回false
 * note: 获取指定位置的链表元素时，在遍历时始终只需要锁定一个结点，获取到下一个结点互斥锁，移动指针到下一个结点，释放当前结点的互斥锁即可  
 */      
template<typename T>
bool ConcurrentLinkedSet<T>::contain(const T &val)
{	
	safe_lock(head);                // 头部哑结点上锁 
	ListNode<T> *now=head;          // 链表游标指针 
	
	while(now)
	{
		if(now->val.hashCode()==val.hashCode()&&now->val.equals(val))    // 先比较hashCode的值，若hashCode相同则继续比较equals，两者比较结果均相等，则当前结点就是要查找的值，返回true 
		{
			safe_unlock(now);  // 返回之前不要忘记解锁当前结点 
			return true;
		}
		ListNode<T> *temp=now;    
		safe_lock(now->next);  // 尝试获取下一个结点的互斥锁 
		now=now->next;         // 获取后继结点的互斥锁成功后，移动指针到下一个结点  // debug note: 禁止先解锁当前结点后再移动，因为一旦解锁后，now->next就可能被修改，这时now=now->next就不再准确了 
		safe_unlock(temp);     // 解锁之前指向的结点 
	}
	
	safe_unlock(now);  // 查找到链表结束均未查找到目标值，则返回false 
	return false;
}

/**
 * add: 向集合中添加元素
 * param val: 要插入的元素值
 * return: 若插入成功，则返回true；若集合中已经存在该元素导致添加失败，则返回false
 * note: 插入操作首先按照查找元素相同的方式，交替锁定一个结点查找到待插入位置的前驱结点now；然后执行插入过程，进行插入之前首先必须锁定结点now和now->next，防止插入过程中这两个结点被修改，然后进行插入过程 
 */
template<typename T>
bool ConcurrentLinkedSet<T>::add(const T &val)
{
	safe_lock(head);   // 头部哑结点上锁
	ListNode<T> *now=head;  // 链表游标指针 
	
	while(now)   // 依次遍历查找第一个哈希值大于输入结点的结点 
	{      
		ListNode<T> *temp=now;
		safe_lock(now->next); // 尝试获取下一个结点的锁 
		// 若当前结点哈希值和输入元素哈希值相等，且实际上也相等，则集合中已经存在该元素，插入失败 
		if(now!=head&&now->val.hashCode()==val.hashCode()&&now->val.equals(val))
		{
			// 此时锁定了now和now->next 
			safe_unlock(now);        // 注意各种情况下需要各自解锁哪些锁定的结点 
			safe_unlock(now->next);
			return false;
		}
		// 判断now->next是否为待插入位置的后继结点，由于已经锁定结点now->next，是则进行插入过程（这时避免释放了结点now->next的前驱结点now，从而避免了解锁后再加锁） 
		if(!now->next||now->next->val.hashCode()>val.hashCode())  // 若now->next结点为空，或者now->next的结点哈希值大于输入结点，则now就是插入位置的前驱结点，开始执行插入过程 
		{
			// 此时已经锁定了now和now->next，可以执行插入
			ListNode<T> *newnode=new ListNode<T>(val);
			newnode->next=now->next;
			now->next=newnode;
			safe_unlock(now);
			safe_unlock(newnode->next);    // warning: 不要写成safe_unlock(now->next)，now->next在插入过程中已经被修改过了
			return true; 
		}
		// 不是则继续移动游标指针到下一个结点，然后释放前一个结点的锁
		now=now->next;
		safe_unlock(temp);
	}
	// 此时只有now被锁定
	safe_unlock(now);
	return false;
}

/**
 * remove: 链表指定位置删除元素
 * param index: 待删除的位置下标
 * param val: 所删除的值
 * note: 删除操作首先按照查找元素相同的方式，查找到待删除结点的前驱结点now；然后执行删除过程，删除过程之前首先需要锁定结点now，now->next，now->next->next三个结点，接着才能执行删除结点操作，删除结束后，解锁剩下的两个结点 
 */ 
template<typename T>
bool ConcurrentLinkedSet<T>::remove(const T &val)
{
	safe_lock(head);        // 锁定头部结点
	ListNode<T> *now=head;  // 链表游标指针 
	
	while(now)    // 遍历链表查找待删除结点的前驱结点 
	{
		ListNode<T> *temp=now;
		safe_lock(now->next);
		// 查找到待删除结点，执行删除操作，最后返回true 
		if(now->next&&now->next->val.hashCode()==val.hashCode()&&now->next->val.equals(val))   
		{
			// 此时now, now->next已经锁定，还需要锁定now->next->next
			safe_lock(now->next->next);
			ListNode<T> *delnode=now->next;
			now->next=now->next->next;
			delete delnode;
			safe_unlock(now);
			safe_unlock(now->next);   // 注意不要写成safe_unlock(now->next->next)，now->next->next已经被改变 
			return true;
		}
		// 否则继续进行查找 
		now=now->next;
		safe_unlock(temp);
	}
	// 查找结束，未查找到待删除结点，返回false 
	safe_unlock(now);   // 不要忘记释放当前结点的互斥锁 
	return false;
}

/**
 * size: 获取链表的长度/元素个数
 */
template<typename T>
int ConcurrentLinkedSet<T>::length()
{
	int count=0;
    safe_lock(head->next);                 // 获取首结点的互斥锁 
	ListNode<T> *now=head->next;           // 用于遍历的游标指针 
	
	while(now)
	{
		count+=1;	
		ListNode<T> *temp=now;
		safe_lock(now->next);              // 获取下一个结点的互斥锁 
		now=now->next;
		safe_unlock(temp);                 // 释放上一个结点的互斥锁 
	}
	safe_unlock(now);
	return count;
}

/**
 * ~toString: 输出当前链表 
 */
template<typename T>
std::string ConcurrentLinkedSet<T>::toString()
{
	std::string res; 
	safe_lock(head->next);                 // 获取首结点的互斥锁 
	ListNode<T> *now=head->next;           // 用于遍历的游标指针 
	
	while(now)
	{
		res=res+((now==head->next)?"":"->")+now->val.toString();	
		ListNode<T> *temp=now;
		safe_lock(now->next);              // 获取下一个结点的互斥锁 
		now=now->next;
		safe_unlock(temp);                 // 释放上一个结点的互斥锁 
	}
	safe_unlock(now);
	return res;
}

/**
 * safe_lock: 带空指针安全检查的加锁方法  
 */
template<typename T>
void ConcurrentLinkedSet<T>::safe_lock(ListNode<T> *node)
{
	if(node)
	node->lock();
}

/**
 * safe_unlock: 带空指针安全检查的解锁方法 
 */
template<typename T>
void ConcurrentLinkedSet<T>::safe_unlock(ListNode<T> *node)
{
	if(node)
	node->unlock();  
}

/**
 * ~ConcurrentLinkedSet: 析构函数
 * note: 析构函数同样可以加锁，但是实际上没有必要，析构时已经没有线程还在调用线程安全链表的方法了 
 */
template<typename T>
ConcurrentLinkedSet<T>::~ConcurrentLinkedSet() 
{
    ListNode<T> *now=head;

    while(now)
    {
        ListNode<T> *temp=now;
        now=now->next;
        delete temp;
    }
}

#endif
