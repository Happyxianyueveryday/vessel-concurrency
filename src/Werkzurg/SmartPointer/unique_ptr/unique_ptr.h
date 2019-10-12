#ifndef UNIQUE_PTR_H 
#define UNIQUE_PTR_H
#include <string>   

template<typename T>
class unique_ptr
{
	public:
	// 1. constructor
	unique_ptr();            // 默认构造函数，创建指向nullptr的智能指针 
	unique_ptr(T *_data);    // 构造函数，从普通指针创建智能指针，之后资源的所有权转交给新建的智能指针对象 
	
	// 2. copy/move controller
	unique_ptr(const unique_ptr &other) = delete;       		// unique_ptr禁用拷贝构造函数 
	unique_ptr &operator= (const unique_ptr &other)=delete;     // unique_ptr禁用拷贝赋值运算符 
	unique_ptr(unique_ptr &&other);                     		// 移动构造函数 
	unique_ptr &operator= (unique_ptr &&other);         		// 移动赋值运算符 
	
	// 3. methods
	bool empty() const;      // 判断当前智能指针是否为空 
	T *release();            // 转移所有权，智能指针释放对当前对象的所有权，并返回对象指针 
    void reset(T *data);     // 释放所有权，智能指针获得新输入的对象的所有权，并销毁原先所有的对象
    void reset();            // 释放所有权，智能指针销毁原先所有的对象，并将自身所有权置为nullptr 
	T &operator* () const;   // 解引用运算符：解引用运算符应当始终返回当前对象的引用 
	T *operator-> () const;  // 箭头运算符：箭头运算符应当始终返回指向当前对象的指针，即this指针 
	
	// 4. destructor
    ~unique_ptr();           // 析构函数，通过销毁当前持有的对象，从而释放对当前所有的对象的所有权
	
	private:
	// 5. destructor
	T *data;                 // 当前智能指针所持有的对象指针 
};

/**
 * unique_ptr: 默认构造函数 
 * note: 默认构造函数创建一个不持有任何对象的unique_ptr 
 */
template<typename T>
unique_ptr<T>::unique_ptr()
{
	data=nullptr;
}

/** 
 * unique_ptr: 普通构造函数
 * note: 普通构造函数初始化一个持有输入指针指向对象的unique_ptr，之后输入的指针不再具有对该对象的所有权 
 */
template<typename T>
unique_ptr<T>::unique_ptr(T *_data)
{
	data=_data;
}

/**
 * unique_ptr: 移动构造函数
 * note: 移动构造函数将移动源指针所持有的对象的所有权转移到当前构造的新智能指针上
 */
template<typename T>
unique_ptr<T>::unique_ptr(unique_ptr &&other)
{
	// 1. 从源指针浅拷贝对象指针，转交所有权 
	data=other.data;
	// 2. 重置源指针的对象指针为默认值，释放所有权 
	other.data=nullptr;
}

/** 
 * operator=: 移动赋值运算符
 * note: 移动赋值运算符将移动源指针所持有的对象的所有权转移到当前指针上，同时释放当前指针所又来持有的资源 
 */
template<typename T>
unique_ptr<T> &unique_ptr<T>::operator= (unique_ptr &&other)
{
	// 1. 判断是否为自身赋值
	if(&other==this)
	return (*this);
	// 2. 判断当前指针是否持有对象，若持有对象则进行释放
	if(data)
	delete data; 
	// 3. 从源指针浅拷贝对象指针，转交所有权 
	data=other.data;
	// 4. 重置源指针的对象指针为默认值，释放所有权 
	other.data=nullptr;
	// 5. 返回当前智能指针
	return (*this);
}

/** 
 * empty: 判断当前智能指针是否持有对象/是否为空 
 */
template<typename T>
bool unique_ptr<T>::empty() const
{
	if(data)
	return false;
	else
	return true;
}

/**
 * release: 令指针释放所持有对象的所有权
 * return: 返回智能指针所持有对象的原始指针 
 */
template<typename T>	
T *unique_ptr<T>::release()
{
	T *temp=data;
	data=nullptr;
	return temp;
}

/**
 * reset: 重载函数，释放当前指针原先所持有的对象，然后持有输入指针指向的对象 
 */
template<typename T>
void unique_ptr<T>::reset(T *_data)
{
	// 1. 释放当前指针原来持有的对象 
	if(data)
	delete data;	
	// 2. 令当前指针持有输入指针指向的对象
	data=_data;	 
}

/**
 * reset: 重载函数，释放当前指针所持有的对象 
 */
template<typename T>
void unique_ptr<T>::reset()
{
	if(data)
	delete data;
	data=nullptr;
}

/**
 * operator*: 解引用运算符  
 */
template<typename T>
T &unique_ptr<T>::operator* () const
{
	return *data;
}

/** 
 * operator->: 箭头运算符 
 */
template<typename T>
T *unique_ptr<T>::operator-> () const
{
	return data;
}

/**
 * ~unique_ptr:  
 */
template<typename T>
unique_ptr<T>::~unique_ptr()
{
	if(data)
	delete data;
}

#endif
