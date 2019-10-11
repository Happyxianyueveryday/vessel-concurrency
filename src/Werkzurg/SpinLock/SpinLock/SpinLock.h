#ifndef SPIN_LOCK_H           
#define SPIN_LOCK_H
#include <atomic>

/** 
 * SpinLock: �ǹ�ƽ�Ļ��������� 
 */
class SpinLock
{
	public: 
	// 1. constructor
	SpinLock();    // ���캯�� 
	
	// 2. copy/move controller
	SpinLock(const SpinLock &other) = delete;
	SpinLock(SpinLock &&other) = delete;
	SpinLock &operator= (const SpinLock &other) = delete;
	SpinLock &operator= (SpinLock &&other) = delete;
	
	// 3. methods
	void lock();   // ���������� 
	void unlock(); // ���������� 
	
	private:
	// 4. domains
	std::atomic<bool> flag;  
};

/**
 * SpinLock: ���캯��������δ������������ 
 */ 
SpinLock::SpinLock()
{
	flag.store(false);    // flag==false��ʾ��ǰ������δ���� 
}

/** 
 * lock: ���������� 
 */
void SpinLock::lock()
{
	bool expected=false;
	while(!flag.compare_exchange_weak(expected, true)) 
	{
		// ע��ѭ������ִ��CAS����ʱ����Ҫ��ԭ����ֵ�����ĳ�ʼֵ����Ϊcompare_exchange_weak������ִ��CAS����ʧ�ܣ��Ὣ��ǰԭ�ӱ�����ֵ�洢��expected�� 
		expected=false;  
	}
}

/**
 * unlock: ���������� 
 */
void SpinLock::unlock()
{
	flag.store(false);
}

#endif
