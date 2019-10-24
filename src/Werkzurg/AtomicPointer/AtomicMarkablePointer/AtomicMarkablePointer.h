#ifndef ATOMIC_MARKABLE_POINTER_H 
#define ATOMIC_MARKABLE_POINTER_H 
#include "AtomicPair.h" 
#include <atomic>

/**
 * AtomicMarkablePointer: 带有版本号的原子指针模板 
 * note: AtomicMarkablePointer仅仅是包装了版本号的线程安全指针，本身不负责指针所指向对象的回收 
 */
template<typename T>
class AtomicMarkablePointer
{
	public:
	// 1. constructor
	AtomicMarkablePointer(T *_data);   // 构造函数 
	
	// 2. copy/move controller
	AtomicMarkablePointer(const AtomicMarkablePointer &other) = delete;               // 原子模板禁用拷贝和移动 
	AtomicMarkablePointer(AtomicMarkablePointer &&other) = delete;
	AtomicMarkablePointer &operator= (const AtomicMarkablePointer &other) = delete;
	AtomicMarkablePointer &operator= (AtomicMarkablePointer &&other) = delete;
	
	// 3. methods
	bool is_lock_free();    // 判断是否支持无锁原子操作，不支持则会使用自旋锁
	bool get_flag();
	T* get_ptr();
	bool compare_exchange_weak(T *expected_data, T *new_data, bool expected_flag, bool new_flag);
	bool compare_exchange_strong(T *expected_data, T *new_data, bool expected_flag, bool new_flag); 
	
	// 4. destructor
	~AtomicMarkablePointer(); 
	
	private:
	// 5. domain
	std::atomic<AtomicPair<T> *> pair; 
}; 

/**
 * AtomicMarkablePointer: 构造函数 
 */
template<typename T>
AtomicMarkablePointer<T>::AtomicMarkablePointer(T *_data)
{
	pair=AtomicPair<T>::factory(_data, 0);
}

/**
 * is_lock_free: 判断该原子版本指针的所有操作是否为无锁的 
 * note: 只要封装的AtomicPair<T> *指针的所有操作无锁，则原子版本指针AtomicMarkablePointer的操作是无锁的 
 */
template<typename T>
bool AtomicMarkablePointer<T>::is_lock_free()
{
	return pair.is_lock_free();  
}

/**
 * get_stamp: 获取当前的版本号 
 */
template<typename T>
bool AtomicMarkablePointer<T>::get_flag()
{
	return pair.load()->flag;
}

/**
 * get_ptr: 获取封装的裸指针 
 */
template<typename T>
T* AtomicMarkablePointer<T>::get_ptr()
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
 */
template<typename T>
bool AtomicMarkablePointer<T>::compare_exchange_weak(T *expected_data, T *new_data, bool expected_flag, bool new_flag)
{
	AtomicPair<T> *current=pair.load();     	// 当前AtomicMarkablePointer所保存的AtomicPair 
	return expected_data==current->data&&      
		expected_flag==current->flag&&     
		((new_data==current->data&&new_data==current->data)||pair.compare_exchange_weak(current, AtomicPair<T>::factory(new_data, new_flag)));       
}

template<typename T>
bool AtomicMarkablePointer<T>::compare_exchange_strong(T *expected_data, T *new_data, bool expected_flag, bool new_flag)
{
	AtomicPair<T> *current=pair.load();     	 // 当前AtomicMarkablePointer所保存的AtomicPair 
	return expected_data==current->data&&      
		expected_flag==current->flag&&    
		((new_data==current->data&&new_flag==current->flag)||pair.compare_exchange_strong(current, AtomicPair<T>::factory(new_data, new_flag)));       // 利用短路求值进行CAS 
}

template<typename T>
AtomicMarkablePointer<T>::~AtomicMarkablePointer()
{
}

#endif
