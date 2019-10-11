#ifndef LINKED_BLOCKING_QUEUE_H    
#define LINKED_BLOCKING_QUEUE_H  
#include "BlockingQueue.h"  
#include <iostream>  
#include <string>
#include <condition_variable> 
#include <mutex>
#define INF INT_MAX 

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
class LinkedBlockingQueue: public BlockingQueue<T>
{
    public:   
    // 1. public constructor
    LinkedBlockingQueue<T>(int _max_size=INF);

    // 2. copy_controller
    LinkedBlockingQueue(const LinkedBlockingQueue<T> &other);                           // 拷贝构造函数
    LinkedBlockingQueue<T> &operator= (const LinkedBlockingQueue<T> &other) = delete;   // 拷贝赋值运算符
    LinkedBlockingQueue(LinkedBlockingQueue<T> &&other);                                // 移动构造函数 
    LinkedBlockingQueue<T> &operator= (LinkedBlockingQueue<T> &&other) = delete;        // 移动赋值运算符 

    // 3. methods
    void push(T val) override;            // 队列入队操作
    void pop(T &val) override;            // 队列出队操作
    void frontval(T &val) override;       // 获取队首元素
    int length() override;                // 计算队列长度，即队列中的元素个数
    bool empty() override;                // 判断队列是否为空
	bool full() override;                 // 判断队列是否为满 

    // 4. destructor
    ~LinkedBlockingQueue();

	private:
    ListNode<T> *front;      // 阻塞队列队首元素指针
    ListNode<T> *end;        // 阻塞队列队尾元素指针
    int size;                // 阻塞队列当前元素个数
    int max_size;            // 阻塞队列中最大元素个数 
    
    std::recursive_mutex mut;          // 控制互斥的可重入锁，使用可重入锁的原因主要是考虑用户可能递归调用阻塞队列的某些方法 
	std::condition_variable_any push_var;  // 生产者入队的等待条件变量
	std::condition_variable_any pop_var;   // 消费者出队的等待条件变量 
};

/**
 * LinkedBlockingQueue: 阻塞队列构造函数
 * param max_size: 指定阻塞队列中的最大元素数量，若不指定则为无限大 
 */
template<typename T> 
LinkedBlockingQueue<T>::LinkedBlockingQueue(int _max_size)
{
    front=new ListNode<T>(0);   // 链表头部哑结点
    end=front;
    size=0;
    max_size=_max_size;
}

/**
 * LinkedBlockingQueue: 拷贝构造函数 
 */
template<typename T> 
LinkedBlockingQueue<T>::LinkedBlockingQueue(const LinkedBlockingQueue<T> &other)
{
	std::lock_guard<std::recursive_mutex> other_guard(other.mut);    // 调用拷贝构造函数时当前对象尚未构造完成，因此只需要获得拷贝源对象的可重入互斥锁	
	
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
	max_size=other.max_size;
}

/**
 * LinkedBlockingQueue: 移动构造函数 
 */
template<typename T> 
LinkedBlockingQueue<T>::LinkedBlockingQueue(LinkedBlockingQueue<T> &&other)
{	
	std::lock_guard<std::recursive_mutex> other_guard(other.mut);
	
	front=other.front; 
	end=other.end; 
	size=other.size;
	max_size=other.max_size;
	
	other.front=new ListNode<T>(0);   // 链表头部哑结点
    other.end=front;
    other.size=0;
}   

/**
 * push: 阻塞版入队操作
 * param val: 需要入队的值 
 */
template<typename T> 
void LinkedBlockingQueue<T>::push(T val)
{
	std::unique_lock<std::recursive_mutex> push_lock(mut);
	push_var.wait(push_lock, [this]() {return !full();});    // 栈已满则进行让权等待，直到栈未满时再入栈 
	
    ListNode<T> *newnode=new (std::nothrow)ListNode<T>(val);
    end->next=newnode;
    end=end->next;
    size+=1;
    
    pop_var.notify_one();       // 入栈成功后如果存在等待的出栈线程，则激活这些线程 
}

/** 
 * pop: 阻塞版出队操作
 * param val: 实际出队的队首元素值 
 */
template<typename T> 
void LinkedBlockingQueue<T>::pop(T &val)
{
	std::unique_lock<std::recursive_mutex> pop_lock(mut);
	pop_var.wait(pop_lock, [this]() {return !empty();});   // 栈已空则进行让权等待，直到栈未空时才出栈，需要注意成员函数pop中又调用了互斥的成员函数empty，这种情况下必须使用可重入锁，否则同一个线程多次加锁会直接导致死锁 
	
    ListNode<T> *temp=front->next;
    front->next=temp->next;
    val=temp->val;
    if(temp==end)   // 注意，删除最后一个元素时end指针也要手动更新 
    end=front;
    delete temp;
    size-=1;
    
    push_var.notify_one();     // 出栈成功后如果存在等待的入栈线程，则激活这些线程 
}

/**
 * frontval: 阻塞版取队首元素操作
 * param val: 实际取得的队首元素值 
 */
template<typename T> 
void LinkedBlockingQueue<T>::frontval(T &val)
{
	std::unique_lock<std::recursive_mutex> top_lock(mut);
	pop_var.wait(top_lock, [this]() {return !empty();});   // 若栈为空则进行让权等待，直到栈未空时才取栈顶元素 
    val=front->next->val;
}

/** 
 * length: 返回当前时刻队列的长度 
 * return: 当前时刻队列的长度 
 */
template<typename T> 
int LinkedBlockingQueue<T>::length() 
{
	std::lock_guard<std::recursive_mutex> length_guard(mut);
    return size;
}

/**
 * empty: 判断队列当前时刻是否为空 
 * return: 若当前队列为空则返回true，否则返回false 
 */
template<typename T>
bool LinkedBlockingQueue<T>::empty()
{
	std::lock_guard<std::recursive_mutex> empty_guard(mut);
	if(!size)
	return true;
	else
	return false; 
}

/**
 * full: 判断当前队列是否为满 
 * return: 若当前队列为满则返回true，否则返回false 
 */
template<typename T>
bool LinkedBlockingQueue<T>::full()
{
	std::lock_guard<std::recursive_mutex> full_guard(mut);
	if(size==max_size)
	return true;
	else
	return false;
}

/**
 * ~LinkedBlockingQueue: 析构函数 
 */
template<typename T> 
LinkedBlockingQueue<T>::~LinkedBlockingQueue()
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
