#ifndef PTR_COUNTER_H 
#define PTR_COUNTER_H
#include <string>

template<typename T>    // 友元模板前置声明 
class weak_ptr;

/** 
 * ptr_counter: 引用计数类
 * note: 该类维护两个计数weak_count和shared_count，分别代表指向当前对象的weak_ptr的数量和shared_ptr的数量
 * note: weak_ptr的核心原理是，由shared_ptr和对应的weak_ptr共同管理ptr_counter对象：
 	    1. 当shared_count的数量为0时，由shared_ptr对象释放计数器的对象指针；
		2.当shared_count和weak_count的数量同时为0时，由weak_ptr或者shared_ptr释放ptr_counter计数器对象。 
 */
template<typename T>
class ptr_counter
{
	public:
	// 1. constructor 
	ptr_counter();         // 默认构造函数 
	ptr_counter(T *_data); // 构造函数 
	
	// 2. copy/move controller 
	ptr_counter(const ptr_counter<T> &other) = delete;             		// 禁用拷贝构造函数 
	ptr_counter<T> &operator= (const ptr_counter<T> &other) = delete;   // 禁用拷贝赋值运算符
	ptr_counter(ptr_counter<T> &&other) = delete;              		    // 禁用移动构造函数
	ptr_counter<T> &operator= (ptr_counter<T> &&other) = delete;  		// 禁用移动赋值运算符
	
	// 3. methods 
	void shared_plus();    // 增加指向当前对象的shared_ptr的计数 
	void weak_plus();      // 增加指向当前对象的weak_ptr的计数
	void shared_sub();     // 减少指向当前对象的shared_ptr的计数
	void weak_sub();       // 减少指向当前对象的weak_ptr的计数 
	T* getdata() const;          	// 获取当前计数器所绑定的对象
	int get_shared_count() const; 	// 获取shared_ptr的计数 
	int get_weak_count() const;   	// 获取weak_ptr的计数
	void free_data();      // 释放引用的对象，但是不析构计数器本身 
	
	// 4. destructor 
	~ptr_counter();        // 析构函数
	
	friend class weak_ptr<T>;
	
	private:
	// 5. domains
	T* data;
	int *shared_count;
	int *weak_count;
};

/**
 * ptr_counter: 默认构造函数 
 */
template<typename T>
ptr_counter<T>::ptr_counter()
{
	data=new T();
	shared_count=new int(1);
	weak_count=new int(0);
}

/**
 * ptr_counter: 构造函数 
 * param data: 需要引用的对象 
 * note: 仅有shared_ptr有权调用ptr_counter的构造函数 
 */
template<typename T>
ptr_counter<T>::ptr_counter(T *_data)
{
	data=_data;
	shared_count=new int(1);
	weak_count=new int(0);
}

/**
 * shared_plus: 增加shared_ptr的计数器 
 */
template<typename T>
void ptr_counter<T>::shared_plus()
{
	*shared_count+=1;
} 

/**
 * shared_plus: 增加weak_ptr的计数器 
 */
template<typename T>
void ptr_counter<T>::weak_plus()
{
	*weak_count+=1; 
} 

/** 
 * shared_sub: 减小shared_ptr的计数器
 */ 
template<typename T>
void ptr_counter<T>::shared_sub()
{
	*shared_count-=1;
} 

/** 
 * weak_sub: 减小weak_ptr的计数器
 */ 
template<typename T>
void ptr_counter<T>::weak_sub()
{
	*weak_count-=1;
}

/**
 * getdata: 获取当前引用对象的指针 
 */
template<typename T>
T* ptr_counter<T>::getdata() const
{
	return data;
}

/**
 * get_shared_count: 获取shared_ptr的计数器 
 */
template<typename T>
int ptr_counter<T>::get_shared_count() const
{
	return *shared_count;
}

/**
 * get_weak_count: 获取weak_ptr的计数器 
 */
template<typename T>
int ptr_counter<T>::get_weak_count() const
{
	return *weak_count;
}

/**
 * free_data: 仅释放计数器引用的对象，不释放计数器本身   
 */
template<typename T> 
void ptr_counter<T>::free_data()
{
	if(data)
	delete data;
	data=nullptr;   // 重置数据引用指针为空 
}

/**
 * ~ptr_count: 析构函数，同时析构计数器本身和所有对象 
 */
template<typename T>	
ptr_counter<T>::~ptr_counter()
{
	if(data)
	delete data;
	delete shared_count;
	delete weak_count;
}

#endif 
