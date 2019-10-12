#ifndef SHARED_PTR_H 
#define SHARED_PTR_H
#include "ptr_counter.h"

template<typename T>  // 友元模板前置声明 
class weak_ptr;

/**
 * shared_ptr: 智能指针类模板 
 */
template<class T> 
class shared_ptr
{
	public: 
	// 1. constructor
	shared_ptr();     			       // 默认构造函数
	shared_ptr(T *data);               // 从普通指针构造shared_ptr 
	
	// 2. copy/move controller
	shared_ptr(shared_ptr &other);                      // 拷贝构造函数 
	shared_ptr<T> &operator= (shared_ptr &other);       // 拷贝赋值运算符 
	shared_ptr(shared_ptr &&other);                 	// 移动构造函数 
	shared_ptr<T> &operator= (shared_ptr && other);     // 移动赋值运算符 
	
	// 3. methods
	T &operator* () const;            // 解引用运算符：解引用运算符应当始终返回当前对象的引用 
	T *operator-> () const;           // 箭头运算符：箭头运算符应当始终返回指向当前对象的指针，即this指针 
	bool unique() const;              // 判断所指向对象的智能指针是否唯一 
	
	// 4. destructor
	~shared_ptr();                    // 析构函数 
	
	friend class weak_ptr<T>;
	
	private:
	// 5. domain
	ptr_counter<T> *counter;          // 智能指针所引用的数据对象和对应计数器，这里智能指针实现的核心是counter指针永远不为空
};

/**
 * shared_ptr: 初始化一个智能指针指向对应类型的新对象 
 */
template<typename T>
shared_ptr<T>::shared_ptr()
{
	counter=new ptr_counter<T>(new T());       // 初始化对象计数器 
}

/**
 * shared_ptr: 初始化一个智能指针指向输入的参数对象 
 */
template<typename T>
shared_ptr<T>::shared_ptr(T *data)
{
	if(data)
	counter=new ptr_counter<T>(data);       // 初始化对象计数器 
	else
	counter=nullptr;
}
	
/** 
 * shared_ptr: 拷贝构造函数 
 * note: 拷贝构造函数首先将源指针的shared_ptr引用计数器增加1，然后浅拷贝源指针的计数器本身（计数器中包含引用对象） 
 * note: 注意参数other.counter和自身this.counter判空 
 */
template<typename T>
shared_ptr<T>::shared_ptr(shared_ptr &other)
{
	// 1. 将源指针的引用计数器增加1
	if(other.counter)      // other判空 
	other.counter->shared_plus();
	// 2. 浅拷贝源指针的引用计数器（包含引用对象本身） 
	counter=other.counter;
}

/**
 * operator=: 拷贝赋值运算符  
 * note: 拷贝赋值运算符的工作如下：
 *       1. 将源指针的shared_ptr引用计数器增加1，将当前指针的引用计数器减小1
 *       2. 在减小1后：
 *			(1) 若shared_ptr计数值为0但是weak_ptr不为0，即存在weak_ptr，则调用计数器的free_data释放计数器中的引用对象，但是不释放 
 *			(2) 若shared_ptr计数值为0且weak_ptr为0，即不存在weak_ptr，则析构计数器 
 *       3. 浅拷贝源指针的引用计数器（包含引用对象本身）
 * note: 注意参数other.counter和自身this.counter判空 
 */
template<typename T>
shared_ptr<T> &shared_ptr<T>::operator= (shared_ptr &other)  
{
	// 1. 判断是否为自身拷贝
	if(&other==this)
	return (*this);
	// 2. 将源指针的shared_ptr引用计数值增加1，将当前指针的引用计数值减小1 
	if(counter)          // 自身判空 
	counter->shared_sub(); 
	if(other.counter)    // other判空 
	other.counter->shared_plus();
	// 3. 在减小1后：
	// (1) 若shared_ptr计数值为0但是weak_ptr不为0，即存在weak_ptr，则调用计数器的free_data释放计数器中的引用对象，但是不释放计数器 
	// (2) 若shared_ptr计数值为0且weak_ptr为0，即不存在weak_ptr，则析构计数器 
	if(counter&&counter->get_shared_count()==0)   // 注意自身判空 
	{
		counter->free_data(); 
		if(counter->get_weak_count()==0)
		delete counter;
	}
	// 4. 浅拷贝源指针的计数器
	counter=other.counter; 
	// 5. 返回当前对象的引用
	return (*this); 
}

