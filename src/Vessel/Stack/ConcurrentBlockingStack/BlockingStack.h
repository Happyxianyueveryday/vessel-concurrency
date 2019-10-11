#ifndef BLOCKING_STACK_H           
#define BLOCKING_STACK_H 
 
/**
 * BlockingStack: 阻塞栈的抽象基类       
 */
template<typename T>           
class BlockingStack
{
	public:
	virtual void push(T val) = 0;    	   // 入栈操作
    virtual void pop(T &val) = 0;          // 出栈操作
    virtual void top(T &val) = 0;          // 获取栈顶元素操作
    virtual int length() = 0;              // 获取栈的元素个数
	virtual bool empty() = 0;              // 判断栈是否为空 
	virtual bool full() = 0;               // 判断栈是否为满 
    virtual ~BlockingStack()               // 虚析构函数 
	{
	}
};

#endif 
