#ifndef CONCURRENT_BLOCKING_STACK_H      
#define CONCURRENT_BLOCKING_STACK_H
#include "BlockingStack.h"
#include <iostream>     
#include <mutex>
#include <condition_variable>
#define INF INT_MAX       

template<typename T>
struct ListNode
{
    // 1. methods
    ListNode(T _val):val(_val), next(nullptr)
    {
    }
    
    // 2. domains
    T val;
    ListNode *next;
};

template<typename T>
class ConcurrentBlockingStack: public BlockingStack<T>   
{
    public: 
    // 1. constructor
    ConcurrentBlockingStack(int _max_size=INF);       // 默认构造函数

    // 2. copy/move controller
    ConcurrentBlockingStack(const ConcurrentBlockingStack<T> &other);                              // 拷贝构造函数
    ConcurrentBlockingStack<T> & operator= (const ConcurrentBlockingStack<T> &other) = delete;     // 禁用拷贝赋值运算符
    ConcurrentBlockingStack(ConcurrentBlockingStack<T> &&other);                                   // 移动构造函数 
    ConcurrentBlockingStack<T> & operator= (ConcurrentBlockingStack<T> &&other) = delete;          // 禁用移动赋值运算符 

    // 3. methods 
    void push(T val) override;    // 入栈操作
    void pop(T &val) override;    // 出栈操作
    void top(T &val) override;    // 获取栈顶元素操作
    int length() override;        // 获取栈的元素个数  
    bool empty() override;        // 判断栈是否为空 
    bool full() override;         // 判断栈是否为满 

    // 4. destructor
    ~ConcurrentBlockingStack();        // 析构函数

    // 5. domain
    ListNode<T> *pos;         // 栈顶元素结点指针
    int size;                 // 栈的元素个数
    int max_size;             // 栈的最大元素个数 
	
	std::condition_variable_any push_var;    // 入栈信号量 
	std::condition_variable_any pop_var;     // 出栈信号量 
	std::recursive_mutex mut;                // 可重入互斥锁  
};  

template<typename T>
ConcurrentBlockingStack<T>::ConcurrentBlockingStack(int _max_size)
{ 
    pos=nullptr;
    max_size=_max_size;
    size=0;
}

template<typename T>
ConcurrentBlockingStack<T>::ConcurrentBlockingStack(const ConcurrentBlockingStack &other)
{ 
	std::lock_guard<std::recursive_mutex> other_guard(other.mut);
	size=other.size;
	max_size=other.max_size;
	pos=nullptr;     // 拷贝构造函数中成员变量并未初始化 
	if(other.pos) 
	{
		pos=new ListNode<T>(other.pos->val);
		ListNode<T> *now=pos, *othnow=other.pos->next;
		while(othnow)
		{
			now->next=new ListNode<T>(othnow->val);
			now=now->next;
			othnow=othnow->next;
		}
	}
}

template<typename T>
ConcurrentBlockingStack<T>::ConcurrentBlockingStack(ConcurrentBlockingStack &&other)
{ 
	std::lock_guard<std::recursive_mutex> other_guard(other.mut);
	pos=other.pos;
	size=other.size;
	max_size=other.max_size;
	
	other.pos=nullptr;
	other.size=0;      
} 

template<typename T>
void ConcurrentBlockingStack<T>::push(T val)
{
	std::unique_lock<std::recursive_mutex> push_lock(mut);
	push_var.wait(push_lock, [this]() {return !full();});       // 若栈已经满，则释放互斥锁进行让权等待，直到栈未满再进行入栈操作 
	
    ListNode<T> *newnode=new (std::nothrow)ListNode<T>(val);   // std::nothrow指定new运算符若内存申请失败不抛出异常，而是返回空指针 
    newnode->next=pos;
    pos=newnode;
    size+=1;
    
    pop_var.notify_one();     // 入栈成功后，栈中至少有一个元素，因此可以唤醒所有正在等待的出栈线程 
}

template<typename T>
void ConcurrentBlockingStack<T>::pop(T &val)
{
	std::unique_lock<std::recursive_mutex> pop_lock(mut);
	pop_var.wait(pop_lock, [this]() {return !empty();});   // 若栈已经空，则释放互斥锁进行让权等待，直到栈不为空再进行出栈操作 
	
    ListNode<T> *temp=pos;
    val=pos->val;
    pos=pos->next;
    size-=1;
    delete temp;
    
    push_var.notify_one();      // 出栈成功后，栈一定未满，因此可以唤醒所有正在等待的入栈线程 
}

template<typename T>
void ConcurrentBlockingStack<T>::top(T &val)
{
    std::unique_lock<std::recursive_mutex> pop_lock(mut);
	pop_var.wait(pop_lock, [this]() {return !empty();});   // 若栈已经空，则释放互斥锁进行让权等待，直到栈不为空再取栈顶元素 
    val=pos->val;
}

template<typename T>
int ConcurrentBlockingStack<T>::length()
{
	std::lock_guard<std::recursive_mutex> length_lock(mut);
    return size;
}

template<typename T>
bool ConcurrentBlockingStack<T>::empty()
{
	std::lock_guard<std::recursive_mutex> empty_lock(mut);
	if(!size)
	return true;
	else
	return false;
}

template<typename T>
bool ConcurrentBlockingStack<T>::full()
{
	std::lock_guard<std::recursive_mutex> full_lock(mut);
	if(size==max_size)
	return true;
	else
	return false;
}

template<typename T>
ConcurrentBlockingStack<T>::~ConcurrentBlockingStack()
{ 
	ListNode<T> *now=pos;
	while(now)
	{
		ListNode<T> *temp=now;
		now=now->next;
		delete temp;
	}
}

#endif 
