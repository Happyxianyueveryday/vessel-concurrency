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
    LinkedBlockingQueue(const LinkedBlockingQueue<T> &other);                           // �������캯��
    LinkedBlockingQueue<T> &operator= (const LinkedBlockingQueue<T> &other) = delete;   // ������ֵ�����
    LinkedBlockingQueue(LinkedBlockingQueue<T> &&other);                                // �ƶ����캯�� 
    LinkedBlockingQueue<T> &operator= (LinkedBlockingQueue<T> &&other) = delete;        // �ƶ���ֵ����� 

    // 3. methods
    void push(T val) override;            // ������Ӳ���
    void pop(T &val) override;            // ���г��Ӳ���
    void frontval(T &val) override;       // ��ȡ����Ԫ��
    int length() override;                // ������г��ȣ��������е�Ԫ�ظ���
    bool empty() override;                // �ж϶����Ƿ�Ϊ��
	bool full() override;                 // �ж϶����Ƿ�Ϊ�� 

    // 4. destructor
    ~LinkedBlockingQueue();

	private:
    ListNode<T> *front;      // �������ж���Ԫ��ָ��
    ListNode<T> *end;        // �������ж�βԪ��ָ��
    int size;                // �������е�ǰԪ�ظ���
    int max_size;            // �������������Ԫ�ظ��� 
    
    std::recursive_mutex mut;          // ���ƻ���Ŀ���������ʹ�ÿ���������ԭ����Ҫ�ǿ����û����ܵݹ�����������е�ĳЩ���� 
	std::condition_variable_any push_var;  // ��������ӵĵȴ���������
	std::condition_variable_any pop_var;   // �����߳��ӵĵȴ��������� 
};

/**
 * LinkedBlockingQueue: �������й��캯��
 * param max_size: ָ�����������е����Ԫ������������ָ����Ϊ���޴� 
 */
template<typename T> 
LinkedBlockingQueue<T>::LinkedBlockingQueue(int _max_size)
{
    front=new ListNode<T>(0);   // ����ͷ���ƽ��
    end=front;
    size=0;
    max_size=_max_size;
}

/**
 * LinkedBlockingQueue: �������캯�� 
 */
template<typename T> 
LinkedBlockingQueue<T>::LinkedBlockingQueue(const LinkedBlockingQueue<T> &other)
{
	std::lock_guard<std::recursive_mutex> other_guard(other.mut);    // ���ÿ������캯��ʱ��ǰ������δ������ɣ����ֻ��Ҫ��ÿ���Դ����Ŀ����뻥����	
	
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
 * LinkedBlockingQueue: �ƶ����캯�� 
 */
template<typename T> 
LinkedBlockingQueue<T>::LinkedBlockingQueue(LinkedBlockingQueue<T> &&other)
{	
	std::lock_guard<std::recursive_mutex> other_guard(other.mut);
	
	front=other.front; 
	end=other.end; 
	size=other.size;
	max_size=other.max_size;
	
	other.front=new ListNode<T>(0);   // ����ͷ���ƽ��
    other.end=front;
    other.size=0;
}   

/**
 * push: ��������Ӳ���
 * param val: ��Ҫ��ӵ�ֵ 
 */
template<typename T> 
void LinkedBlockingQueue<T>::push(T val)
{
	std::unique_lock<std::recursive_mutex> push_lock(mut);
	push_var.wait(push_lock, [this]() {return !full();});    // ջ�����������Ȩ�ȴ���ֱ��ջδ��ʱ����ջ 
	
    ListNode<T> *newnode=new (std::nothrow)ListNode<T>(val);
    end->next=newnode;
    end=end->next;
    size+=1;
    
    pop_var.notify_one();       // ��ջ�ɹ���������ڵȴ��ĳ�ջ�̣߳��򼤻���Щ�߳� 
}

/** 
 * pop: ��������Ӳ���
 * param val: ʵ�ʳ��ӵĶ���Ԫ��ֵ 
 */
template<typename T> 
void LinkedBlockingQueue<T>::pop(T &val)
{
	std::unique_lock<std::recursive_mutex> pop_lock(mut);
	pop_var.wait(pop_lock, [this]() {return !empty();});   // ջ�ѿ��������Ȩ�ȴ���ֱ��ջδ��ʱ�ų�ջ����Ҫע���Ա����pop���ֵ����˻���ĳ�Ա����empty����������±���ʹ�ÿ�������������ͬһ���̶߳�μ�����ֱ�ӵ������� 
	
    ListNode<T> *temp=front->next;
    front->next=temp->next;
    val=temp->val;
    if(temp==end)   // ע�⣬ɾ�����һ��Ԫ��ʱendָ��ҲҪ�ֶ����� 
    end=front;
    delete temp;
    size-=1;
    
    push_var.notify_one();     // ��ջ�ɹ���������ڵȴ�����ջ�̣߳��򼤻���Щ�߳� 
}

/**
 * frontval: ������ȡ����Ԫ�ز���
 * param val: ʵ��ȡ�õĶ���Ԫ��ֵ 
 */
template<typename T> 
void LinkedBlockingQueue<T>::frontval(T &val)
{
	std::unique_lock<std::recursive_mutex> top_lock(mut);
	pop_var.wait(top_lock, [this]() {return !empty();});   // ��ջΪ���������Ȩ�ȴ���ֱ��ջδ��ʱ��ȡջ��Ԫ�� 
    val=front->next->val;
}

/** 
 * length: ���ص�ǰʱ�̶��еĳ��� 
 * return: ��ǰʱ�̶��еĳ��� 
 */
template<typename T> 
int LinkedBlockingQueue<T>::length() 
{
	std::lock_guard<std::recursive_mutex> length_guard(mut);
    return size;
}

/**
 * empty: �ж϶��е�ǰʱ���Ƿ�Ϊ�� 
 * return: ����ǰ����Ϊ���򷵻�true�����򷵻�false 
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
 * full: �жϵ�ǰ�����Ƿ�Ϊ�� 
 * return: ����ǰ����Ϊ���򷵻�true�����򷵻�false 
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
 * ~LinkedBlockingQueue: �������� 
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
