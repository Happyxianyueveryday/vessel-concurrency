#ifndef CONCURRENT_LINKED_STACK_H  
#define CONCURRENT_LINKED_STACK_H
#include "ConcurrentQueue.h" 
#include "rwmutex.h"
#include "wrmutex.h"
#include "fair_rwmutex.h"
#include <iostream> 
#include <string>
#include <mutex>

template<typename T> 
struct ListNode
{
    // 1. methods
    ListNode(T _val):val(_val), next(NULL)
    {
    }
    
    // 2. domains
    T val; 
    ListNode<T> *next;  
};

template<typename T> 
class ConcurrentLinkedQueue: public Queue<T>
{
    public:   
    // 1. public constructor
    ConcurrentLinkedQueue<T>(std::string mode);

    // 2. copy_controller
    ConcurrentLinkedQueue(const ConcurrentLinkedQueue<T> &other);                           // 拷贝构造函数
    ConcurrentLinkedQueue<T> &operator= (const ConcurrentLinkedQueue<T> &other) = delete;   // 拷贝赋值运算符
    ConcurrentLinkedQueue(ConcurrentLinkedQueue<T> &&other);                                // 移动构造函数 
    ConcurrentLinkedQueue<T> &operator= (ConcurrentLinkedQueue<T> &&other) = delete;        // 移动赋值运算符 

    // 3. methods
    bool push(T val) override;            // 队列入队操作
    bool pop(T &val) override;            // 队列出队操作
    bool frontval(T &val) override;       // 获取队首元素
    int length() override;                // 计算队列长度，即队列中的元素个数

    // 4. destructor
    ~ConcurrentLinkedQueue();

	private:
    ListNode<T> *front;       // 队首元素指针
    ListNode<T> *end;         // 队尾元素指针
    int size;                 // 队列元素个数 
    std::string mode;              // 读写锁的优先级模式 
	rwmutex rwmut;            // 读优先的读写锁 
    wrmutex wrmut;            // 写优先的读写锁 
    fair_rwmutex fair_rwmut;  // 读写公平的读写锁 
    
    // 5. private methods
    void set_lock(std::string lockmode);                     
};

template<typename T> 
ConcurrentLinkedQueue<T>::ConcurrentLinkedQueue(std::string _mode)
{
    front=new ListNode<T>(0);   // 链表头部哑结点
    end=front;
    size=0;
    if(_mode=="read-first"||_mode=="write-first"||_mode=="fair")
    mode=_mode;
    else
    mode="read-first";
}

template<typename T> 
ConcurrentLinkedQueue<T>::ConcurrentLinkedQueue(const ConcurrentLinkedQueue<T> &other)
{
	other.set_lock("write_lock"); 
		
    front=new ListNode<T>(0);
    
    ListNode<T> *now=front;
    ListNode<T> *othnow=other.front->next;
    while(othnow)
    {
    	now->next=new ListNode<T>(othnow->val);
    	now=now->next;
    	othnow=othnow->next;
	}
	
	end=now;
	size=other.size;
	mode=other.mode;
	
	other.set_lock("write_unlock");
}

template<typename T> 
ConcurrentLinkedQueue<T>::ConcurrentLinkedQueue(ConcurrentLinkedQueue<T> &&other)
{
	other.set_lock("write_lock");
	
	front=other.front; 
	end=other.end; 
	size=other.size;
	mode=other.mode;
	
	other.front=new ListNode<T>(0);   // 链表头部哑结点
    other.end=front;
    other.size=0;
    
	other.set_lock("write_unlock");
}

template<typename T> 
bool ConcurrentLinkedQueue<T>::push(T val)
{
	set_lock("write_lock");
    ListNode<T> *newnode=new (std::nothrow)ListNode<T>(val);
    if(newnode)
    {
        end->next=newnode;
        end=end->next;
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
bool ConcurrentLinkedQueue<T>::pop(T &val)
{
	set_lock("write_lock");
    if(!size)
    {
    	set_lock("write_unlock");
    	return false;
    }
    else
    {
        ListNode<T> *temp=front->next;
        front->next=temp->next;
        val=temp->val;
        
        if(temp==end)   // 注意，删除最后一个元素时end指针也要手动更新 
        end=front;
        
        delete temp;
        size-=1;
        set_lock("write_unlock");
        return true;
    }
}

template<typename T> 
bool ConcurrentLinkedQueue<T>::frontval(T &val)
{
	set_lock("read_lock");
    if(!size)
    {
    	set_lock("read_unlock");
    	return false;
    }
    else
    {
        val=front->next->val;
        set_lock("read_unlock");
        return true;
    }
}

template<typename T> 
int ConcurrentLinkedQueue<T>::length() 
{
	set_lock("read_lock");
	int res=size;
	set_lock("read_unlock");
    return size;
}

template<typename T>
void ConcurrentLinkedQueue<T>::set_lock(std::string lockmode)
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
ConcurrentLinkedQueue<T>::~ConcurrentLinkedQueue()
{
	set_lock("write_lock");
    ListNode<T> *now=front;
    while(now)
    {
        ListNode<T> *temp=now;
        now=now->next;
        delete temp;
    }
    set_lock("write_unlock");
}

#endif
