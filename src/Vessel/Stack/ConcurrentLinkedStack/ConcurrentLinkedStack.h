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
    ConcurrentLinkedStack(std::string mode);       // ���캯��

    // 2. copy/move controller
    ConcurrentLinkedStack(const ConcurrentLinkedStack<T> &other);                              // �������캯��
    ConcurrentLinkedStack<T> & operator= (const ConcurrentLinkedStack<T> &other) = delete;     // ���ÿ�����ֵ�����
    ConcurrentLinkedStack(ConcurrentLinkedStack<T> &&other);                                   // �ƶ����캯�� 
    ConcurrentLinkedStack<T> & operator= (ConcurrentLinkedStack<T> &&other) = delete;          // �����ƶ���ֵ����� 

    // 3. methods
    bool push(T val);    // ��ջ����
    bool pop(T &val);    // ��ջ����
    bool top(T &val);    // ��ȡջ��Ԫ�ز���
    int length();        // ��ȡջ��Ԫ�ظ��� 

    // 4. destructor
    ~ConcurrentLinkedStack();        // ��������

	private: 
    // 5. domain
    ListNode<T> *pos;         // ջ��Ԫ�ؽ��ָ��
    int size;                 // ջ��Ԫ�ظ���
    std::string mode;      		  // ʹ�õĶ�д��ģʽ 
    rwmutex rwmut;            // �����ȵĶ�д�� 
    wrmutex wrmut;            // д���ȵĶ�д�� 
    fair_rwmutex fair_rwmut;  // ��д��ƽ�Ķ�д��
	
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
	set_lock("write_lock");   // push��д���� 
    ListNode<T> *newnode=new (std::nothrow)ListNode<T>(val);   // std::nothrowָ��new��������ڴ�����ʧ�ܲ��׳��쳣�����Ƿ��ؿ�ָ�� 
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
	set_lock("write_lock");   // pop��д���� 
    if(!pos)           // �жϲ���Ҳ���봦�ڻ����������£�����if����return���֮��������������̶�ջ�����˲��������»�ȡ��˲��õ��Ľ���Ͳ�׼ȷ 
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
	set_lock("read_lock");    // top�Ƕ����� 
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
	set_lock("read_lock");    // length�Ƕ����� 
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
	set_lock("write_lock");    // ������д���� 
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
