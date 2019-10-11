#define ARRAY_BLOCKING_QUEUE_H             
#define ARRAY_BLOCKING_QUEUE_H       
#include "BlockingQueue.h"
#include <iostream>  
#include <string>
#include <mutex>
#include <condition_variable>

/**
 * ArrayBlockingQueue: 基于互斥锁的线程安全数组队列
 * note: 因为队列的底层实现基于循环数组，且使用了空出一个数组单元不使用的策略来区分队列满还是队列空，因此 
 */
template<typename T>
class ArrayBlockingQueue: public BlockingQueue<T>
{
    public:   
    // 1. public constructor 
    static ArrayBlockingQueue<T> *ArrayQueueFactory(int max_size);    // 构造函数

    // 2. copy_controller
    ArrayBlockingQueue(const ArrayBlockingQueue<T> &other);                             // 拷贝构造函数
    ArrayBlockingQueue<T> & operator = (const ArrayBlockingQueue<T> &other) = delete;   // 拷贝赋值运算符
    ArrayBlockingQueue(ArrayBlockingQueue<T> &&other);                        		    // 移动构造函数
	ArrayBlockingQueue<T> &operator= (ArrayBlockingQueue<T> &&other) = delete;          // 移动赋值运算符  

    // 3. methods
    void push(T val) override;            // 队列入队操作
    void pop(T &val) override;            // 队列出队操作
    void frontval(T &val) override;       // 获取队首元素
    int length() override;                // 计算队列长度，即队列中的元素个数
    bool full() override;                 // 判断队列是否为满 
    bool empty() override;                // 判断队列是否为空 

    // 4. destructor
    ~ArrayBlockingQueue();

	private:
    int max_size;    // 当前队列存储数组的大小
    T *data;         // 栈数组存储
    // 判断队列是否为空的方法：这里使用环形数组来存储队列元素，并且放弃其中一个单元不用来区分队空和队满两种特殊情况，因此当front==end表明队列为空，当(end+1)%max_size==begin时表明队列为满
    int front;       // 队首元素下标
    int end;         // 队尾元素的尾后元素的下标
    
    std::recursive_mutex mut;       // 用于同步控制和并发的互斥量 
    std::condition_variable_any push_var;    // 入队线程的条件变量 
    std::condition_variable_any pop_var;     // 出队线程的条件变量 
    
    // 5. private constructor                     
    ArrayBlockingQueue(int _max_size); 
};

template<typename T>
ArrayBlockingQueue<T> *ArrayBlockingQueue<T>::ArrayQueueFactory(int _max_size) 
{
    if(_max_size<0)
    return NULL;
    else
    return new ArrayBlockingQueue<T>(_max_size+1);    // 创建能够存放_max_size个元素的队列，对应的存储数组长度需要是_max_size+1
}

template<typename T>
ArrayBlockingQueue<T>::ArrayBlockingQueue(int _max_size)
{ 
    max_size=_max_size;
    data=new T [max_size];
    front=0;   // 初始的队首元素下标为0
    end=0;     // 初始的队尾元素的尾后元素下标为0
}

template<typename T>
ArrayBlockingQueue<T>::ArrayBlockingQueue(const ArrayBlockingQueue<T> &other)
{
	std::lock_guard<std::mutex> other_guard(other.mut);
    max_size=other.max_size;
    data=new T [max_size];
    front=other.front;
    end=other.end;

    for(int i=0;i<max_size;i++)
    {
        data[i]=other.data[i];
    }
}

template<typename T>
ArrayBlockingQueue<T>::ArrayBlockingQueue(ArrayBlockingQueue<T> &&other)
{
	std::lock_guard<std::mutex> other_guard(other.mut);
	max_size=other.max_size;
    data=other.data;
    front=other.front;
    end=other.end;

    other.data=new T [max_size];
    other.front=0;   // 初始的队首元素下标为0
    other.end=0;     // 初始的队尾元素的尾后元素下标为0
}

template<typename T>
void ArrayBlockingQueue<T>::push(T val) 
{
	
    std::unique_lock<std::recursive_mutex> ulock(mut);
    push_var.wait(ulock, [this]() {return !full();});   // 若队列已满，则解锁ulock，进行让权等待 
    data[end]=val; 
    end=(end+1)%max_size; 
    pop_var.notify_one();    // 入队结束后，通知正在等待出队的线程进行出队 
}

template<typename T>
void ArrayBlockingQueue<T>::pop(T &val)
{
	std::unique_lock<std::recursive_mutex> ulock(mut);
	pop_var.wait(ulock, [this]() {return !empty();});
    val=data[front];
    front=(front+1)%max_size;
    push_var.notify_one();
}

template<typename T>
void ArrayBlockingQueue<T>::frontval(T &val) 
{
	std::unique_lock<std::recursive_mutex> ulock(mut);
	pop_var.wait(ulock, [this]() {return !empty();});
    val=data[front];
}

template<typename T>
int ArrayBlockingQueue<T>::length()
{
	std::lock_guard<std::recursive_mutex> guard(mut);      
    // 计算队列中元素个数的标准公式就是：(end-front+max_size)%max_size，举几个常见例子可以推导得到 
    return (end-front+max_size)%max_size;
}

template<typename T>
bool ArrayBlockingQueue<T>::full() 
{
	//front指向队首，end指向队尾的尾后元素，因此end的下一个元素就是front时，即(end+1)%max_size==front时队满
	std::lock_guard<std::recursive_mutex> guard(mut); 
	if((end+1)%max_size==front)
	return true;
	else
	return false;
}

template<typename T>
bool ArrayBlockingQueue<T>::empty()
{
	// 若队列尾后元素下标和队首元素下标相等，则队列已经空 
	std::lock_guard<std::recursive_mutex> guard(mut);
	if(front==end) 
	return true;
	else
	return false; 
}

template<typename T>
ArrayBlockingQueue<T>::~ArrayBlockingQueue()
{
    delete [] data;
}
