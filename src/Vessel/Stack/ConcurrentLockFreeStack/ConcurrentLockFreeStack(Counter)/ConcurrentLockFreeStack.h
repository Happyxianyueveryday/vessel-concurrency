#ifndef CONCURRENT_LOCK_FREE_STACK
#define CONCURRENT_LOCK_FREE_STACK
#include "AtomicStampPointer.h"
#include <thread>
#include <string>

template<typename T>
struct ListNode
{
	ListNode(T _val=T()):val(std::make_shared<T>(_val)), next(nullptr)
	{
	}
	std::shared_ptr<T> val;    // ����ֵ��ʹ������ָ����б��� 
	ListNode<T> *next;    // ָ����һ������ָ�룬����ջ��CAS����Ҫ��ͷ���head���޸ģ��������ʹ��ԭ������߰汾ָ�� 
};

/**
 * ConcurrentLockFreeStack: ����ջ�������̼߳��������� 
 * note: ����ջ���������ջ��������������������Ҫ�����һ��ABA���⣬���ǳ�ջ������ڴ���գ����߳�ͬʱpop��ʱ���ս�㣩�����⣻��һ����������ʹ�ð汾��ָ����������ڶ�����������ʹ���̼߳�����ʽ���н�� 
 */
template<typename T>
class ConcurrentLockFreeStack      
{ 
	public:
	// 1. constructor
	ConcurrentLockFreeStack();   
	
	// 2. copy/move controller
	ConcurrentLockFreeStack(const ConcurrentLockFreeStack<T> &other) = delete;      		 // ���ÿ������캯�� 
	ConcurrentLockFreeStack &operator= (const ConcurrentLockFreeStack<T> &other) = delete;   // ���ÿ�����ֵ�����
	ConcurrentLockFreeStack(ConcurrentLockFreeStack<T> &other);                              // �����ƶ����캯�� 
	ConcurrentLockFreeStack &operator= (ConcurrentLockFreeStack<T> &&other) = delete;   	 // �����ƶ���ֵ�����  
	
	// 3. methods
	bool push(T val);            // ��ջ 
	std::shared_ptr<T> pop();    // ��ջ 
	void print();                // �����÷���: ���ջ 
	
	// 4. destructor
	~ConcurrentLockFreeStack();
	
	private:
	// 5. domain 
	AtomicStampPointer<ListNode<T>> head;    // ջ�����ָ�� 
	unsigned int pop_count;                  // ���ڵ���pop�������̵߳ļ����� 
	std::atomic<ListNode<T>*> bin_head;      // �����յĽ������ 
	
	// 6. private function
	void gc(ListNode<T> *old_head);          // ������������� 
};

/**
 * ConcurrentLockFreeStack: ���캯�� 
 */
template<typename T>
ConcurrentLockFreeStack<T>::ConcurrentLockFreeStack():head(nullptr), bin_head(nullptr), pop_count(0)
{
}

/**
 * push: ����ջ��ջ���� 
 * note: ����ջ����ջ�����������£�
 *		 1. �����µ���ջ���
 *       2. ���½���nextָ��ָ��ͷ���
 *       3. ʹ��CAS������ڵ�ͷ����Ƿ���½���nextָ��ָ��Ľ����ͬ������ͬ������ͷ���Ϊ�½�� 
 */
template<typename T>
bool ConcurrentLockFreeStack<T>::push(T val)
{
	ListNode<T> *newnode=new ListNode<T>(val);
	newnode->next=head.get_ptr();
	while(!head.compare_exchange_weak(newnode->next, newnode, head.get_stamp(), head.get_stamp()+1));
}

/**
 * pop: ����ջ��ջ����
 * note 1: ����ջ�ĳ�ջ����Ҫ����ĺ��������ǣ����ܴ��ڶ��pop��������ִ�У��ں�ʱ�����ͷ�Ҫ��ջ�Ľ�㣬�ǳ�ջ������Ҫ����ĺ������� 
 * note 2: ����ʹ�õ����̼߳�����������������⣬�����㷨Ϊ:  
 *         1. �߳̽���pop����ʱ����pop�̼߳���������1 
 *         2. ʹ��CASɾ��ջ�����
 *         3. ����ɾ����ջ������ֵ 
 *         4. �������������������������������̼߳�������ֵ�ж��Ƿ���Ի���ɾ����ջ����㣬�����е�ǰһ���߳���ʹ��pop������Խ����������գ�����������������
 *         5. �߳�׼���뿪pop�������������������н�pop�̼߳�������С1�������س�ջ��ջ������ֵ 
 */
