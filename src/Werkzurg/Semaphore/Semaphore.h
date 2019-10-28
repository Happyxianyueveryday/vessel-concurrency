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
	inline void wait();        // P���� 
	inline void notify();      // V����
	
	// 4. destructor
	~Semaphore();
	
	// 5. domain 
	private:
	std::recursive_mutex mut;      	// ������ 
	std::condition_variable_any cond;    // �������� 
	unsigned int count;         // ��Դ������ 
};

/**
 * Semaphore: ���캯�� 
 */ 
Semaphore::Semaphore():count(0)
{
}

/**
 * wait: �ź���P���� 
 */
inline void Semaphore::wait()
{
	std::unique_lock<std::recursive_mutex> ulock(mut);
	cond.wait(ulock, [this]() {return count>0;});    // ֱ����Դ��������0ʱP�����ſ������
	count--;    // ������Դ����   
}

/**
 * notify: �ź���V���� 
 */
inline void Semaphore::notify()
{
	std::unique_lock<std::recursive_mutex> ulock(mut);
	count++;    // ������Դ���� 
	cond.notify_one();
}

/**
 * ~Semaphore: �������� 
 */
Semaphore::~Semaphore()
{
}

#endif
