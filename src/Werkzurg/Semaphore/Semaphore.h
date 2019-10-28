#ifndef SEMAPHORE_H
#define SEMAPHORE_H
#include <mutex>
#include <condition_variable>

class Semaphore
{
	public:
	// 1. constructor
	Semaphore();
	
	// 2. copy/move controller
	Semaphore(const Semaphore &other) = delete;
	Semaphore &operator= (const Semaphore &other) = delete;
	Semaphore(Semaphore &&other) = delete;
	Semaphore &operator= (Semaphore &&other) = delete;
	
	// 3. methods
	inline void wait();        // P操作 
	inline void notify();      // V操作
	
	// 4. destructor
	~Semaphore();
	
	// 5. domain 
	private:
	std::recursive_mutex mut;      	// 互斥量 
	std::condition_variable_any cond;    // 条件变量 
	unsigned int count;         // 资源计数器 
};

/**
 * Semaphore: 构造函数 
 */ 
Semaphore::Semaphore():count(0)
{
}

/**
 * wait: 信号量P操作 
 */
inline void Semaphore::wait()
{
	std::unique_lock<std::recursive_mutex> ulock(mut);
	cond.wait(ulock, [this]() {return count>0;});    // 直到资源计数大于0时P操作才可以完成
	count--;    // 减少资源计数   
}

/**
 * notify: 信号量V操作 
 */
inline void Semaphore::notify()
{
	std::unique_lock<std::recursive_mutex> ulock(mut);
	count++;    // 增加资源计数 
	cond.notify_one();
}

/**
 * ~Semaphore: 析构函数 
 */
Semaphore::~Semaphore()
{
}

#endif
