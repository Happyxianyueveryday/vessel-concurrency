#ifndef TICKET_LOCAL_SPIN_LOCK_H                              
#define TICKET_LOCAL_SPIN_LOCK_H 
#include <thread>
#include <atomic> 

thread_local unsigned int ticket;   // 每个线程取得的排队号，不同线程具有一个不同的排队号副本，因此使用thread_local，每个线程都会具有一个独立的ticket变量的副本 

/** 
 * TicketLocalSpinLock: 公平的自旋锁 
 * note: 公平的自旋锁通过时间序号来进行实现，核心机制是使用两个序号，排队号和服务号，具体请参见实现  
 */
class TicketLocalSpinLock
{
	public: 
	// 1. constructor
	TicketLocalSpinLock();    // 构造函数 
	
	// 2. copy/move controller 
	TicketLocalSpinLock(const TicketLocalSpinLock &other) = delete;
	TicketLocalSpinLock(TicketLocalSpinLock &&other) = delete;
	TicketLocalSpinLock &operator= (const TicketLocalSpinLock &other) = delete;
	TicketLocalSpinLock &operator= (TicketLocalSpinLock &&other) = delete;
	
	// 3. methods
	void lock();     // 自旋锁加锁
	void unlock();   // 自旋锁解锁
	
	private:
	// 4. domains
	std::atomic<unsigned int> wait_no;       // 排队号，所有想要加锁的线程首先均会获得一个序号作为排队号，然后持有该排队号进行等待 
	std::atomic<unsigned int> service_no;    // 服务号，下一个获得锁的线程期望应该具有的排队号
};

/**
 * TicketLocalSpinLock: 构造函数，创建未上锁的自旋锁 
 */ 
TicketLocalSpinLock::TicketLocalSpinLock()
{
	wait_no.store(0);        // 初始状态下，下一个想要加锁线程所分配的排队号为0 
	service_no.store(0);     // 初始状态下，下一个获取到锁的线程的期望序号为0 
}  

/** 
 * lock: 自旋锁加锁 
 */
void TicketLocalSpinLock::lock()  
{
	// 1. 首先向当前想要获得锁的线程分配一个排队号，然后将排队号自增1，以分配给下一个想要加锁的线程 
	ticket=wait_no.fetch_add(1);
	// 2. 然后使用CAS循环比较当前线程分配到的排队号是否与服务号相等，相等则获得当前锁，但是不增加服务号（因为该线程尚未完成操作）    
	while(service_no.load()!=ticket);  
}
 
/**
 * unlock: 自旋锁解锁 
 */
void TicketLocalSpinLock::unlock()
{
	// 当前线程的操作全部完成，自增服务号以释放当前锁并且允许下一个线程获得锁 
	unsigned int expected=ticket; 
	while(!service_no.compare_exchange_weak(expected, ticket+1)) 
	{
		expected=ticket;    // 期望值变量expected复位 
	}
}

#endif
