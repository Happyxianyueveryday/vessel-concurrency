#ifndef CONCURRENTLINKEDSET_H              
#define CONCURRENTLINKEDSET_H              
#include "ConcurrentSet.h"
#include <iostream>
#include <string>
#include <mutex>

template<typename T>
class ListNode
{
	public:
    // 1. methods
    ListNode(T _val=T()):val(_val), next(nullptr)
    { 
    }
    
    inline void lock()
    {
    	mut.lock();
	}
    
    inline void unlock()
    {
    	mut.unlock();
	}
    
    // 2. domains
    T val;             // ���洢��ֵ 
    ListNode<T> *next; // ָ����һ�������ָ�� 
    
    private:
    std::mutex mut;         // ���Ļ�����  
};

/**
 * ConcurrentLinkedSet: ��������ͽ�����ʵ�ֵ��̰߳�ȫ�ļ���  
 * note: ʹ�ý�����ʵ���̰߳�ȫʱ����Ҫʮ��С�ĸ��������б��������������˳�򣬽�ֹ���������������ʱÿ�����ֻ�ܼ���һ�ν���һ�� 
 */
template<typename T>
class ConcurrentLinkedSet: public ConcurrentSet<T>
{
    public:
    // 1. constructor
    ConcurrentLinkedSet();                             // Ĭ�Ϲ��캯��

    // 2. copy/move controller
    ConcurrentLinkedSet(const ConcurrentLinkedSet &other) = delete;                 		// ���ÿ������캯��
    ConcurrentLinkedSet &operator= (const ConcurrentLinkedSet &other) = delete;             // ���ÿ�����ֵ�����
    ConcurrentLinkedSet(ConcurrentLinkedSet &&other) = delete;                      		// �����ƶ����캯��
	ConcurrentLinkedSet &operator= (ConcurrentLinkedSet &&other) = delete;                  // �����ƶ���ֵ����� 
    
    // 3. methods
    bool contain(const T &val) override;      // �жϸ���Ԫ���Ƿ��ڼ����� 
    bool add(const T &val) override;          // �򼯺�������ָ��Ԫ�� 
    bool remove(const T &val) override;       // ����ָ��λ��ɾ��Ԫ��
    int length() override;                    // ��ȡ���ϵĴ�С 
    std::string toString() override;               // �����ǰ����

    // 4. destructor
    ~ConcurrentLinkedSet();

    private:
    ListNode<T> *head;         // ���ڱ���Ԫ�ص���������hashCode��С�������� 
    
    // 5. private methods
    void safe_lock(ListNode<T> *node);
    void safe_unlock(ListNode<T> *node);
};

/**
 * ConcurrentLinkedSet: Ĭ�Ϲ��캯��
 */
template<typename T>
ConcurrentLinkedSet<T>::ConcurrentLinkedSet() 
{
    head=new ListNode<T>();    // ��������ͷ���ƽ��
}

/**
 * contain: �жϼ������Ƿ���ڸ���Ԫ�� 
 * param val: ������Ҫ�жϵ�Ԫ�� 
 * return: �������д��ڸ���Ԫ�أ��򷵻�true�����򷵻�false
 * note: ��ȡָ��λ�õ�����Ԫ��ʱ���ڱ���ʱʼ��ֻ��Ҫ����һ����㣬��ȡ����һ����㻥�������ƶ�ָ�뵽��һ����㣬�ͷŵ�ǰ���Ļ���������  
 */      
template<typename T>
bool ConcurrentLinkedSet<T>::contain(const T &val)
{	
	safe_lock(head);                // ͷ���ƽ������ 
	ListNode<T> *now=head;          // �����α�ָ�� 
	
	while(now)
	{
		if(now->val.hashCode()==val.hashCode()&&now->val.equals(val))    // �ȱȽ�hashCode��ֵ����hashCode��ͬ������Ƚ�equals�����߱ȽϽ������ȣ���ǰ������Ҫ���ҵ�ֵ������true 
		{
			safe_unlock(now);  // ����֮ǰ��Ҫ���ǽ�����ǰ��� 
			return true;
		}
		ListNode<T> *temp=now;    
		safe_lock(now->next);  // ���Ի�ȡ��һ�����Ļ����� 
		now=now->next;         // ��ȡ��̽��Ļ������ɹ����ƶ�ָ�뵽��һ�����  // debug note: ��ֹ�Ƚ�����ǰ�������ƶ�����Ϊһ��������now->next�Ϳ��ܱ��޸ģ���ʱnow=now->next�Ͳ���׼ȷ�� 
		safe_unlock(temp);     // ����֮ǰָ��Ľ�� 
	}
	
	safe_unlock(now);  // ���ҵ����������δ���ҵ�Ŀ��ֵ���򷵻�false 
	return false;
}

/**
 * add: �򼯺������Ԫ��
 * param val: Ҫ�����Ԫ��ֵ
 * return: ������ɹ����򷵻�true�����������Ѿ����ڸ�Ԫ�ص������ʧ�ܣ��򷵻�false
 * note: ����������Ȱ��ղ���Ԫ����ͬ�ķ�ʽ����������һ�������ҵ�������λ�õ�ǰ�����now��Ȼ��ִ�в�����̣����в���֮ǰ���ȱ����������now��now->next����ֹ�����������������㱻�޸ģ�Ȼ����в������ 
 */
