#ifndef CONCURRENT_MAP_H     
#define CONCURRENT_MAP_H 
#include <string> 
#include <vector>         

/** 
 * ConcurrentMap: 哈希表接口抽象基类  
 * typename T: 哈希表键的类型
 * typename R: 哈希表值的类型 
 */
template <typename T, typename R>
class ConcurrentMap
{    
	public:    
	virtual void insert(T key, R value) = 0;        // 向哈希表中插入指定的键值对，若哈希表中已经存在该键，则更新该键对应的值为新值 
	virtual void remove(T key) = 0;                 // 从哈希表中移除指定键的键值对 
	virtual bool contain(T key) = 0;          // 判断哈希表中是否包含指定键   
	virtual bool get(T key, R &value) = 0;    // 查找哈希表中给定键对应的值  
	virtual std::vector<T> keys() = 0;             // 获得哈希表中所有键的列表  
	virtual int length() = 0;                 // 获取哈希表的长度 
	virtual std::string toString() = 0;            // 输出哈希表    
	virtual ~ConcurrentMap() {}                     // 哈希表抽象基类虚析构函数
};

#endif
