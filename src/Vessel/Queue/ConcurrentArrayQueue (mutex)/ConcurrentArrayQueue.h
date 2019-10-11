#define CONCURRENT_ARRAY_QUEUE_H               
#define CONCURRENT_ARRAY_QUEUE_H       
#include "ConcurrentQueue.h"  
#include <string>
#include <mutex>

/**
 * ConcurrentArrayQueue: 基于互斥锁的线程安全数组队列
 * note: 因为队列的底层实现基于循环数组，且使用了空出一个数组单元不使用的策略来区分队列满还是队列空，因此 
 */
template<typename T>
class ConcurrentArrayQueue: public ConcurrentQueue<T>
{
    public:   
    // 1. public constructor 
    static ConcurrentArrayQueue<T> *ArrayQueueFactory(int max_size);    // 构造函数

    // 2. copy_controller
    ConcurrentArrayQueue(const ConcurrentArrayQueue<T> &other);                             // 拷贝构造函数
    ConcurrentArrayQueue<T> & operator = (const ConcurrentArrayQueue<T> &other) = delete;   // 拷贝赋值运算符
    ConcurrentArrayQueue(ConcurrentArrayQueue<T> &&other);                        		    // 移动构造函数
	ConcurrentArrayQueue<T> &operator= (ConcurrentArrayQueue<T> &&other) = delete;          // 移动赋值运算符 

    // 3. methods
    bool push(T val) override;            // 队列入队操作
    bool pop(T &val) override;            // 队列出队操作
    bool frontval(T &val) override;       // 获取队首元素
    int length() override;                // 计算队列长度，即队列中的元素个数

    // 4. destructor
    ~ConcurrentArrayQueue();

	private:
    int max_size;    // 当前队列存储数组的大小
    T *data;         // 栈数组存储
    // 判断队列是否为空的方法：这里使用环形数组来存储队列元素，并且放弃其中一个单元不用来区分队空和队满两种特殊情况，因此当front==end表明队列为空，当(end+1)%max_size==begin时表明队列为满
    int front;       // 队首元素下标
    int end;         // 队尾元素的尾后元素的下标
    std::mutex mut;       // 并发控制 
    
    // 5. private constructor                     
    ConcurrentArrayQueue(int _max_size); 
};

template<typename T>
ConcurrentArrayQueue<T> *ConcurrentArrayQueue<T>::ArrayQueueFactory(int _max_size) 
{
    if(_max_size<0)
    return NULL;
    else
    return new ConcurrentArrayQueue<T>(_max_size+1);    // 创建能够存放_max_size个元素的队列，对应的存储数组长度需要是_max_size+1
}

template<typename T>
ConcurrentArrayQueue<T>::ConcurrentArrayQueue(int _max_size)
{ 
    max_size=_max_size;
    data=new T [max_size];
    front=0;   // 初始的队首元素下标为0
    end=0;     // 初始的队尾元素的尾后元素下标为0
}

template<typename T>
ConcurrentArrayQueue<T>::ConcurrentArrayQueue(const ConcurrentArrayQueue<T> &other)
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
ConcurrentArrayQueue<T>::ConcurrentArrayQueue(ConcurrentArrayQueue<T> &&other)
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
bool ConcurrentArrayQueue<T>::push(T val) 
{
	std::lock_guard<std::mutex> guard(mut);
    if((end+1)%max_size==front)    // 入队前需要首先判断队列是否已满，front指向队首，end指向队尾的尾后元素，因此end的下一个元素就是front时，即(end+1)%max_size==front时队满
    return false;
    else
    {
        data[end]=val;
        end=(end+1)%max_size;
        return true;
    }
}

template<typename T>
bool ConcurrentArrayQueue<T>::pop(T &val)
{
	std::lock_guard<std::mutex> guard(mut);
    if(front==end)    // 出队前首先需要判断队列是否已空，当front==end时就表明队空
    return false;
    else
    {
        val=data[front];
        front=(front+1)%max_size;
        return true;
    }
}

template<typename T>
bool ConcurrentArrayQueue<T>::frontval(T &val) 
{
	std::lock_guard<std::mutex> guard(mut);
    if(front==end)   // 取队首元素的操作基本和出队操作相同
    return false;
    else
    {
        val=data[front];
        return true;
    }
}

template<typename T>
int ConcurrentArrayQueue<T>::length()
{
	std::lock_guard<std::mutex> guard(mut);      
    // 计算队列中元素个数的标准公式就是：(end-front+max_size)%max_size，举几个常见例子可以推导得到 
    return (end-front+max_size)%max_size;
}

template<typename T>
ConcurrentArrayQueue<T>::~ConcurrentArrayQueue()
{
    delete [] data;
}
