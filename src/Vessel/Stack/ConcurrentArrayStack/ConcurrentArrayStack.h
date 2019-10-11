#ifndef CONCURRENT_ARRAY_LIST_H                
#define CONCURRENT_ARRAY_LIST_H   
#include "ConcurrentStack.h"  
#include "rwmutex.h"
#include "wrmutex.h"
#include "fair_rwmutex.h"
#include <iostream>
#include <string>   
#include <mutex>    

/**
 * ConcurrentArrayStack: 线程安全的数组栈（读写锁版本） 
 */
template<typename T>
class ConcurrentArrayStack: public ConcurrentStack<T>
{
    public:   
    // 1. public constructor
    ConcurrentArrayStack(int _max_size, std::string mode);

    // 2. copy/move_controller
    ConcurrentArrayStack(const ConcurrentArrayStack<T> &other);                           // 拷贝构造函数 
    ConcurrentArrayStack<T> &operator= (const ConcurrentArrayStack<T> &other) = delete;   // 禁用拷贝赋值运算符 
    ConcurrentArrayStack(ConcurrentArrayStack<T> &&other);                                // 移动构造函数 
    ConcurrentArrayStack<T> &operator= (ConcurrentArrayStack<T> &&other) = delete;        // 禁用移动赋值运算符 

    // 3. methods
    bool push(T val);       // 入栈操作，该操作是线程安全的 
    bool pop(T &val);		// 出栈操作，该操作是线程安全的 
    bool top(T &val);       // 获取栈顶元素操作，该操作线程安全，但是仅有在取得的瞬间是有效的，过一段时间后可能栈被修改，这时取得的栈顶元素就不是此刻的栈顶元素了。因此必须将取栈顶操作和使用栈顶元素操作合并为一个原子操作 
    int length();           // 计算栈中的元素数量，该操作线程安全，但是仅有在取得的瞬间是有效的，因此必须将计算栈元素数量操作和使用元素数量的代码合并为一个原子操作  

    // 4. destructor
    ~ConcurrentArrayStack();
    
    private:
    int size;         // 当前栈中元素个数
    int max_size;     // 当前栈的最大可容纳元素个数
    T *data;          // 栈数组存储
    int pos;          // 栈顶指针位置
    std::string mode;      // 使用的读写锁模式 
    rwmutex rwmut;    // 读优先的读写锁 
    wrmutex wrmut;    // 写优先的读写锁 
    fair_rwmutex fair_rwmut;  // 读写公平的读写锁 
    
    // 5. private methods
    void set_lock(std::string lockmode);
};

template<typename T>
ConcurrentArrayStack<T>::ConcurrentArrayStack(int _max_size, std::string _mode)
{   
    size=0;  // 构造函数中当前对象的成员尚未被初始化，因此无需加锁当前对象自身 
    max_size=(_max_size>=0)?_max_size:10;
    data=new T [max_size];
    pos=-1;
    if(_mode=="read-first"||_mode=="write-first"||_mode=="fair")
    mode=_mode;
    else
    mode="read-first";
}

template<typename T>
ConcurrentArrayStack<T>::ConcurrentArrayStack(const ConcurrentArrayStack<T> &other)
{      
	other.set_lock("write_lock"); // 拷贝源写操作加锁，拷贝构造函数中当前对象的成员尚未被初始化，因此无需加锁当前对象自身  
	
    size=other.size;
    max_size=other.max_size;
    data=new T [max_size];
    pos=other.pos;
    mode=other.mode;

    for(int i=0;i<pos;i++)
    {
        data[i]=other.data[i];
    }
    
    other.set_lock("write_unlock");  // 拷贝源写操作解锁 
}

template<typename T>
ConcurrentArrayStack<T>::ConcurrentArrayStack(ConcurrentArrayStack &&other)
{	
	other.set_lock("write_lock"); // 拷贝源写操作加锁 
	
	size=other.size;
    max_size=other.max_size;
    data=other.data;
    pos=other.pos;
    
    other.size=0;
    other.data=new T [other.max_size];
    other.pos=-1;
    
    other.set_lock("write_unlock"); // 拷贝源写操作解锁 
}

template<typename T>
bool ConcurrentArrayStack<T>::push(T val)  
{
	set_lock("write_lock");        // 写操作加锁 
	
    if(size>=max_size)    // 这个判断条件若不加锁，则可能判断完毕后，其他线程又进行了入栈/出栈操作，得到的结果则不准确 
    {
    	set_lock("write_unlock");  // 写操作解锁 
    	return false;
	}
    else                  
    { 
        pos+=1;
        data[pos]=val;
        size+=1;
        set_lock("write_unlock");  // 写操作解锁 
        return true;
    }
}

template<typename T>
bool ConcurrentArrayStack<T>::pop(T &val)
{
	set_lock("write_lock");       // 写操作加锁
	
    if(size==0)  	
	{
		set_lock("write_unlock");       // 写操作解锁 
		return false;
	}	
    else               
    { 
        val=data[pos];
        pos-=1;
        size-=1;
        set_lock("write_unlock");       // 写操作解锁 
        return true;
    }
}

template<typename T>
bool ConcurrentArrayStack<T>::top(T &val)
{
	set_lock("read_lock");    // 读操作加锁  
    if(size==0) 
    {
    	set_lock("read_unlock");  // 读操作解锁 
    	return false;
	}
    else
    { 
    	val=data[pos];
    	set_lock("read_unlock");  // 读操作解锁 
    	return true;
 	}
}

template<typename T>
int ConcurrentArrayStack<T>::length()
{
	set_lock("read_lock");    // 读操作加锁  
	int res=size;
	set_lock("read_unlock");  // 读操作解锁 
    return res;
}

template<typename T>
void ConcurrentArrayStack<T>::set_lock(std::string lockmode)
{
	if(lockmode=="read_lock")
	{
		if(mode=="read-first")
		rwmut.read_lock();
		else if(mode=="write-first")
		wrmut.read_lock();
		else 
		fair_rwmut.read_lock();
	}
	else if(lockmode=="read_unlock")
	{
		if(mode=="read-first")
		rwmut.read_unlock();
		else if(mode=="write-first")
		wrmut.read_unlock();
		else 
		fair_rwmut.read_unlock();
	}
	else if(lockmode=="write_lock")
	{
		if(mode=="read-first")
		rwmut.write_lock();
		else if(mode=="write-first")
		wrmut.write_lock();
		else 
		fair_rwmut.write_lock();
	}
	else if(lockmode=="write_unlock")
	{
		if(mode=="read-first")
		rwmut.write_unlock();
		else if(mode=="write-first")
		wrmut.write_unlock();
		else 
		fair_rwmut.write_unlock();
	}
}

template<typename T>
ConcurrentArrayStack<T>::~ConcurrentArrayStack()
{
	set_lock("write_lock");
    delete data;
    set_lock("write_unlock");
}

#endif
