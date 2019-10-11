#ifndef CONCURRENT_LINKED_STACK_H                   
#define CONCURRENT_LINKED_STACK_H 
#include "ConcurrentStack.h"
#include "rwmutex.h"
#include "wrmutex.h"
#include "fair_rwmutex.h"
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
class ConcurrentLinkedStack: public ConcurrentStack<T>   
{
    public: 
    // 1. constructor
    ConcurrentLinkedStack(std::string mode);       // 构造函数

    // 2. copy/move controller
    ConcurrentLinkedStack(const ConcurrentLinkedStack<T> &other);                              // 拷贝构造函数
    ConcurrentLinkedStack<T> & operator= (const ConcurrentLinkedStack<T> &other) = delete;     // 禁用拷贝赋值运算符
    ConcurrentLinkedStack(ConcurrentLinkedStack<T> &&other);                                   // 移动构造函数 
    ConcurrentLinkedStack<T> & operator= (ConcurrentLinkedStack<T> &&other) = delete;          // 禁用移动赋值运算符 

    // 3. methods
    bool push(T val);    // 入栈操作
    bool pop(T &val);    // 出栈操作
    bool top(T &val);    // 获取栈顶元素操作
    int length();        // 获取栈的元素个数 

    // 4. destructor
    ~ConcurrentLinkedStack();        // 析构函数

	private: 
    // 5. domain
    ListNode<T> *pos;         // 栈顶元素结点指针
    int size;                 // 栈的元素个数
    std::string mode;      		  // 使用的读写锁模式 
    rwmutex rwmut;            // 读优先的读写锁 
    wrmutex wrmut;            // 写优先的读写锁 
    fair_rwmutex fair_rwmut;  // 读写公平的读写锁
	
	// 6. private methods
    void set_lock(std::string lockmode); 
};

template<typename T>
ConcurrentLinkedStack<T>::ConcurrentLinkedStack(std::string _mode)
{
    pos=nullptr;
    size=0;
    if(_mode=="read-first"||_mode=="write-first"||_mode=="fair")
    mode=_mode;
    else 
    mode="read-first";
}

template<typename T>
ConcurrentLinkedStack<T>::ConcurrentLinkedStack(const ConcurrentLinkedStack &other)
{
	other.set_lock("write_lock");
	
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
	
	other.set_lock("write_unlock"); 
}

template<typename T>
ConcurrentLinkedStack<T>::ConcurrentLinkedStack(ConcurrentLinkedStack &&other)
{
	other.set_lock("write_lock");
	
	pos=other.pos;
	size=other.size;
	
	other.pos=nullptr;
	other.size=0;
	
	other.set_lock("write_unlock");      
} 

template<typename T>
bool ConcurrentLinkedStack<T>::push(T val)
{
	set_lock("write_lock");   // push是写操作 
    ListNode<T> *newnode=new (std::nothrow)ListNode<T>(val);   // std::nothrow指定new运算符若内存申请失败不抛出异常，而是返回空指针 
    if(newnode)
    {
    	newnode->next=pos;
    	pos=newnode;
    	size+=1;
    	set_lock("write_unlock");
    	return true;
	}
	else
	{
		set_lock("write_unlock");
		return false;
	}
}

template<typename T>
bool ConcurrentLinkedStack<T>::pop(T &val)
{ 
	set_lock("write_lock");   // pop是写操作 
    if(!pos)           // 判断部分也必须处于互斥锁保护下，否则if语句和return语句之间可能有其他进程对栈进行了操作，导致获取的瞬间得到的结果就补准确 
    {
    	set_lock("write_unlock");
    	return false;
	}
    else
    {
        ListNode<T> *temp=pos;
        val=pos->val;
        pos=pos->next;
        size-=1;
        delete temp;
        set_lock("write_unlock");
        return true; 
    }
}

template<typename T>
bool ConcurrentLinkedStack<T>::top(T &val)
{
	set_lock("read_lock");    // top是读操作 
    if(!pos)
    {
    	set_lock("read_unlock");
    	return false;
	}
    else
    {
        val=pos->val;
        set_lock("read_unlock");
        return true;
    }
    
}

template<typename T>
int ConcurrentLinkedStack<T>::length()
{  
	set_lock("read_lock");    // length是读操作 
    int res=size;
    set_lock("read_unlock");
    return res;
}

template<typename T>
void ConcurrentLinkedStack<T>::set_lock(std::string lockmode)
{
	if(lockmode=="read_lock")
	{
		if(mode=="read-first")
		rwmut.read_lock();
		else if(mode=="write-first")
		wrmut.read_lock();
		else 
		fair_rwmut.read_lock();
	}
	else if(lockmode=="read_unlock")
	{
		if(mode=="read-first")
		rwmut.read_unlock();
		else if(mode=="write-first")
		wrmut.read_unlock();
		else 
		fair_rwmut.read_unlock();
	}
	else if(lockmode=="write_lock")
	{
		if(mode=="read-first")
		rwmut.write_lock();
		else if(mode=="write-first")
		wrmut.write_lock();
		else 
		fair_rwmut.write_lock();
	}
	else if(lockmode=="write_unlock")
	{
		if(mode=="read-first")
		rwmut.write_unlock();
		else if(mode=="write-first")
		wrmut.write_unlock();
		else 
		fair_rwmut.write_unlock();
	}
}

template<typename T>
ConcurrentLinkedStack<T>::~ConcurrentLinkedStack()
{
	set_lock("write_lock");    // 析构是写操作 
	ListNode<T> *now=pos;
	while(now)
	{
		ListNode<T> *temp=now;
		now=now->next;
		delete temp;
	}
	set_lock("write_unlock");
}

#endif
