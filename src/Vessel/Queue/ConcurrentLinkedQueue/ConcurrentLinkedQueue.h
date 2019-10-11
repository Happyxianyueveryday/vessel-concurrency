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
    ConcurrentLinkedQueue(const ConcurrentLinkedQueue<T> &other);                           // �������캯��
    ConcurrentLinkedQueue<T> &operator= (const ConcurrentLinkedQueue<T> &other) = delete;   // ������ֵ�����
    ConcurrentLinkedQueue(ConcurrentLinkedQueue<T> &&other);                                // �ƶ����캯�� 
    ConcurrentLinkedQueue<T> &operator= (ConcurrentLinkedQueue<T> &&other) = delete;        // �ƶ���ֵ����� 

    // 3. methods
    bool push(T val) override;            // ������Ӳ���
    bool pop(T &val) override;            // ���г��Ӳ���
    bool frontval(T &val) override;       // ��ȡ����Ԫ��
    int length() override;                // ������г��ȣ��������е�Ԫ�ظ���

    // 4. destructor
    ~ConcurrentLinkedQueue();

	private:
    ListNode<T> *front;       // ����Ԫ��ָ��
    ListNode<T> *end;         // ��βԪ��ָ��
    int size;                 // ����Ԫ�ظ��� 
    std::string mode;              // ��д�������ȼ�ģʽ 
	rwmutex rwmut;            // �����ȵĶ�д�� 
    wrmutex wrmut;            // д���ȵĶ�д�� 
    fair_rwmutex fair_rwmut;  // ��д��ƽ�Ķ�д�� 
    
    // 5. private methods
    void set_lock(std::string lockmode);                     
};

template<typename T> 
ConcurrentLinkedQueue<T>::ConcurrentLinkedQueue(std::string _mode)
{
    front=new ListNode<T>(0);   // ����ͷ���ƽ��
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
	
	other.front=new ListNode<T>(0);   // ����ͷ���ƽ��
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
        
        if(temp==end)   // ע�⣬ɾ�����һ��Ԫ��ʱendָ��ҲҪ�ֶ����� 
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
