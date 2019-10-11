#ifndef CONCURRENTSET_H                 
#define CONCURRENTSET_H         
#include <string>        

template<typename T> 
class ConcurrentSet
{
	public:
	virtual bool contain(const T &val) = 0;     // 判断给定元素是否在集合中   
    virtual bool add(const T &val) = 0;         // 向集合中增加指定元素 
    virtual bool remove(const T &val) = 0;      // 链表指定位置删除元素
    virtual int length() = 0;                   // 获取集合的大小 
    virtual std::string toString() = 0;              // 输出当前集合 
	virtual ~ConcurrentSet() {};                // 虚析构函数  
};

#endif 
