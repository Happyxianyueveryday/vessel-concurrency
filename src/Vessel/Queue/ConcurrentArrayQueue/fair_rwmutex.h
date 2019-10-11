#ifndef FAIR_RWMUTEX_H 
#define FAIR_RWMUTEX_H               
#include <mutex>    

/**
 * fair_rwmutex: 基于互斥量实现的读优先的读写锁  
 * note 1: 读写锁: 是指允许多个线程同时进行读操作，但只允许一个线程同时进行写操作，且读写操作之间不能同时进行的一种互斥锁 
 * note 2: 读写公平法: 不论CPU当前没有被抢占，被写线程或者读线程抢占，正在等待的线程都遵循到达的顺序抢占CPU，先来先抢占，而不区分读线程还是写线程 
 * note 3: 读写公平法的实现核心：新建一个队列互斥锁seq，通过该互斥锁控制所有到达的写线程或者读线程在该互斥锁上排队，从而实现先来先抢占的读写平等 
 */
class fair_rwmutex      
{
	public: 
	// 1. constructor
	fair_rwmutex();   		   // 默认构造函数 
	
	// 2. copy/move controller = delete
	fair_rwmutex(const fair_rwmutex &other) = delete;               // 读写锁禁用拷贝构造函数 
	fair_rwmutex &operator= (const fair_rwmutex &other) = delete;
	fair_rwmutex(fair_rwmutex &&other) = delete;
	fair_rwmutex &operator= (fair_rwmutex &&other) = delete;
	
	// 3. methods
	void read_lock();      // 读者线程加锁，读取操作前加锁 
	void read_unlock();    // 读者线程解锁，读取操作后解锁 
	void write_lock();     // 写者线程加锁，写入操作前加锁 
	void write_unlock();   // 写者线程加锁，写入操作后解锁 
	
	private:
	// 4. domains 
	std::mutex count_mut;       // 负责读者计数器count修改的互斥锁 
	int count;             // 读者数量的计数器 
	std::mutex write_mut;       // 负责写者间互斥写的互斥锁 
	std::mutex seq_mut;         // 实现读写公平的互斥锁 
};

/**
 * fair_rwmutex: 初始化独写锁 
 */  
fair_rwmutex::fair_rwmutex()
{
	count=0;    // 设置读者计数器为0 
}

/**
 * read_lock: 读者线程加锁 
 * note 1: 读者线程加锁包括以下两个重要步骤： 
 *       1. 互斥地将读者计数器count增加1 
 *       2. 互斥地判断当前读者数量是否为1，若为1则当前线程为第一个读线程，则加锁write_mut，终止写者的写操作
 * note 2: 读写公平法中必须使用一个新的互斥锁保证read_lock()方法和write_lock()方法之间可以互斥执行 
 */
void fair_rwmutex::read_lock()
{
	seq_mut.lock();     // 保证互斥执行read_lock和write_lock，P操作 
	count_mut.lock();   // 互斥访问读者计数器count，P操作 
	count+=1;           // 将读者数量增加1 
	if(count==1)        // 若读者数量增加1后值为1，则为第一个读者，根据读优先原则，首先加锁写者的互斥锁，使得写者的写操作被延迟到读者后进行 
	write_mut.lock();   // 加锁写者的互斥锁       
	count_mut.unlock(); // 互斥访问读者计数器count，V操作 
	seq_mut.unlock();   // 保证互斥执行read_lock和write_lock，V操作 
} 

/**
 * read_unlock: 读者线程解锁 
 * note: 读者线程解锁包括以下两个重要步骤：
 *		 1. 互斥地将读者计数器count减少1 
 *       2. 互斥地判断当前读者计数器是否为0，若为0则说明所有读者已经执行完读操作，这时解锁写者的互斥锁，使得被延迟的写者能够继续进行写操作 
 */  
void fair_rwmutex::read_unlock()
{
	count_mut.lock();   // 互斥访问读者计数器count，P操作
	count-=1;           // 减小读者计数器count 
	if(count==0)        // 若读者数量减小后为0，则所有的读者已经进行完了读操作，这时释放写者的互斥锁，允许写者进行写操作 
	write_mut.unlock(); // 解锁写者的互斥锁 
	count_mut.unlock(); // 互斥访问读者计数器count，V操作 
} 

/**
 * write_lock: 写者加锁 
 */
void fair_rwmutex::write_lock()
{
	seq_mut.lock();     // 保证互斥执行read_lock和write_lock，P操作 
	write_mut.lock();
	seq_mut.unlock();   // 保证互斥执行read_lock和write_lock，V操作 
} 

/**
 * write_unlock: 写者解锁 
 */
void fair_rwmutex::write_unlock()
{
	write_mut.unlock();
}

 

#endif
