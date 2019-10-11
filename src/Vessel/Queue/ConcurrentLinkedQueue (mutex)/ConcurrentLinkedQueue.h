#ifndef CONCURRENT_LINKED_QUEUE_H 
#define CONCURRENT_LINKED_QUEUE_H
#include "ConcurrentQueue.h" 
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
    ConcurrentLinkedQueue<T>();

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
    ListNode<T> *front;      // 队首元素指针
    ListNode<T> *end;        // 队尾元素指针
    int size;                // 队列元素个数 
    std::mutex mut;               // 链表队列互斥量     
};

template<typename T> 
ConcurrentLinkedQueue<T>::ConcurrentLinkedQueue()
{
    front=new ListNode<T>(0);   // 链表头部哑结点
    end=front;
    size=0;
}

template<typename T> 
ConcurrentLinkedQueue<T>::ConcurrentLinkedQueue(const ConcurrentLinkedQueue<T> &other)
{
	std::lock_guard<std::mutex> other_guard(other.mut);
	
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
}

template<typename T> 
ConcurrentLinkedQueue<T>::ConcurrentLinkedQueue(ConcurrentLinkedQueue<T> &&other)
{
	std::lock_guard<std::mutex> other_guard(other.mut);
	
	front=other.front; 
	end=other.end; 
	size=other.size;
	
	other.front=new ListNode<T>(0);   // 链表头部哑结点
    other.end=front;
    other.size=0;
}

template<typename T> 
bool ConcurrentLinkedQueue<T>::push(T val)
{
	std::lock_guard<std::mutex> guard(mut);
    ListNode<T> *newnode=new (std::nothrow)ListNode<T>(val);
    if(newnode)
    {
        end->next=newnode;
        end=end->next;
        size+=1;
        return true;
    }
    else
    return false;
}

template<typename T> 
bool ConcurrentLinkedQueue<T>::pop(T &val)
{
	std::lock_guard<std::mutex> guard(mut);
    if(!size)
    return false;
    else
    {
        ListNode<T> *temp=front->next;
        front->next=temp->next;
        val=temp->val;
        
        if(temp==end)   // 注意，删除最后一个元素时end指针也要手动更新 
        end=front;
        
        delete temp;
        size-=1;
        return true;
    }
}

template<typename T> 
bool ConcurrentLinkedQueue<T>::frontval(T &val)
{
	std::lock_guard<std::mutex> guard(mut);
    if(!size)
    return false;
    else
    {
        val=front->next->val;
        return true;
    }
}

template<typename T> 
int ConcurrentLinkedQueue<T>::length() 
{
	std::lock_guard<std::mutex> guard(mut);
    return size;
}

template<typename T> 
ConcurrentLinkedQueue<T>::~ConcurrentLinkedQueue()
{
    ListNode<T> *now=front;
    while(now)
    {
        ListNode<T> *temp=now;
        now=now->next;
        delete temp;
    }
}

#endif
