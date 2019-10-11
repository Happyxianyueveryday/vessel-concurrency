#ifndef SHARED_PTR_H 
#define SHARED_PTR_H
#include <string>

/**
 * shared_ptr: 智能指针类模板 
 */
template<class T> class shared_ptr
{
	public: 
	// 1. constructor
	shared_ptr();     			       // 默认构造函数
	
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
	
	private:
	// 5. domain
	T *data;                          // 智能指针所指向的对象的裸指针 
	int *count;                       // 对象的引用计数 
};

/**
 * shared_ptr: 初始化一个智能指针指向对应类型的新对象 
 */
template<typename T>
shared_ptr<T>::shared_ptr()
{
	data=new T();       // 初始化对象指针 
	count=new int(1);   // 初始化引用计数（注意必须使用计数的指针，原因很好理解，这里不再解析 
}
	
/** 
 * shared_ptr: 拷贝构造函数 
 * note: 拷贝构造函数首先将源指针的引用计数加1, 然后浅拷贝（重要）源指针的对象指针和引用计数指针。因为拷贝源指针所指向的对象的引用数增加了1 
 */
template<typename T>
shared_ptr<T>::shared_ptr(shared_ptr &other)
{
	// 1. 将源指针引用计数增加1 
	*(other.count)+=1;
	// 2. 浅拷贝源指针的对象指针和引用计数指针
	data=other.data; 
	count=other.count;
}

/**
 * operator=: 拷贝赋值运算符  
 * note: 因为赋值操作是将等号右侧的值赋值给左边，因此：
 *       1. 拷贝赋值运算符首先将源指针所指向对象的引用计数增加1，然后将当前指针所指向对象的引用计数减小1
 *	     2. 若减小1后等于0，则释放当前指针指向的对象
 *       3. 最后从源指针浅拷贝成员 
 */
template<typename T>
shared_ptr<T> &shared_ptr<T>::operator= (shared_ptr &other)  
{
	// 1. 判断是否为自身引用 
	if(&other==this)
	return (*this);
	// 2. 将源指针所指向对象的引用计数加1，将当前指针所指向对象的引用计数减1 
	*(other.count)+=1;
	*count-=1;
	// 3. 若减小1后当前指针指向对象引用计数为0，则析构所指向的对象 
	if(*count==0)
	{
		delete data;
		delete count;
	}
	// 4. 从源对象浅拷贝源对象指针和引用计数指针 
	data=other.data;
	count=other.count;
	// 5. 返回当前指针
	return (*this);
}

/**
 * shared_ptr: 移动构造函数 
 * note: 智能指针的移动语义比较特殊，移动构造函数需要进行的操作是: 首先浅拷贝源指针的所有成员，然后将源指针成员重置为默认值（默认构造函数中给成员的赋值）
 * note: 和拷贝构造函数相比，移动构造函数不需要增加源指针指向对象的引用计数，因为移动操作没有产生新的指向源指针对象的指针 
 */
template<typename T>
shared_ptr<T>::shared_ptr(shared_ptr &&other)
{
	// 1. 浅拷贝源指针的成员 
	data=other.data;
	count=other.count;
	// 2. 重置源指针的成员为默认值
	other.data=new T();
	other.count=new int(1);
}

/**
 * operator=: 移动赋值运算符
 * note: 移动赋值运算符的移动语义比较特殊，其需要进行的操作是：
 *       1. 首先将当前指针所指向对象的引用计数减1，若减1后引用计数为0则对对象进行析构
 *       2. 然后浅拷贝源指针的所有成员
 *       3. 最后将源指针所有成员重置为默认值 
 * note: 和拷贝赋值运算符相比，移动赋值运算符减少了当前指针的引用计数，但是不增加源指针的引用计数 
 */
template<typename T>
shared_ptr<T> &shared_ptr<T>::operator= (shared_ptr && other)
{
	// 1. 判断是否为自身赋值
	if(&other==this)
	return (*this);
	// 2. 将当前指针指向对象的引用值减小1，如果减小后值为0，则释放当前资源 
	(*count)-=1;
	if(*count==0)
	{
		delete data;
		delete count;
	}
	// 3. 浅拷贝源指针的所有成员 
	data=other.data;
	count=other.count;
	// 4. 重置源指针的成员为初始值
	other.data=new T();
	other.count=new int(1);
	// 5. 返回当前指针 
	return (*this);
}

/**
 * operator*: 解引用运算符
 * note: 解引用运算符直接返回当前对象的引用即可 
 */ 
template<typename T>
T &shared_ptr<T>::operator* () const
{
	return *data;
}

/**
 * operator->: 箭头运算符 
 * note: 箭头运算符直接返回当前指针所指向的对象的指针即可 
 */ 
template<typename T>
T *shared_ptr<T>::operator-> () const
{
	return data;
}

/**
 * unique: 判断当前指针指向对象的引用计数是否等于1 
 */
template<typename T>
bool shared_ptr<T>::unique() const
{
	if(*count==1)
	return true;
	else
	return false;
}

/**
 * ~shared_ptr: 析构函数    
 * note: 析构函数首先将引用计数减小1，若减小后值为0则释放对象，否则不释放对象 
 */
template<typename T>
shared_ptr<T>::~shared_ptr()
{
	*count-=1;
	if(*count==0)
	{
		delete data;
		delete count;
	}
}

#endif
