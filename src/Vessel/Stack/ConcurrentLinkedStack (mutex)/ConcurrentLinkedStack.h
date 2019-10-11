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
    ConcurrentArrayStack();       // Ĭ�Ϲ��캯��

    // 2. copy/move controller
    ConcurrentArrayStack(const ConcurrentArrayStack<T> &other);                              // �������캯��
    ConcurrentArrayStack<T> & operator= (const ConcurrentArrayStack<T> &other) = delete;     // ���ÿ�����ֵ�����
    ConcurrentArrayStack(ConcurrentArrayStack<T> &&other);                                   // �ƶ����캯�� 
    ConcurrentArrayStack<T> & operator= (ConcurrentArrayStack<T> &&other) = delete;          // �����ƶ���ֵ����� 

    // 3. methods 
    bool push(T val);    // ��ջ����
    bool pop(T &val);    // ��ջ����
    bool top(T &val);    // ��ȡջ��Ԫ�ز���
    int length();        // ��ȡջ��Ԫ�ظ��� 

    // 4. destructor
    ~ConcurrentArrayStack();        // ��������

    // 5. domain
    ListNode<T> *pos;         // ջ��Ԫ�ؽ��ָ��
    int size;                 // ջ��Ԫ�ظ���
    std::mutex mut;                // ����д�뻥���� 
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
    ListNode<T> *newnode=new (std::nothrow)ListNode<T>(val);   // std::nothrowָ��new��������ڴ�����ʧ�ܲ��׳��쳣�����Ƿ��ؿ�ָ�� 
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
    if(!pos)           // �жϲ���Ҳ���봦�ڻ����������£�����if����return���֮��������������̶�ջ�����˲��������»�ȡ��˲��õ��Ľ���Ͳ�׼ȷ 
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
