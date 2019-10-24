#ifndef ATOMIC_STAMP_POINTER_H  
#define ATOMIC_STAMP_POINTER_H  
#include "AtomicPair.h" 
#include <atomic>

/**
 * AtomicStampPointer: 带有版本号的原子指针模板 
 * note: AtomicStampPointer仅仅是包装了版本号的线程安全指针，本身不负责指针所指向对象的回收 
 */
template<typename T>
class AtomicStampPointer
{
	public:
	// 1. constructor
	AtomicStampPointer(T *_data);   // 构造函数 
	
	// 2. copy/move controller
	AtomicStampPointer(const AtomicStampPointer &other) = delete;               // 原子模板禁用拷贝和移动 
	AtomicStampPointer(AtomicStampPointer &&other) = delete;
	AtomicStampPointer &operator= (const AtomicStampPointer &other) = delete;
	AtomicStampPointer &operator= (AtomicStampPointer &&other) = delete;
	
	// 3. methods
	bool is_lock_free();       // 判断是否支持无锁原子操作，不支持则会使用自旋锁
	unsigned int get_stamp();  // 获取当前的版本号 
	const T* get_ptr();               // 获取封装的对象裸指针 
	bool compare_exchange_weak(T *&expected_value, T *new_value, unsigned int expected_version, unsigned int new_version);
	bool compare_exchange_strong(T *&expected_value, T *new_value, unsigned int expected_version, unsigned int new_version); 
	
	// 4. destructor
	~AtomicStampPointer();
	
	private:
	// 5. domain
	std::atomic<AtomicPair<T> *> pair; 
}; 

/**
 * AtomicStampPointer: 构造函数 
 */
template<typename T>
AtomicStampPointer<T>::AtomicStampPointer(T *_data)
{
	pair=AtomicPair<T>::factory(_data, 0);
}

/**
 * is_lock_free: 判断该原子版本指针的所有操作是否为无锁的 
 * note: 只要封装的AtomicPair<T> *指针的所有操作无锁，则原子版本指针AtomicStampPointer的操作是无锁的 
 */
template<typename T>
bool AtomicStampPointer<T>::is_lock_free()
{
	return pair.is_lock_free();  
}

/**
 * get_stamp: 获取当前的版本号 
 */
template<typename T>
unsigned int AtomicStampPointer<T>::get_stamp()
{
	return pair.load()->version;
}

/**
 * get_ptr: 获取封装的裸指针 
 */
template<typename T>
const T* AtomicStampPointer<T>::get_ptr()
{
	return pair.load()->data;
}

/** 
 * compare_exchange_weak: 原子指针CAS操作 
 * param expected_value: 期望当前的指针值，和标准库的接口不同，当前指针值不会通过expected_value这个参数返回 
 * param new_value: 需要更新的指针值
 * param expected_version: 期望当前的版本号，和标准库的接口不同，当前版本号不会通过expected_version这个参数返回  
 * param new_version: 需要更新的版本号 
 * note: 1. 若目标指针值和当前保存的指针值相等，期望版本号也一致，则进行CAS操作，创建新的一个AtomicPair对象替代原对象 
 *       2. 特别的，一个可选的优化是，如果要更新的值和当前保存的值完全相等，则无需进行更新
 *       3. 若目标指针值和当前保存的指针值不相等，则将目标指针值更新为当前保存的指针值 
 */
template<typename T>
bool AtomicStampPointer<T>::compare_exchange_weak(T *&expected_data, T *new_data, unsigned int expected_version, unsigned int new_version)
{
	AtomicPair<T> *current=pair.load();     	// 当前AtomicStampPointer所保存的AtomicPair 
	return (expected_data==current->data&&       
		expected_version==current->version&&     
		((new_data==current->data&&new_version==current->version)||pair.compare_exchange_weak(current, AtomicPair<T>::factory(new_data, new_version))))||
		(static_cast<bool>(expected_data=current->data)&&false);
}

template<typename T>
bool AtomicStampPointer<T>::compare_exchange_strong(T *&expected_data, T *new_data, unsigned int expected_version, unsigned int new_version)
{
	AtomicPair<T> *current=pair.load();     	 // 当前AtomicStampPointer所保存的AtomicPair 
	return (expected_data==current->data&&       
		expected_version==current->version&&     
		((new_data==current->data&&new_version==current->version)||pair.compare_exchange_strong(current, AtomicPair<T>::factory(new_data, new_version))))||
		(static_cast<bool>(expected_data=current->data)&&false);
}

template<typename T>
AtomicStampPointer<T>::~AtomicStampPointer()
{
}

#endif
