#ifndef CONCURRENT_QUEUE_H        
#define CONCURRENT_QUEUE_H         
 
/**
 * Queue: 队列的抽象基类
 * note 1: 注意，virtual仅仅指定虚函数，cpp的纯虚函数使用=0的记号来表明，纯虚函数和虚函数的概念要区分清楚 
 * note 2: 只要含有一个纯虚函数，则该类就是抽象基类，抽象基类和普通基类一致，都必须存在虚析构函数，为了动态绑定的需要 
 */
template<typename T>
class ConcurrentQueue 
{
	public:
    virtual bool push(T val) = 0;            // 队列入队操作            
    virtual bool pop(T &val) = 0;            // 队列出队操作
    virtual bool frontval(T &val) = 0;       // 获取队首元素
    virtual int length() = 0;                // 计算队列长度，即队列中的元素个数  
    virtual ~ConcurrentQueue() {};           // 纯虚析构函数，注意任何的基类都必须存在虚析构函数，抽象基类同理   
};

#endif 
