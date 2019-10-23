#ifndef MCSLOCK_H 
#define MCSLOCK_H 
#include <atomic>  

struct MCSNode
{
	MCSNode():next(nullptr) 
	{	
		flag.store(true);
	}
	std::atomic<bool> flag; // flag: 锁状态，flag为false表示可加锁，为true表示不可加锁  
	MCSNode *next;   	    // next: 指向下一个MCSNode结点的指针 
};

/**
 * MCSLock: MCSLock根据当前线程所持有的队列结点锁状态进行自旋
 * note 1: 简单介绍一下CLHLock和MCSLock的区别：
 *         1. CLHLock的核心原理是加锁时当前结点首先自旋检查前驱结点的状态，然后决定是否加锁；解锁时直接修改当前结点的状态的状态即可 
 *         2. MCSLock的核心原理是加锁时直接自旋检查当前结点的状态，然后决定是否加锁；解锁时除了修改当前结点的状态，还需要修改后继结点的状态 
 * note 2: 该锁不可重入    
 */
thread_local MCSNode *local_node;    // 当前线程所持有的自己的结点
 
class MCSLock  
{
	public:
	// 1. constructor
	MCSLock();
	
	// 2. copy/move controller
	MCSLock(const MCSLock &other) = delete;
	MCSLock(MCSLock &&other) = delete;
	MCSLock &operator= (const MCSLock &other) = delete;
	MCSLock &operator= (MCSLock &&other) = delete;
	
	// 3. methods
	void lock();
	void unlock();
	
	// 4. destructor
	~MCSLock();
	
	private:
	// 5. domain
	std::atomic<MCSNode*> tail;      		// 整个等待结点队列的尾结点 
};

/**
 * MCSLock: 构造函数
 * note: 构造函数创建一个MCSLock供多个线程使用
 */
MCSLock::MCSLock()
{
	tail.store(nullptr);
}

/**
 * lock: MCSLock加锁操作
 * note: MCSLock的加锁操作只需要在当前结点上进行自旋检查状态即可 
 */
void MCSLock::lock()
{
	// 1. 创建当前线程的状态结点 
	local_node=new MCSNode();
	// 2. 将当前线程的状态结点加入到链表队列中
	MCSNode *prev=tail.exchange(local_node);
	// 3. 如果存在前驱结点，则将前驱结点和当前结点相连，自旋检查当前线程的状态结点，直到状态为false,也即可加锁时加锁成功
	if(prev)   
	{
		prev->next=local_node;
		while(local_node->flag.load()==true);
	}
	// 4. 若不存在前驱结点则直接加锁成功 
}

/**
 * unlock: MCSLock解锁操作
 * note: MCSLock的解锁操作首先判断如果当前结点存在后继结点，则需要修改后继结点的状态为false，使得后继结点可以加锁；然后检查当前结点是否为最后一个结点，是则置空尾结点；最后释放当前结点 
 */
void MCSLock::unlock()
{
	// 1. 检查是否存在后继结点，若存在后继结点，则修改后继结点的状态为可加锁 
	if(local_node->next)
	local_node->next->flag.store(false); 
	// 2. 然后检查当前结点是否为最后一个结点，是则置空尾结点 
	else 
	{
		MCSNode *expected=local_node;
		while(tail.compare_exchange_weak(expected, nullptr))
		{
			expected=local_node;
		}
	}
	// 3. 释放当前结点
	delete local_node;
}

/**
 * ~MCSLock: 析构函数 
 */
MCSLock::~MCSLock()
{
}

#endif
