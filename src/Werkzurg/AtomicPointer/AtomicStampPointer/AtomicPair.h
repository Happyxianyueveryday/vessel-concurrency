#ifndef ATOMIC_PAIR_H   
#define ATOMIC_PAIR_H     
#include <iostream>
#include <atomic> 

/**
 * AtomicPair: 原子性的键值对
 * note: 单个对象很容易保持原子性，但是AtomicPair中存在data和version两个值，因此需要保证两个值的不可变性，在AtomicStampPointer中保存一个AtomicPair的指针，从而转化为单变量（AtomicPair对象指针）的原子性问题 
 */
template<typename T> 
class AtomicPair
{
	public:
	static AtomicPair<T> *factory(T* _data, unsigned int _version);
	~AtomicPair();
		
	T *data;                      // 指向对象的指针，AtomicPair不负责该对象的生命周期管理 
	const unsigned int version;   // 对象修改的版本号
	
	private:
	AtomicPair(T* _data, unsigned int _version); 
};

/**
 * factory: 工厂静态方法，创建一个AtomicPair方法并返回其对象指针 
 */
template<typename T>
AtomicPair<T> *AtomicPair<T>::factory(T* _data, unsigned int _version)
{
	return new AtomicPair<T>(_data, _version);
} 
 
/**
 * AtomicPair: 私有构造函数 
 */
template<typename T>
AtomicPair<T>::AtomicPair(T* _data, unsigned int _version):data(_data), version(_version)
{
}

/**
 * AtomicPair: 析构函数 
 * note: AtomicStampPointer并不管理对象本身的释放 
 */ 
template<typename T>
AtomicPair<T>::~AtomicPair()
{
	//std::cout<<"析构\n";
}

#endif
