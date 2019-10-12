#ifndef WEAK_PTR_H  
#define WEAK_PTR_H
#include "shared_ptr.h"

template<typename T>
class weak_ptr
{
	public:
	// 1. constructor
	weak_ptr(shared_ptr<T> &ptr);    // 从shared_ptr构造一个指向相同对象的weak_ptr 
	
	// 2. copy/move controller
	weak_ptr(const weak_ptr<T> &other);                      // 拷贝构造函数 
	weak_ptr<T> &operator= (weak_ptr<T> &other);       		 // 拷贝赋值运算符 
	weak_ptr(weak_ptr<T> &&other) = delete;                  // 禁用移动构造函数 
	weak_ptr<T> &operator= (weak_ptr<T> &&other) = delete;   // 禁用移动赋值运算符 
	weak_ptr<T> &operator= (shared_ptr<T> &other);     		 // 普通赋值运算符 
	weak_ptr<T> &operator= (shared_ptr<T> &&other) = delete; // 禁用移动赋值运算符 
	
	// 3. methods
	void reset();    			// 将当前weak_ptr置为空
	int use_count() const;      // 返回所指向对象的shared_ptr数量
	shared_ptr<T> lock() const; // 安全返回所指向对象的shared_ptr指针 
	bool expired() const;       // 判断当前weak_ptr是否为空  
	
	// 4. destructor
	~weak_ptr();            // 析构函数 
	
	private:
	// 5. domain
	ptr_counter<T> *counter;// 当前weak_ptr所绑定的计数器对象（内部包含引用对象），注意weak_ptr中绝对不能保存shared_ptr的拷贝或者指针 
};

/**
 * weak_ptr: 构造函数，从shared_ptr构造一个weak_ptr 
 */
template<typename T>
weak_ptr<T>::weak_ptr(shared_ptr<T> &ptr)
{
	counter=ptr.counter;    // 指向所依赖的shared_ptr的计数器（内部包含引用对象）
	if(counter) 
	counter->weak_plus();   // 增加计数器的weak_ptr计数值
}

/**
 * weak_ptr: 拷贝构造函数 
 * note: 需要做的操作和shared_ptr类似，首先将源指针的weak_ptr计数值加1，然后浅拷贝对象即可
 */
template<typename T>
weak_ptr<T>::weak_ptr(const weak_ptr<T> &other)
{
	// 1. 将源指针的weak_ptr计数值增加1 
	if(other.counter)       // 源指针counter判空 
	other->counter->weak_plus();
	// 2. 浅拷贝对象 
	counter=other.counter;
}

/**
 * operator=: 拷贝赋值运算符 
 * note: 拷贝赋值运算符需要进行的工作如下：
 * 		 1. 将源指针的weak_ptr计数值增加1，将当前指针的weak_ptr计数值减少1 
 *       2. 若减少后当前指针的weak_ptr计数值和shared_ptr计数值均为0，则释放计数器对象 
 *       3. 从源指针浅拷贝计数器成员 
 */
template<typename T>
weak_ptr<T> &weak_ptr<T>::operator= (weak_ptr<T> &other)
{
	// 1. 判断是否为自身赋值
	if(&other==this)
	return (*this); 
	// 2. 将源指针的weak_ptr计数值增加1，将当前指针的weak_ptr计数值减少1
	if(other.counter)
	other.counter->weak_plus();
	if(counter)
	counter->weak_sub();
    // 3. 若减少后当前指针的weak_ptr计数值和shared_ptr计数值均为0，则释放计数器对象 
    if(counter&&counter->get_weak_count()==0&&counter->get_shared_count()==0)
    delete counter;
	// 4. 从源指针浅拷贝计数器成员 
	counter=other.counter;
	// 5. 返回当前指针对象
	return (*this); 
}

/**
 * operator=: 普通赋值运算符 
 * note: 普通赋值运算符用于修改当前weak_ptr所捆绑的shared_ptr，所需要进行的操作如下：
 * 		 1. 将源指针的weak_ptr计数值增加1，将当前指针的weak_ptr计数值减少1 
 *       2. 若减小后当前指针计数器的weak_ptr计数值和shared_ptr计数值均为0，则释放当前指针的计数器对象
 *       3. 浅拷贝源指针的计数器 
 */
template<typename T>
weak_ptr<T> &weak_ptr<T>::operator= (shared_ptr<T> &other)
{
	// 1. 判断是否为自身赋值（即当前weak_ptr绑定的shared_ptr指向对象和源指针相同）
	if(other.counter==counter)
	return (*this); 
	// 2. 将源指针的weak_ptr计数值增加1，将当前指针的weak_ptr计数值减少1 
	if(other.counter)    // 注意对源指针判空 
	other.counter->weak_plus();
	if(counter)
	counter->weak_sub();
	// 3. 若减小后当前指针计数器的weak_ptr计数值和shared_ptr计数值均为0，则释放当前指针的计数器对象
	if(counter&&counter->get_weak_count()==0&&counter->get_shared_count()==0)
    delete counter;
    // 4. 从源指针浅拷贝计数器成员 
	counter=other.counter;
	// 5. 返回当前指针对象
	return (*this);
}

/**
 * reset: 将weak_ptr置为空指针 
 * note: 首先将自身计数器的weak_ptr计数值减小1，减小后若当前指针计数器的weak_ptr计数值和shared_ptr计数值均为0，则释放当前指针的计数器对象 
 */	
template<typename T>
void weak_ptr<T>::reset()
{
	if(counter)
	counter->weak_sub();
	if(counter&&counter->get_weak_count()==0&&counter->get_shared_count()==0)
	delete counter;
	counter=nullptr;
}

/**
 * use_count: 返回当前weak_ptr所指向的对象的shared_ptr引用个数 
 */ 
template<typename T>
int weak_ptr<T>::use_count() const
{
	if(counter)
	return counter->get_shared_count();
	else
	return 0;
}

/**
 * lock: 安全获得当前weak_ptr所指向对象的shared_ptr 
 */
template<typename T>
shared_ptr<T> weak_ptr<T>::lock() const
{
	if(counter&&counter->get_shared_count())
	// return shared_ptr(counter->getdata()); 这种写法是危险的，绝不能使用一个裸指针创建多个shared_ptr 
	// 因此此处需要极其特殊的操作来创建裸指针的shared_ptr，即手动生成shared_ptr，并赋值合理的引用计数 
	{
		shared_ptr<T> res(nullptr);
		res.counter=counter;
		counter->shared_plus();  
		return res;  
	} 
	else
	return shared_ptr<T>(nullptr);
}

/**
 * expired: 判断当前weak_ptr是否有效 
 */
template<typename T>
bool weak_ptr<T>::expired() const
{
	if(counter&&counter->get_shared_count())
	return true;
	else
	return false;
}

template<typename T>
weak_ptr<T>::~weak_ptr()
{
	if(counter&&counter->get_shared_count()==0&&counter->get_weak_count()==0)
	delete counter;
}        

#endif
