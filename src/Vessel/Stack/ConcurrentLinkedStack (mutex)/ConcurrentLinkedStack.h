#ifndef CONCURRENT_LINKED_STACK_H      
#define CONCURRENT_LINKED_STACK_H 
#include "ConcurrentStack.h"
#include <iostream>     
#include <mutex>

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
class ConcurrentArrayStack: public ConcurrentStack<T>   
{
    public: 
    // 1. constructor
    ConcurrentArrayStack();       // 默认构造函数

    // 2. copy/move controller
    ConcurrentArrayStack(const ConcurrentArrayStack<T> &other);                              // 拷贝构造函数
    ConcurrentArrayStack<T> & operator= (const ConcurrentArrayStack<T> &other) = delete;     // 禁用拷贝赋值运算符
    ConcurrentArrayStack(ConcurrentArrayStack<T> &&other);                                   // 移动构造函数 
    ConcurrentArrayStack<T> & operator= (ConcurrentArrayStack<T> &&other) = delete;          // 禁用移动赋值运算符 

    // 3. methods 
    bool push(T val);    // 入栈操作
    bool pop(T &val);    // 出栈操作
    bool top(T &val);    // 获取栈顶元素操作
    int length();        // 获取栈的元素个数 

    // 4. destructor
    ~ConcurrentArrayStack();        // 析构函数

    // 5. domain
    ListNode<T> *pos;         // 栈顶元素结点指针
    int size;                 // 栈的元素个数
    std::mutex mut;                // 对象写入互斥锁 
}; 

template<typename T>
ConcurrentArrayStack<T>::ConcurrentArrayStack()
{ 
    pos=nullptr;
    size=0;
}

template<typename T>
ConcurrentArrayStack<T>::ConcurrentArrayStack(const ConcurrentArrayStack &other)
{ 
	std::lock_guard<std::mutex> other_guard(other.mut);
	
	size=other.size;
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
ConcurrentArrayStack<T>::ConcurrentArrayStack(ConcurrentArrayStack &&other)
{ 
	std::lock_guard<std::mutex> other_guard(other.mut);
	
	pos=other.pos;
	size=other.size;
	
	other.pos=nullptr;
	other.size=0;
} 

template<typename T>
bool ConcurrentArrayStack<T>::push(T val)
{
    std::lock_guard<std::mutex> guard(mut);
    ListNode<T> *newnode=new (std::nothrow)ListNode<T>(val);   // std::nothrow指定new运算符若内存申请失败不抛出异常，而是返回空指针 
    if(newnode)
    {
    	newnode->next=pos;
    	pos=newnode;
    	size+=1;
    	return true;
	}
	else
	return false;
}

template<typename T>
bool ConcurrentArrayStack<T>::pop(T &val)
{
	std::lock_guard<std::mutex> guard(mut);   
    if(!pos)           // 判断部分也必须处于互斥锁保护下，否则if语句和return语句之间可能有其他进程对栈进行了操作，导致获取的瞬间得到的结果就补准确 
    return false; 
    else
    {
        ListNode<T> *temp=pos;
        val=pos->val;
        pos=pos->next;
        size-=1;
        delete temp;
        return true; 
    }
}

template<typename T>
bool ConcurrentArrayStack<T>::top(T &val)
{
	std::lock_guard<std::mutex> guard(mut);   
    if(!pos)
    return false;
    else
    {
        val=pos->val;
        return true;
    }
    
}

template<typename T>
int ConcurrentArrayStack<T>::length()
{
	std::lock_guard<std::mutex> guard(mut);   
    return size;
}

template<typename T>
ConcurrentArrayStack<T>::~ConcurrentArrayStack()
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