template<typename T>
std::shared_ptr<T> ConcurrentLockFreeStack<T>::pop()
{
	// 1. �߳̽���pop������������������1 
	++pop_count;       // ǰ�������������ԭ�Ӳ��� 
	// 2. ʹ��CAS����ɾ��ջ�����
	ListNode<T> *old_head=head.get_ptr();
	while(old_head&&!head.compare_exchange_strong(old_head, old_head->next, head.get_stamp(), head.get_stamp()+1));
	// 3. ����ջ������ֵ
	std::shared_ptr<T> res;
	if(old_head)
	res.swap(old_head->val);
	// 4. �̵߳������������������Ի�����ɾ����ջ����㣬�������������н�pop�̼߳���������1 
	gc(old_head);
	// 5. ���س�ջ��ջ������ֵ 
	return res;
}

/**
 * gc: ��ջ��������������
 * note: �������������㷨����:
 *       1. ��pop�̼߳�������ֵΪ1����ֻ��һ���߳���ִ��pop��������ʱ�ճ�ջ�Ľ�㼴������old_head�ض����԰�ȫ�ͷţ���ȡ�´���������bin_head
 *       2. �ٴ��ж�pop�̼߳�������ֵ��������һ���߳���ִ��pop����������ʱ�����ͷ�������old_head�ʹ���������bin_head�����ж���߳���ִ��pop����������ʱ�����ͷ�������old_head�������ͷŴ�����������ǽ�����������bin_head�Ż� 
 */
template<typename T>
void ConcurrentLockFreeStack<T>::gc(ListNode<T> *old_head)
{
	// 1. ���е�ǰ�߳���pop������ʱ������ĳ�ջ���old_headһ�����԰�ȫ�ؽ��л��գ��������ս������Ҫ���������������Ƿ���л��� 
	if(pop_count==1)      
	{
		ListNode<T> *tar_head=bin_head.exchange(nullptr);   // ԭ�ӵ�ȡ��֮ǰ�Ĵ����ս�������ͷ��㣬׼�����л��� 
		// 1.1 ����һ���߳���pop�����ڣ���ʱ�Ϳ��ԺϷ����ͷŴ����ս��������Ϊ�������ڼ�����Ϊ1��ʱ��ȡ�����ģ���˿�������ֱ���ͷ�
		if(--pop_count==0)  
		{
			while(tar_head)  
			{
				ListNode<T> *temp=tar_head;
				tar_head=tar_head->next;
				delete temp;
			}
		}
		// 1.2 ����ʱ�ж���߳���pop�����ڣ�����ʱ�����������ܰ�ȫ�ͷţ���ȡ�µĴ���������ǿգ��򽫴���������Ż� 
		else if(tar_head)  
		{
			// ��ȡ�µĴ���������Żص��㷨��: 
			// (1) ���Ȳ��ҵ�ȡ�µĴ����������β��� 
			ListNode<T> *tail=tar_head;
			while(tail->next)
			{
				tail=tail->next;
			} 
			// (2) ����ʱ�µĴ���������������֮ǰȡ�µĴ���������֮����һ������ʹ��CAS��� 
			tail->next=bin_head.load();
			while(!bin_head.compare_exchange_weak(tail->next, tar_head));
		} 
		delete old_head;  // ����ʱpop_countΪ1����˸ճ�ջ�Ľ���Ѿ������ͷ� 
	}
	// 2. ���������߶���߳���pop������ʱ���ͷų�ջ���old_head���ߴ����ս������bin_head���ǲ���ȫ����Ϊ����ʱ��Ҫ����ջ���old_head���뵽�����ս������bin_head�� 
	else
	{
		// ����ǰ����ĳ�ջ�����������������㷨��:
		// (1) ���Ƚ��������������ӵ���ǰ��ջ���֮��
		old_head->next=bin_head.load();
		// (2) ʹ��CAS���ô���������
		while(!bin_head.compare_exchange_weak(old_head->next, old_head));
		--pop_count;   // �����̼߳����� 
	}
}

/**
 * print: ��������ʹ�õķ��������ջ��Ԫ�أ��÷�����֧�ֲ�������
 */
template<typename T>
void ConcurrentLockFreeStack<T>::print()
{
	ListNode<T> *now=head.get_ptr();
	while(now)
	{
		std::cout<<*(now->val)<<" ";
		now=now->next;
	}
	std::cout<<std::endl;
}

/**
 * ~ConcurrentLockFreeStack: �������� 
 * note: �������������ͷ�ջ�е����н���Լ����������� 
 */
template<typename T>
ConcurrentLockFreeStack<T>::~ConcurrentLockFreeStack()
{
	// 1. �ͷ�ջ�е����н��
	ListNode<T> *now=head.get_ptr(); 
	while(now)
	{
		ListNode<T> *temp=now;
		now=now->next;
		delete temp;
	}
	// 2. �ͷŴ����������е����н��
	now=bin_head.load();
	while(now)
	{
		ListNode<T> *temp=now;
		now=now->next;
		delete temp;
	}
}

#endif
