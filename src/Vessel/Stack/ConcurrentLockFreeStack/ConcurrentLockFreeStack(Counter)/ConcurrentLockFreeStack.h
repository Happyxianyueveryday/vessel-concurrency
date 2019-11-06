#ifndef CONCURRENT_LOCK_FREE_STACK
#define CONCURRENT_LOCK_FREE_STACK
#include "AtomicStampPointer.h"
#include <thread>
#include <string>

template<typename T>
struct ListNode
{
	ListNode(T _val=T()):val(std::make_shared<T>(_val)), next(nullptr)
	{
	}
	std::shared_ptr<T> val;    // 结点的值，使用智能指针进行保存 
	ListNode<T> *next;    // 指向下一个结点的指针，无锁栈的CAS仅需要在头结点head处修改，因此无需使用原子类或者版本指针 
};

/**
 * ConcurrentLockFreeStack: 无锁栈（基于线程计数技术） 
 * note: 无锁栈相对于有锁栈，存在两个棘手问题需要解决，一是ABA问题，二是出栈对象的内存回收（多线程同时pop何时回收结点）的问题；第一个问题这里使用版本号指针来解决，第二个问题这里使用线程计数方式进行解决 
 */
template<typename T>
class ConcurrentLockFreeStack      
{ 
	public:
	// 1. constructor
	ConcurrentLockFreeStack();   
	
	// 2. copy/move controller
	ConcurrentLockFreeStack(const ConcurrentLockFreeStack<T> &other) = delete;      		 // 禁用拷贝构造函数 
	ConcurrentLockFreeStack &operator= (const ConcurrentLockFreeStack<T> &other) = delete;   // 禁用拷贝赋值运算符
	ConcurrentLockFreeStack(ConcurrentLockFreeStack<T> &other);                              // 禁用移动构造函数 
	ConcurrentLockFreeStack &operator= (ConcurrentLockFreeStack<T> &&other) = delete;   	 // 禁用移动赋值运算符  
	
	// 3. methods
	bool push(T val);            // 入栈 
	std::shared_ptr<T> pop();    // 出栈 
	void print();                // 测试用方法: 输出栈 
	
	// 4. destructor
	~ConcurrentLockFreeStack();
	
	private:
	// 5. domain 
	AtomicStampPointer<ListNode<T>> head;    // 栈顶结点指针 
	unsigned int pop_count;                  // 正在调用pop方法的线程的计数器 
	std::atomic<ListNode<T>*> bin_head;      // 待回收的结点链表 
	
	// 6. private function
	void gc(ListNode<T> *old_head);          // 结点垃圾回收器 
};

/**
 * ConcurrentLockFreeStack: 构造函数 
 */
template<typename T>
ConcurrentLockFreeStack<T>::ConcurrentLockFreeStack():head(nullptr), bin_head(nullptr), pop_count(0)
{
}

/**
 * push: 无锁栈入栈操作 
 * note: 无锁栈的入栈操作步骤如下：
 *		 1. 创建新的入栈结点
 *       2. 将新结点的next指针指向头结点
 *       3. 使用CAS检查现在的头结点是否和新结点的next指针指向的结点相同，若相同则设置头结点为新结点 
 */
template<typename T>
bool ConcurrentLockFreeStack<T>::push(T val)
{
	ListNode<T> *newnode=new ListNode<T>(val);
	newnode->next=head.get_ptr();
	while(!head.compare_exchange_weak(newnode->next, newnode, head.get_stamp(), head.get_stamp()+1));
}

/**
 * pop: 无锁栈出栈操作
 * note 1: 无锁栈的出栈操作要解决的核心问题是：可能存在多个pop方法并发执行，在何时可以释放要出栈的结点，是出栈操作需要处理的核心问题 
 * note 2: 这里使用的是线程计数方法解决上述问题，具体算法为:  
 *         1. 线程进入pop方法时，将pop线程计数器增加1 
 *         2. 使用CAS删除栈顶结点
 *         3. 保存删除的栈顶结点的值 
 *         4. 调用垃圾回收器，垃圾回收器根据线程计数器的值判断是否可以回收删除的栈顶结点，若仅有当前一个线程在使用pop，则可以进行垃圾回收；否则留待后续回收
 *         5. 线程准备离开pop方法，在垃圾回收器中将pop线程计数器减小1，并返回出栈的栈顶结点的值 
 */
