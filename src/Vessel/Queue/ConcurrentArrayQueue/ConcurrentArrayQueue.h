#define CONCURRENT_ARRAY_QUEUE_H          
#define CONCURRENT_ARRAY_QUEUE_H      
#include "ConcurrentQueue.h" 
#include "rwmutex.h"
#include "wrmutex.h"
#include "fair_rwmutex.h"
#include <string> 

/**
 * ConcurrentArrayQueue: 数组栈 
 */
template<typename T>
class ConcurrentArrayQueue: public ConcurrentQueue<T>
{
    public:   
    // 1. public constructor 
    static ConcurrentArrayQueue<T> *ArrayQueueFactory(int max_size, std::string mode);    // 构造函数

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
    std::string mode;     // 设定的读写锁中的优先级策略 
    
    rwmutex rwmut;    // 读优先的读写锁 
    wrmutex wrmut;    // 写优先的读写锁 
    fair_rwmutex fair_rwmut;  // 读写公平的读写锁 
    
    // 5. private methods
    void set_lock(std::string lockmode);                 
    ConcurrentArrayQueue(int _max_size, std::string _mode); 
};

template<typename T>
ConcurrentArrayQueue<T> *ConcurrentArrayQueue<T>::ArrayQueueFactory(int _max_size, std::string _mode) 
{
    if(_max_size<0)
    return NULL;
    else
    return new ConcurrentArrayQueue<T>(_max_size+1, _mode);    // 创建能够存放_max_size个元素的队列，对应的存储数组长度需要是_max_size+1
}

template<typename T>
ConcurrentArrayQueue<T>::ConcurrentArrayQueue(int _max_size, std::string _mode)
{
    max_size=_max_size;
    data=new T [max_size];
    front=0;   // 初始的队首元素下标为0
    end=0;     // 初始的队尾元素的尾后元素下标为0
    if(_mode=="read-first"||_mode=="write-first"||_mode=="fair")
    mode=_mode;
    else
    mode="read-first";
}

template<typename T>
ConcurrentArrayQueue<T>::ConcurrentArrayQueue(const ConcurrentArrayQueue<T> &other)
{
	other.set_lock("write_lock");
    max_size=other.max_size;
    data=new T [max_size];
    front=other.front;
    end=other.end;
    mode=other.mode;

    for(int i=0;i<max_size;i++)
    {
        data[i]=other.data[i];
    }
    other.set_lock("write_unlock");
}

template<typename T>
ConcurrentArrayQueue<T>::ConcurrentArrayQueue(ConcurrentArrayQueue<T> &&other)
{
	other.set_lock("write_lock");
	max_size=other.max_size;
    data=other.data;
    front=other.front;
    end=other.end;
    mode=other.mode;

    other.data=new T [max_size];
    other.front=0;   // 初始的队首元素下标为0
    other.end=0;     // 初始的队尾元素的尾后元素下标为0 
    other.set_lock("write_unlock");
}

template<typename T>
bool ConcurrentArrayQueue<T>::push(T val) 
{
	set_lock("write_lock");
    if((end+1)%max_size==front)    // 入队前需要首先判断队列是否已满，front指向队首，end指向队尾的尾后元素，因此end的下一个元素就是front时，即(end+1)%max_size==front时队满
    {
    	set_lock("write_unlock");
    	return false;
	}
    else
    {
        data[end]=val;
        end=(end+1)%max_size;
        set_lock("write_unlock");
        return true;
    }
}

template<typename T>
bool ConcurrentArrayQueue<T>::pop(T &val)
{
	set_lock("write_lock");
    if(front==end)    // 出队前首先需要判断队列是否已空，当front==end时就表明队空
    {
    	set_lock("write_unlock");
    	return false;
	}
    else
    {
        val=data[front];
        front=(front+1)%max_size;
        set_lock("write_unlock");
        return true;
    }
}

template<typename T>
bool ConcurrentArrayQueue<T>::frontval(T &val) 
{
	set_lock("read_lock");
    if(front==end)   // 取队首元素的操作基本和出队操作相同
    {
    	set_lock("read_unlock");
    	return false;
    }
    else
    {
        val=data[front];
        set_lock("read_unlock");
        return true;
    }
}

template<typename T>
int ConcurrentArrayQueue<T>::length()
{
    // 计算队列中元素个数的标准公式就是：(end-front+max_size)%max_size，举几个常见例子可以推导得到 
    set_lock("read_lock");
    int res=(end-front+max_size)%max_size;
    set_lock("read_unlock");
    return res;
}

template<typename T>
void ConcurrentArrayQueue<T>::set_lock(std::string lockmode)
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
ConcurrentArrayQueue<T>::~ConcurrentArrayQueue()
{
	set_lock("write_lock");
    delete [] data;
    set_lock("write_unlock");
}