/**
 * shared_ptr: 移动构造函数 
 * note: 移动构造运算符直接浅拷贝源指针的计数器，然后将源指针的计数器置为默认值即可，移动操作并没有增加源指针指向对象的计数器 
 * note: 注意参数other.counter和自身this.counter判空 
 */
template<typename T>
shared_ptr<T>::shared_ptr(shared_ptr &&other)
{
	// 1. 浅拷贝源指针的计数器
	counter=other.counter; 
	// 2. 将源指针的计数器置为空 
	other.counter=nullptr;
}

/**
 * operator=: 移动赋值运算符
 * note: 移动赋值运算符的工作如下：
 *       1. 将自身指针的shared_ptr计数值减1，在减去1后： 
 *			(1) 若shared_ptr计数值为0但是weak_ptr不为0，即存在weak_ptr，则调用计数器的free_data释放计数器中的引用对象，但是不释放计数器，交由weak_ptr释放 
 *	        (2) 若shared_ptr计数值为0且weak_ptr为0，即不存在weak_ptr，则析构计数器
 *		 2. 浅拷贝源指针的计数器
 *       3. 将源指针的计数器重置为默认值 
 * note: 注意参数other.counter和自身this.counter判空 
 */
template<typename T>
shared_ptr<T> &shared_ptr<T>::operator= (shared_ptr && other)
{
	// 1. 判断是否自身赋值 
	if(&other==this) 
	return (*this); 
	// 2. 将自身指针的shared_ptr计数值减1，在减去1后： 
	// (1) 若shared_ptr计数值为0但是weak_ptr不为0，即存在weak_ptr，则调用计数器的free_data释放计数器中的引用对象，但是不释放计数器，交由weak_ptr释放 
	// (2) 若shared_ptr计数值为0且weak_ptr为0，即不存在weak_ptr，则析构计数器 
	if(counter)    // 自身判空 
	counter->shared_sub();
	if(counter&&counter->get_shared_count()==0)   // 注意自身判空 
	{
		counter->free_data();
		if(counter->get_weak_count()==0)
		delete counter;
	}
	// 3. 浅拷贝源指针的计数器
	counter=other.counter; 
	// 4. 重置源指针的计数器为默认值 
	other.counter=nullptr;
	// 5. 返回当前指针对象
	return (*this);
}

/**
 * operator*: 解引用运算符
 * note: 解引用运算符返回当前指针引用对象的引用 
 * note: 注意自身指针this.counter判空 
 */ 
template<typename T>
T &shared_ptr<T>::operator* () const
{
	if(counter)
	return *(counter->getdata());
}

/**
 * operator->: 箭头运算符 
 * note: 箭头运算符返回当前指针对象的指针
 * note: 注意自身指针this.counter判空  
 */ 
template<typename T>
T *shared_ptr<T>::operator-> () const
{
	if(counter)
	return counter->getdata();
	else
	return nullptr;
}

/**
 * unique: 判断当前指针指向对象的引用计数是否等于1 
 * note: 注意自身指针this.counter判空 
 */
template<typename T>
bool shared_ptr<T>::unique() const
{
	if(counter&&counter->get_shared_count()==1)
	return true;
	else
	return false; 
}

/**
 * ~shared_ptr: 析构函数    
 * note: 析构函数首先将计数器的shared_ptr计数值减小1，若减小后值为0则释放引用的对象，但是不释放计数器；若weak_ptr计数值也为0，则进一步释放计数器 
 * note: 注意自身指针this.counter判空 
 */
template<typename T>
shared_ptr<T>::~shared_ptr()
{
	if(counter)
	{
		counter->shared_sub();	
		if(counter->get_shared_count()==0)
		{
			counter->free_data();	
			if(counter->get_weak_count()==0)
			delete counter;
		}
	}
}

#endif
