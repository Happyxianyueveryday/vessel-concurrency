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
    ListNode<T> *front;      // ����Ԫ��ָ��
    ListNode<T> *end;        // ��βԪ��ָ��
    int size;                // ����Ԫ�ظ��� 
    std::mutex mut;               // ������л�����     
};

template<typename T> 
ConcurrentLinkedQueue<T>::ConcurrentLinkedQueue()
{
    front=new ListNode<T>(0);   // ����ͷ���ƽ��
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
	
	other.front=new ListNode<T>(0);   // ����ͷ���ƽ��
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
        
        if(temp==end)   // ע�⣬ɾ�����һ��Ԫ��ʱendָ��ҲҪ�ֶ����� 
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