template<typename T>
std::shared_ptr<T> ConcurrentLockFreeStack<T>::pop()
{
	// 1. 线程进入pop方法，将计数器增加1 
	++pop_count;       // 前置自增运算符是原子操作 
	// 2. 使用CAS方法删除栈顶结点
	ListNode<T> *old_head=head.get_ptr();
	while(old_head&&!head.compare_exchange_strong(old_head, old_head->next, head.get_stamp(), head.get_stamp()+1));
	// 3. 保存栈顶结点的值
	std::shared_ptr<T> res;
	if(old_head)
	res.swap(old_head->val);
	// 4. 线程调用垃圾回收器，尝试回收已删除的栈顶结点，在垃圾回收器中将pop线程计数器减少1 
	gc(old_head);
	// 5. 返回出栈的栈顶结点的值 
	return res;
}

/**
 * gc: 出栈结点的垃圾回收器
 * note: 垃圾回收器的算法如下:
 *       1. 若pop线程计数器的值为1，则只有一个线程在执行pop方法，这时刚出栈的结点即输入结点old_head必定可以安全释放，再取下待回收链表bin_head
 *       2. 再次判断pop线程计数器的值，若仅有一个线程在执行pop方法，则这时可以释放输入结点old_head和待回收链表bin_head；若有多个线程在执行pop方法，则这时仅能释放输入结点old_head，不能释放待回收链表而是将待回收链表bin_head放回 
 */
template<typename T>
void ConcurrentLockFreeStack<T>::gc(ListNode<T> *old_head)
{
	// 1. 仅有当前线程在pop方法内时，输入的出栈结点old_head一定可以安全地进行回收，而待回收结点链表要具体根据情况决定是否进行回收 
	if(pop_count==1)      
	{
		ListNode<T> *tar_head=bin_head.exchange(nullptr);   // 原子地取下之前的待回收结点链表的头结点，准备进行回收 
		// 1.1 仅有一个线程在pop方法内，这时就可以合法地释放待回收结点链表，因为链表是在计数器为1的时候取下来的，因此可以无锁直接释放
		if(--pop_count==0)  
		{
			while(tar_head)  
			{
				ListNode<T> *temp=tar_head;
				tar_head=tar_head->next;
				delete temp;
			}
		}
		// 1.2 若这时有多个线程在pop方法内，则这时待回收链表不能安全释放，若取下的待回收链表非空，则将待回收链表放回 
		else if(tar_head)  
		{
			// 将取下的待回收链表放回的算法是: 
			// (1) 首先查找到取下的待回收链表的尾结点 
			ListNode<T> *tail=tar_head;
			while(tail->next)
			{
				tail=tail->next;
			} 
			// (2) 将这时新的待回收链表连接在之前取下的待回收链表之后，这一个过程使用CAS完成 
			tail->next=bin_head.load();
			while(!bin_head.compare_exchange_weak(tail->next, tar_head));
		} 
		delete old_head;  // 进入时pop_count为1，因此刚出栈的结点已经可以释放 
	}
	// 2. 有两个或者多个线程在pop方法内时，释放出栈结点old_head或者待回收结点链表bin_head都是不安全的行为，这时需要将出栈结点old_head加入到待回收结点链表bin_head中 
	else
	{
		// 将当前输入的出栈结点加入待回收链表的算法是:
		// (1) 首先将待回收链表连接到当前出栈结点之后
		old_head->next=bin_head.load();
		// (2) 使用CAS重置待回收链表
		while(!bin_head.compare_exchange_weak(old_head->next, old_head));
		--pop_count;   // 减少线程计数器 
	}
}

/**
 * print: 仅供测试使用的方法，输出栈的元素，该方法不支持并发调用
 */
template<typename T>
void ConcurrentLockFreeStack<T>::print()
{
	ListNode<T> *now=head.get_ptr();
	while(now)
	{
		std::cout<<*(now->val)<<" ";
		now=now->next;
	}
	std::cout<<std::endl;
}

/**
 * ~ConcurrentLockFreeStack: 析构函数 
 * note: 在析构函数中释放栈中的所有结点以及待回收链表 
 */
template<typename T>
ConcurrentLockFreeStack<T>::~ConcurrentLockFreeStack()
{
	// 1. 释放栈中的所有结点
	ListNode<T> *now=head.get_ptr(); 
	while(now)
	{
		ListNode<T> *temp=now;
		now=now->next;
		delete temp;
	}
	// 2. 释放待回收链表中的所有结点
	now=bin_head.load();
	while(now)
	{
		ListNode<T> *temp=now;
		now=now->next;
		delete temp;
	}
}

#endif
