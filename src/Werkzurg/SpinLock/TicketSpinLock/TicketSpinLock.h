#ifndef TICKET_SPIN_LOCK_H             
#define TICKET_SPIN_LOCK_H
#include <atomic>

/** 
 * TicketSpinLock: 公平的自旋锁 
 * note: 公平的自旋锁通过时间序号来进行实现，核心机制是使用两个序号，排队号和服务号，具体请参见实现  
 */
class TicketSpinLock
{
	public: 
	// 1. constructor
	TicketSpinLock();    // 构造函数 
	
	// 2. copy/move controller 
	TicketSpinLock(const TicketSpinLock &other) = delete;
	TicketSpinLock(TicketSpinLock &&other) = delete;
	TicketSpinLock &operator= (const TicketSpinLock &other) = delete;
	TicketSpinLock &operator= (TicketSpinLock &&other) = delete;
	
	// 3. methods
	unsigned int lock();                // 自旋锁加锁
	void unlock(unsigned int ticket);   // 自旋锁解锁
	
	private:
	// 4. domains
	std::atomic<unsigned int> wait_no;       // 排队号，所有想要加锁的线程首先均会获得一个序号作为排队号，然后持有该排队号进行等待 
	std::atomic<unsigned int> service_no;    // 服务号，下一个获得锁的线程期望应该具有的排队号
};

/**
 * TicketSpinLock: 构造函数，创建未上锁的自旋锁 
 */ 
TicketSpinLock::TicketSpinLock()
{
	wait_no.store(0);        // 初始状态下，下一个想要加锁线程所分配的排队号为0 
	service_no.store(0);     // 初始状态下，下一个获取到锁的线程的期望序号为0 
}  

/** 
 * lock: 自旋锁加锁 
 */
unsigned int TicketSpinLock::lock()  
{
	// 1. 首先向当前想要获得锁的线程分配一个排队号，然后将排队号自增1，以分配给下一个想要加锁的线程 
	unsigned int ticket=wait_no.fetch_add(1);
	// 2. 然后使用CAS循环比较当前线程分配到的排队号是否与服务号相等，相等则获得当前锁，但是不增加服务号（因为该线程尚未完成操作）  
	while(service_no.load()!=ticket);
	// 3. 获取到当前锁后，返回所获取的排队号，该排队号用于后续的解锁
	return ticket;   
}

/**
 * unlock: 自旋锁解锁 
 */
void TicketSpinLock::unlock(unsigned int ticket)
{
	// 当前线程的操作全部完成，自增服务号以释放当前锁并且允许下一个线程获得锁 
	unsigned int expected=ticket; 
	while(!service_no.compare_exchange_weak(expected, ticket+1)) 
	{
		expected=ticket;    // 期望值变量expected复位 
	}
}

#endif