template<typename T>
bool ConcurrentLinkedSet<T>::add(const T &val)
{
	safe_lock(head);   // ͷ���ƽ������
	ListNode<T> *now=head;  // �����α�ָ�� 
	
	while(now)   // ���α������ҵ�һ����ϣֵ����������Ľ�� 
	{      
		ListNode<T> *temp=now;
		safe_lock(now->next); // ���Ի�ȡ��һ�������� 
		// ����ǰ����ϣֵ������Ԫ�ع�ϣֵ��ȣ���ʵ����Ҳ��ȣ��򼯺����Ѿ����ڸ�Ԫ�أ�����ʧ�� 
		if(now!=head&&now->val.hashCode()==val.hashCode()&&now->val.equals(val))
		{
			// ��ʱ������now��now->next 
			safe_unlock(now);        // ע������������Ҫ���Խ�����Щ�����Ľ�� 
			safe_unlock(now->next);
			return false;
		}
		// �ж�now->next�Ƿ�Ϊ������λ�õĺ�̽�㣬�����Ѿ��������now->next��������в�����̣���ʱ�����ͷ��˽��now->next��ǰ�����now���Ӷ������˽������ټ����� 
		if(!now->next||now->next->val.hashCode()>val.hashCode())  // ��now->next���Ϊ�գ�����now->next�Ľ���ϣֵ���������㣬��now���ǲ���λ�õ�ǰ����㣬��ʼִ�в������ 
		{
			// ��ʱ�Ѿ�������now��now->next������ִ�в���
			ListNode<T> *newnode=new ListNode<T>(val);
			newnode->next=now->next;
			now->next=newnode;
			safe_unlock(now);
			safe_unlock(newnode->next);    // warning: ��Ҫд��safe_unlock(now->next)��now->next�ڲ���������Ѿ����޸Ĺ���
			return true; 
		}
		// ����������ƶ��α�ָ�뵽��һ����㣬Ȼ���ͷ�ǰһ��������
		now=now->next;
		safe_unlock(temp);
	}
	// ��ʱֻ��now������
	safe_unlock(now);
	return false;
}

/**
 * remove: ����ָ��λ��ɾ��Ԫ��
 * param index: ��ɾ����λ���±�
 * param val: ��ɾ����ֵ
 * note: ɾ���������Ȱ��ղ���Ԫ����ͬ�ķ�ʽ�����ҵ���ɾ������ǰ�����now��Ȼ��ִ��ɾ�����̣�ɾ������֮ǰ������Ҫ�������now��now->next��now->next->next������㣬���Ų���ִ��ɾ����������ɾ�������󣬽���ʣ�µ�������� 
 */ 
template<typename T>
bool ConcurrentLinkedSet<T>::remove(const T &val)
{
	safe_lock(head);        // ����ͷ�����
	ListNode<T> *now=head;  // �����α�ָ�� 
	
	while(now)    // ����������Ҵ�ɾ������ǰ����� 
	{
		ListNode<T> *temp=now;
		safe_lock(now->next);
		// ���ҵ���ɾ����㣬ִ��ɾ����������󷵻�true 
		if(now->next&&now->next->val.hashCode()==val.hashCode()&&now->next->val.equals(val))   
		{
			// ��ʱnow, now->next�Ѿ�����������Ҫ����now->next->next
			safe_lock(now->next->next);
			ListNode<T> *delnode=now->next;
			now->next=now->next->next;
			delete delnode;
			safe_unlock(now);
			safe_unlock(now->next);   // ע�ⲻҪд��safe_unlock(now->next->next)��now->next->next�Ѿ����ı� 
			return true;
		}
		// ����������в��� 
		now=now->next;
		safe_unlock(temp);
	}
	// ���ҽ�����δ���ҵ���ɾ����㣬����false 
	safe_unlock(now);   // ��Ҫ�����ͷŵ�ǰ���Ļ����� 
	return false;
}

/**
 * size: ��ȡ����ĳ���/Ԫ�ظ���
 */
template<typename T>
int ConcurrentLinkedSet<T>::length()
{
	int count=0;
    safe_lock(head->next);                 // ��ȡ�׽��Ļ����� 
	ListNode<T> *now=head->next;           // ���ڱ������α�ָ�� 
	
	while(now)
	{
		count+=1;	
		ListNode<T> *temp=now;
		safe_lock(now->next);              // ��ȡ��һ�����Ļ����� 
		now=now->next;
		safe_unlock(temp);                 // �ͷ���һ�����Ļ����� 
	}
	safe_unlock(now);
	return count;
}

/**
 * ~toString: �����ǰ���� 
 */
template<typename T>
std::string ConcurrentLinkedSet<T>::toString()
{
	std::string res; 
	safe_lock(head->next);                 // ��ȡ�׽��Ļ����� 
	ListNode<T> *now=head->next;           // ���ڱ������α�ָ�� 
	
	while(now)
	{
		res=res+((now==head->next)?"":"->")+now->val.toString();	
		ListNode<T> *temp=now;
		safe_lock(now->next);              // ��ȡ��һ�����Ļ����� 
		now=now->next;
		safe_unlock(temp);                 // �ͷ���һ�����Ļ����� 
	}
	safe_unlock(now);
	return res;
}

/**
 * safe_lock: ����ָ�밲ȫ���ļ�������  
 */
template<typename T>
void ConcurrentLinkedSet<T>::safe_lock(ListNode<T> *node)
{
	if(node)
	node->lock();
}

/**
 * safe_unlock: ����ָ�밲ȫ���Ľ������� 
 */
template<typename T>
void ConcurrentLinkedSet<T>::safe_unlock(ListNode<T> *node)
{
	if(node)
	node->unlock();  
}

/**
 * ~ConcurrentLinkedSet: ��������
 * note: ��������ͬ�����Լ���������ʵ����û�б�Ҫ������ʱ�Ѿ�û���̻߳��ڵ����̰߳�ȫ����ķ����� 
 */
template<typename T>
ConcurrentLinkedSet<T>::~ConcurrentLinkedSet() 
{
    ListNode<T> *now=head;

    while(now)
    {
        ListNode<T> *temp=now;
        now=now->next;
        delete temp;
    }
}

#endif
