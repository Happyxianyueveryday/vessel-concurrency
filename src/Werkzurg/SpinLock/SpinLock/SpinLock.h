#ifndef SPIN_LOCK_H           
#define SPIN_LOCK_H
#include <atomic>

/** 
 * SpinLock: 非公平的基础自旋锁 
 */
class SpinLock
{
	public: 
	// 1. constructor
	SpinLock();    // 构造函数 
	
	// 2. copy/move controller
	SpinLock(const SpinLock &other) = delete;
	SpinLock(SpinLock &&other) = delete;
	SpinLock &operator= (const SpinLock &other) = delete;
	SpinLock &operator= (SpinLock &&other) = delete;
	
	// 3. methods
	void lock();   // 自旋锁加锁 
	void unlock(); // 自旋锁解锁 
	
	private:
	// 4. domains
	std::atomic<bool> flag;  
};

/**
 * SpinLock: 构造函数，创建未上锁的自旋锁 
 */ 
SpinLock::SpinLock()
{
	flag.store(false);    // flag==false表示当前自旋锁未加锁 
}

/** 
 * lock: 自旋锁加锁 
 */
void SpinLock::lock()
{
	bool expected=false;
	while(!flag.compare_exchange_weak(expected, true)) 
	{
		// 注意循环尝试执行CAS操作时，需要复原期望值变量的初始值，因为compare_exchange_weak方法若执行CAS操作失败，会将当前原子变量的值存储在expected中 
		expected=false;  
	}
}

/**
 * unlock: 自旋锁解锁 
 */
void SpinLock::unlock()
{
	flag.store(false);
}

#endif
