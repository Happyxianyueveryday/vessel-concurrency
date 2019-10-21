#ifndef CLHLOCK_H
#define CLHLOCK_H
#include <atomic> 

struct CLHNode
{
	CLHNode()
	{	
		flag.store(true);
	}
	std::atomic<bool> flag;        // flag为false表示未加锁，为true表示已加锁  
};

/**
 * CLHLock: CLHLock根据当前线程所持有的队列结点的前驱结点的锁状态进行自旋
 * note: 该锁不可重入    
 */
thread_local CLHNode *local_node;    // 当前线程所持有的自己的结点
thread_local CLHNode *pred_node;     // 当前线程的结点的前驱结点 
 
class CLHLock  
{
	public:
	// 1. constructor
	CLHLock();
	
	// 2. copy/move controller
	CLHLock(const CLHLock &other) = delete;
	CLHLock(CLHLock &&other) = delete;
	CLHLock &operator= (const CLHLock &other) = delete;
	CLHLock &operator= (CLHLock &&other) = delete;
	
	// 3. methods
	void lock();
	void unlock();
	
	// 4. destructor
	~CLHLock();
	
	private:
	// 5. domain
	std::atomic<CLHNode*> tail;      		// 整个等待结点队列的尾结点 
};

/**
 * CLHLock: 构造函数
 * note: 构造函数创建一个CLHLock供多个线程使用，因此只需要初始化tail，local_node和prev_node为空即可 
 */
CLHLock::CLHLock()
{
	tail.store(nullptr);
	local_node=nullptr;
	pred_node=nullptr; 
}

/**
 * lock: CLHLock加锁操作
 * note 1: 某个线程进行CLHLock加锁的算法如下: 
 *		 1. 创建一个线程本地的结点，即local_node(thread_local)
 *       2. 将当前结点local_node加入排队队列，即使用atomic_exchange方法修改尾结点tail(非thread_local)为当前的本地结点，并将尾结点修改前的值返回并赋值给前驱结点变量pred_node(thread_local) 
 *	 	 3. 若获取的pred_node为空，则直接获得锁，若非空，在获取的前驱结点pred_node(thread_local)所指向的CLHNode的flag变量上自旋，直到flag==false时加锁  
 * note 2: 受限于c++不存在gc机制，一个线程每次进入就创建一个新的local_node作为当前结点，包括同一个线程多次加锁的情况 
 */
void CLHLock::lock()
{
	// 1. 创建当前线程的结点Local_node(thread_local) 
	local_node=new CLHNode(); 
	// 2. 修改tail为当前新创建的结点，并获得tail的旧值，该值就是当前结点的前驱结点pred_node
	pred_node=tail.exchange(local_node);
	// 3. 若pred_node==nullptr则直接获得锁，否则在前驱结点上进行自旋，当前驱结点的flag变量为false时即可进行加锁
	if(pred_node)
	{
		while(pred_node->flag.load()==true);    // 在前驱结点pred_node的flag变量上自旋，直到flag==false即可加锁
		delete pred_node;                       // 若获得锁则说明前驱结点对应的线程已经解锁，故释放前驱结点 
	}
	// 4. 最终获得锁 
}

/**
 * unlock: CLHLock解锁操作 
 * note 1: 某个线程进行CLHLock解锁的算法如下:  
 *       1. 将当前结点的状态重置为false
 * note 2: 受限于c++没有gc机制，当前线程并不能直接销毁当前结点，因为当前结点不知道是否存在后继结点需要在当前结点上自旋判断标志flag的情况，因此出了刚创建CLHLock时外，任意时刻必定有一个CLHNode没有被释放，这个结点的释放交给CLHLock的析构函数 
 */
void CLHLock::unlock()
{
	if(local_node)
	local_node->flag.store(false); 
}

/**
 * ~CLHLock: 析构函数 
 * note: 析构函数负责销毁最后一个CLHNode结点 
 */
CLHLock::~CLHLock()
{
	CLHNode *temp=tail.load(); 
	if(temp)
	delete temp;
}

#endif
