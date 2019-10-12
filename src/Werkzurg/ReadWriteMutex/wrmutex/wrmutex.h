#ifndef WRMUTEX_H
#define WRMUTEX_H
#include <mutex>         

/**
 * wrmutex: 基于互斥量实现的写优先的读写锁
 * note 1：读写锁：允许多个线程同时进行读操作，但只允许一个线程同时进行写操作，且读写操作之间不能同时进行的一种互斥锁 
 * note 2：写优先：若CPU未被抢占或者被读线程抢占，则读写线程公平抢占；若CPU被写线程抢占，则后续在队列中等待的写线程将被优先执行，读线程将被延迟执行 
 * note 3：写优先的实现：写优先在读优先实现的基础上：  
 *         1. 增加一个互斥锁seq作为等待队列，所有到达的读写线程最后均要在这个队列上等待
 *         2. 增加一个计数器write_count用于计算写线程的数量，以及访问该计数器对应的互斥锁write_count_mut
 *         3. 和读优先的实现思路类似，增加一个加在读线程上的互斥锁read_mut来实现写优先，当写线程数量首次不为0（即第一个写线程）时锁定read_mut，当写线程数量重新归零（即最后一个写线程）时释放read_mut 
 *         4. 增加一个缓冲队列read_temp_seq，所有到达的读线程首先在这里排队，每次只允许其中一个读线程离开该队列到seq上排队，只有该线程执行完毕后才允许缓冲队列将下一个读线程加入到等待队列中 
 */
class wrmutex
{
	public: 
	// 1. constructor
	wrmutex();   		   // 默认构造函数 
	
	// 2. copy/move controller = delete
	wrmutex(const wrmutex &other) = delete;               // 读写锁禁用拷贝构造函数 
	wrmutex &operator= (const wrmutex &other) = delete;
	wrmutex(wrmutex &&other) = delete;
	wrmutex &operator= (wrmutex &&other) = delete;
	
	// 3. methods
	// 附注：关键点在于lock的操作，unlock更像是lock的逆序反向操作 
	void read_lock();      // 读者线程加锁，读取操作前加锁 
	void read_unlock();    // 读者线程解锁，读取操作后解锁 
	void write_lock();     // 写者线程加锁，写入操作前加锁 
	void write_unlock();   // 写者线程加锁，写入操作后解锁 
	
	private:
	// 4. domains  
	std::mutex read_count_mut;       // 负责读者计数器read_count修改的互斥锁 
	int read_count;             // 读者数量的计数器 
	std::mutex write_count_mut;      // 负责写者计数器write_count的计数器 
	int write_count;            // 写者数量的计数器 
	std::mutex seq;                  // 作为队列的互斥量，到达的读写线程都首先在该互斥量上阻塞（相当于排队） 
	std::mutex temp_seq;             // 缓冲队列，到达的读线程首先进入缓冲队列，每次只将一个读线程放入队列seq中 
	std::mutex write_mut;            // 负责写者间互斥写的互斥锁 
	std::mutex read_mut;             // 负责实现写者优先的互斥锁 
};

/**
 * wrmutex: 初始化读写锁 
 */  
wrmutex::wrmutex()
{
	read_count=0;    // 设置读者计数器为0 
	write_count=0;   // 设置写者计数器为0 
}

/**
 * read_lock: 读者加锁 
 * note: 读线程加锁的基本操作如下： 
 * 		 1. 将读者线程加入临时缓冲队列temp_seq
 *       2. 然后尝试加锁等待队列的互斥锁seq，如果能够加锁，则说明等待队列中唯一的读线程已经被执行，则这时将自身加入等待队列中
 *       3. 将读线程计数器互斥地增加1，若当前线程为第一个读线程，则锁定写线程的互斥锁 
 */
void wrmutex::read_lock()
{
	temp_seq.lock();        // 当前读线程加入到缓冲队列
	seq.lock();             // 只有当等待线程中的唯一读线程执行完毕，释放锁之后，将当前线程加入到等待线程 
	
	read_count_mut.lock();  // 互斥访问读线程计数器 
	read_count+=1;          // 读线程计数器增加1 
	if(read_count==1)       // 若当前线程为第一个读线程，则锁定写线程的互斥锁，延迟写线程执行 
	write_mut.lock();
	read_count_mut.unlock();
	
	// 用户读操作代码 ... 
} 

/**
 * read_unlock: 读者解锁 
 * note: 读线程解锁的基本操作如下：
 *       1. 将读线程计数器互斥地减小1，若当前线程为最后一个读线程，则释放写线程的互斥锁 
 */  
void wrmutex::read_unlock()
{
	// 用户读操作代码 ...
	 
	read_count_mut.lock();   // 互斥访问读线程计数器 
	read_count-=1;           // 读线程计数器减小1 
	if(read_count==0)        // 若当前线程为最后一个读线程，则释放写线程的互斥锁，恢复写线程执行 
	write_mut.unlock();
	read_count_mut.unlock();
	
	seq.unlock();      // 释放锁退出等待队列 
	temp_seq.unlock(); // 释放锁退出缓冲队列 
} 

/**
 * write_lock: 写者加锁 
 * note: 写线程加锁的步骤如下：
 *       1. 互斥地增加写线程的计数器，若当前写线程为第一个到达的写线程（即增加后计数器值为1），则锁定读线程的互斥锁以延迟读线程 
 *       2. 通过尝试锁定等待队列的互斥锁，将自身加入线程等待队列 
 *       3. 通过尝试锁定写操作互斥锁，保证当前写线程和队列中的其他写线程互斥执行 
 */
void wrmutex::write_lock()
{
	// 值得提醒的是，为什么不把写线程计数增加操作放到后面，这是因为写线程之间总是互斥执行的，因此放到后面的话计数永远不会超过1，在创建写线程时就应当增加计数，而不是等到运行之后，同理知道最后写线程运行完毕准备销毁时，才减少计数，而不是刚运行结束就减小计数 
	write_count_mut.lock();  // 互斥访问写线程计数器 
	write_count+=1;          // 增加写线程的数量 
	if(write_count==1)       // 若当前写线程为第一个到达的写线程，则将读线程的互斥锁加锁，以延迟读线程的执行，保证写线程优先 
	read_mut.lock();
	write_count_mut.unlock();
	
	seq.lock();       // 将当前写线程加入等待队列 
	write_mut.lock(); // 和队列中其他写线程互斥进行写操作
	 
	// 用户写操作代码 ... 
} 

/**
 * write_unlock: 写者解锁 
 * note: 写线程解锁只有两步基本操作，和写者加锁中获取锁操作依次对应：
 *       1. 释放写操作互斥锁 
 *       2. 释放等待队列锁 
 *       3. 减小当前写线程的计数，若当前写线程为最后一个到达的写线程，则将读线程的互斥锁read_mut解锁，允许读线程的执行 
 */
void wrmutex::write_unlock()
{
	// 用户写操作代码 ... 
	
	write_mut.unlock();   // 写操作完成，当前写线程释放写操作互斥锁 
	seq.unlock();         // 释放等待队列互斥锁，退出队列，下一个线程获得该锁并执行 
	
	write_count_mut.lock();  // 互斥访问写线程计数器 
	write_count-=1;          // 写线程计数器减小1 
	if(write_count==0)       // 若当前写线程为最后一个写线程，则释放读线程的互斥锁 
	read_mut.unlock();
	write_count_mut.unlock();
}

 

#endif
