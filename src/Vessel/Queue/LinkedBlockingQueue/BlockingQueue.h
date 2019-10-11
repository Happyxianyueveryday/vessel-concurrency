#ifndef BLOCKING_QUEUE_H    
#define BLOCKING_QUEUE_H                 

/**
 * BlockingQueue: 队列的抽象基类  
 */
template<typename T>
class BlockingQueue
{
	public:
    virtual void push(T val) = 0;              // 队列入队操作
    virtual void pop(T &val) = 0;              // 队列出队操作
    virtual void frontval(T &val) = 0;         // 获取队首元素
    virtual int length() = 0;                  // 计算队列长度，即队列中的元素个数
    virtual bool empty() = 0;                  // 判断队列是否为空 
    virtual bool full() = 0;                   // 判断队列是否为满 
    virtual ~BlockingQueue() {};               // 纯虚析构函数，注意任何的基类都必须存在虚析构函数，抽象基类同理 
};

#endif
