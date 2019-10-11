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
    ConcurrentBlockingStack(int _max_size=INF);       // Ĭ�Ϲ��캯��

    // 2. copy/move controller
    ConcurrentBlockingStack(const ConcurrentBlockingStack<T> &other);                              // �������캯��
    ConcurrentBlockingStack<T> & operator= (const ConcurrentBlockingStack<T> &other) = delete;     // ���ÿ�����ֵ�����
    ConcurrentBlockingStack(ConcurrentBlockingStack<T> &&other);                                   // �ƶ����캯�� 
    ConcurrentBlockingStack<T> & operator= (ConcurrentBlockingStack<T> &&other) = delete;          // �����ƶ���ֵ����� 

    // 3. methods 
    void push(T val) override;    // ��ջ����
    void pop(T &val) override;    // ��ջ����
    void top(T &val) override;    // ��ȡջ��Ԫ�ز���
    int length() override;        // ��ȡջ��Ԫ�ظ���  
    bool empty() override;        // �ж�ջ�Ƿ�Ϊ�� 
    bool full() override;         // �ж�ջ�Ƿ�Ϊ�� 

    // 4. destructor
    ~ConcurrentBlockingStack();        // ��������

    // 5. domain
    ListNode<T> *pos;         // ջ��Ԫ�ؽ��ָ��
    int size;                 // ջ��Ԫ�ظ���
    int max_size;             // ջ�����Ԫ�ظ��� 
	
	std::condition_variable_any push_var;    // ��ջ�ź��� 
	std::condition_variable_any pop_var;     // ��ջ�ź��� 
	std::recursive_mutex mut;                // �����뻥����  
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
	pos=nullptr;     // �������캯���г�Ա������δ��ʼ�� 
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
	push_var.wait(push_lock, [this]() {return !full();});       // ��ջ�Ѿ��������ͷŻ�����������Ȩ�ȴ���ֱ��ջδ���ٽ�����ջ���� 
	
    ListNode<T> *newnode=new (std::nothrow)ListNode<T>(val);   // std::nothrowָ��new��������ڴ�����ʧ�ܲ��׳��쳣�����Ƿ��ؿ�ָ�� 
    newnode->next=pos;
    pos=newnode;
    size+=1;
    
    pop_var.notify_one();     // ��ջ�ɹ���ջ��������һ��Ԫ�أ���˿��Ի����������ڵȴ��ĳ�ջ�߳� 
}

template<typename T>
void ConcurrentBlockingStack<T>::pop(T &val)
{
	std::unique_lock<std::recursive_mutex> pop_lock(mut);
	pop_var.wait(pop_lock, [this]() {return !empty();});   // ��ջ�Ѿ��գ����ͷŻ�����������Ȩ�ȴ���ֱ��ջ��Ϊ���ٽ��г�ջ���� 
	
    ListNode<T> *temp=pos;
    val=pos->val;
    pos=pos->next;
    size-=1;
    delete temp;
    
    push_var.notify_one();      // ��ջ�ɹ���ջһ��δ������˿��Ի����������ڵȴ�����ջ�߳� 
}

template<typename T>
void ConcurrentBlockingStack<T>::top(T &val)
{
    std::unique_lock<std::recursive_mutex> pop_lock(mut);
	pop_var.wait(pop_lock, [this]() {return !empty();});   // ��ջ�Ѿ��գ����ͷŻ�����������Ȩ�ȴ���ֱ��ջ��Ϊ����ȡջ��Ԫ�� 
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
