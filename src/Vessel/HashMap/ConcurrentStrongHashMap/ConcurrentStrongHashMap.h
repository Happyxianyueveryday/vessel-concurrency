#ifndef CONCURRENT_STRONG_HASHMAP_H        
#define CONCURRENT_STRONG_HASHMAP_H   
#include "ConcurrentMap.h"
#include "Segment.h" 
#include <string>
#include <vector>
#include <mutex> 

/**
 * ConcurrentStrongHashMap: 强一致性并发哈希表 
 * note:  
 */
template<typename T, typename R>
class ConcurrentStrongHashMap:public ConcurrentMap<T, R>       
{
	public:
	// 1. constructor
	ConcurrentStrongHashMap(unsigned int _curr_level=16);
	ConcurrentStrongHashMap(const std::vector<T> &_keys, const std::vector<R> &_values, unsigned int _curr_level=16);
	
	// 2. copy/move_controller 
    ConcurrentStrongHashMap(const ConcurrentStrongHashMap &other) = delete;                   // 禁用拷贝构造函数
    ConcurrentStrongHashMap &operator= (const ConcurrentStrongHashMap &other) = delete;       // 禁用拷贝赋值运算符
    ConcurrentStrongHashMap(ConcurrentStrongHashMap &&other) = delete;                        // 禁用移动构造函数
	ConcurrentStrongHashMap &operator= (ConcurrentStrongHashMap &&other) = delete;            // 禁用移动赋值运算符

    // 3. methods
    void insert(T key, R value) override;   	 // 向哈希表中插入指定的键值对，若哈希表中已经存在该键，则更新该键对应的值为新值 
	void remove(T key) override;            	 // 从哈希表中移除指定键的键值对 
	bool contain(T key) override;           	 // 判断哈希表中是否包含指定键   
	bool get(T key, R &value) override;    		 // 查找哈希表中给定键对应的值  
	std::vector<T> keys() override;              // 获得哈希表中所有键的列表  
	int length() override;                 		 // 获取哈希表的长度 
	std::string toString() override;             // 输出哈希表
	R &operator[] (T key);                       // 重载的下标运算符 
	~ConcurrentStrongHashMap();                  // 析构函数 
    
    private:
    // 4. domains
    Segment<T, R> **segments;                   // Segment段数组 
    const unsigned int capacity;                // Segment段的数量（固定为16） 
    unsigned int curr_level;                    // 写时并发等级：实际上就是Segment的个数，Segment的个数等于最多的并发写线程个数 
    
    // 5. private functions
    unsigned int jenkins_prehash(unsigned int code) const;     // 预处理哈希算法：Wang/Jenkins hash算法
    unsigned int first_hash(unsigned int code) const;          // 第一次哈希：确定需要加入的Segment位置下标 
};

/**
 * ConcurrentHashMap: 构造函数
 * param _curr_level: 并发程度，强一致性哈希表最多支持_curr_level个并发的读/写线程进行并发读写 
 */
template<typename T, typename R>
ConcurrentStrongHashMap<T, R>::ConcurrentStrongHashMap(unsigned int _curr_level):capacity((_curr_level>0)?_curr_level:16)
{
	segments=new Segment<T, R>* [capacity];
	
	for(int i=0;i<capacity;i++)
	{
		segments[i]=nullptr;
	}
}

/**
 * ConcurrentStrongHashMap: 构造函数
 * param _keys: 键列表
 * param _values: 与键列表相匹配的值列表
 * param _curr_level: 并发程度，强一致性哈希表最多支持_curr_level个并发的读/写线程进行并发读写 
 */
template<typename T, typename R>
ConcurrentStrongHashMap<T, R>::ConcurrentStrongHashMap(const std::vector<T> &_keys, const std::vector<R> &_values, unsigned int _curr_level):capacity((_curr_level>0)?_curr_level:16)
{
	segments=new Segment<T, R>* [capacity];
	for(int i=0;i<capacity;i++)
	{
		segments[i]=nullptr;
	}
	for(int i=0;i<_keys.size();i++)
	{
		this->insert(_keys[i], _values[i]);
	}
}

/** 
 * insert: 向哈希表中插入新的键值对，或者更新已经存在的键值对中的值
 * note: 在实现上只需要首先通过第一段哈希定位需要读写的Segment，然后将插入任务委托给Segment的插入方法即可 
 */
template<typename T, typename R>
void ConcurrentStrongHashMap<T, R>::insert(T key, R value)
{
	int index=first_hash(key.hashCode());
	if(!segments[index])
	segments[index]=new Segment<T, R>();
	segments[index]->insert(key, value);
}

/** 
 * remove: 从哈希表中删除指定键的键值对
 * note: 在实现上，首先通过第一段哈希定位待删除键所在的Segment，然后将插入任务委托给Segment的插入方法即可 
 */
template<typename T, typename R>
void ConcurrentStrongHashMap<T, R>::remove(T key)
{
	int index=first_hash(key.hashCode());
	if(!segments[index])
	return;
	else
	segments[index]->remove(key);
}

/**
 * contain: 判断某个键是否在哈希表内
 * return: 若给定的键在哈希表内则返回true，否则返回false 
 */
template<typename T, typename R>
bool ConcurrentStrongHashMap<T, R>::contain(T key)
{
	int index=first_hash(key.hashCode());
	if(segments[index])
	return segments[index]->contain(key);
	else
	return false;
}

/**
 * get: 获得给定键对应的值（安全版）
 * note: 若哈希表中存在给定的键，则返回true，值通过输入参数value返回；否则，返回false 
 */
template<typename T, typename R>
bool ConcurrentStrongHashMap<T, R>::get(T key, R &value)
{
	int index=first_hash(key.hashCode());
	if(segments[index])
	return segments[index]->get(key, value);
	else
	return false;
} 

/**
 * operator[]: 获得给定键对应的值（不安全版）
 * note: 若哈希表中存在给定的键，则返回对应的值；否则，行为是未定义的 
 */
template<typename T, typename R>
R &ConcurrentStrongHashMap<T, R>::operator[] (T key)
{
	int index=first_hash(key.hashCode());
	if(segments[index])
	return (*segments[index])[key];
}

/**
 * keys: 获取哈希表的键的集合 
 * note 1: 获取哈希表的键的集合需要遍历所有的Segment，因此属于一个协同操作
 * note 2: 对于协同操作，这里使用了和jdk1.7相似的策略：
 *         （乐观锁阶段） 
 *         1. 不加锁遍历所有的Segment，调用每个Segment的keys方法(Segment的keys方法无锁），收集键列表并计算各个Segment的mod_count之和
 *         2. 再次计算mod_count之和
 *         3. 若第二次计算的mod_count之和大于第一次，则重新执行步骤1，2；最多重新执行5次步骤1，2后转向步骤4 
 *         （悲观锁阶段） 
 *         4. 依次加锁并重新收集 
 */
template<typename T, typename R>
std::vector<T> ConcurrentStrongHashMap<T, R>::keys()
{
	// 乐观锁阶段 
	for(int i=0;i<6;i++)    // 乐观锁阶段最多执行6次 
	{
		std::vector<T> res;
		int first_mod_sum=0;
		for(int k=0;k<capacity;k++)
		{
			if(segments[k])
			{
				std::vector<T> temp=segments[k]->keys();
				first_mod_sum+=segments[k]->get_mod_count();
				for(int m=0;m<temp.size();m++)
				{
					res.push_back(temp[m]);
				}
			}
		}
		
		int second_mod_sum=0;
		for(int k=0;k<capacity;k++)
		{
			if(segments[k])
			second_mod_sum+=segments[k]->get_mod_count();
		}
		
		if(first_mod_sum==second_mod_sum)
		return res;
	}
	// 悲观锁阶段
	std::vector<T> res; 
	for(int k=0;k<capacity;k++)
	{
		if(segments[k])
		{
			segments[k]->mut.lock();
			std::vector<T> temp=segments[k]->keys();
			for(int m=0;m<temp.size();m++)
			{
				res.push_back(temp[m]);
			}
		}
	}
	for(int k=0;k<capacity;k++)
	{
		segments[k]->mut.unlock();
	}
	return res;
}

/* 
 * length: 统计哈希表的元素数量
 * note: 该方法同样是一个协同方法，算法同上的keys方法 
 */ 
template<typename T, typename R>
int ConcurrentStrongHashMap<T, R>::length() 
{
	// 乐观锁阶段 
	for(int i=0;i<6;i++)    // 乐观锁阶段最多执行6次 
	{
		int size=0;
		int first_mod_sum=0;
		for(int k=0;k<capacity;k++)
		{
			if(segments[k])
			{
				size+=segments[k]->length();
				first_mod_sum+=segments[k]->get_mod_count();
			}
		}
		
		int second_mod_sum=0;
		for(int k=0;k<capacity;k++)
		{
			if(segments[k])
			second_mod_sum+=segments[k]->get_mod_count();
		}
		
		if(first_mod_sum==second_mod_sum)
		return size;
	}
	// 悲观锁阶段
	int size;
	for(int k=0;k<capacity;k++)
	{
		if(segments[k])
		{
			segments[k]->mut.lock();
			size+=segments[k]->length();
		}
	}
	for(int k=0;k<capacity;k++)
	{
		segments[k]->mut.unlock();
	}
	return size;
}

/**
 * toString: 输出哈希表
 * note: 该方法同样是一个协同方法，算法同上的keys方法 
 */ 
template<typename T, typename R>
std::string ConcurrentStrongHashMap<T, R>::toString()
{
	// 乐观锁阶段 
	for(int i=0;i<6;i++)    // 乐观锁阶段最多执行6次 
	{
		std::string res;
		int first_mod_sum=0;
		for(int k=0;k<capacity;k++)
		{
			if(segments[k])
			{
				res=res+segments[k]->toString()+"\n";
				first_mod_sum+=segments[k]->get_mod_count();
			}
		}
		
		int second_mod_sum=0;
		for(int k=0;k<capacity;k++)
		{
			if(segments[k])
			second_mod_sum+=segments[k]->get_mod_count();
		}
		
		if(first_mod_sum==second_mod_sum)
		return res;
	}
	// 悲观锁阶段
	std::string res;
	for(int k=0;k<capacity;k++)
	{
		if(segments[k])
		{
			segments[k]->mut.lock();
			res=res+segments[k]->toString()+"\n";
		}
	}
	for(int k=0;k<capacity;k++)
	{
		segments[k]->mut.unlock();
	}
	return res;
}

/** 
 * first_hash: 第一次哈希（一段哈希） 
 * note 1: 当Segment的数量即capacity的值为2的幂时，始终有：code%capacity==code&(capacity)，从而减少了运算量 
 * note 2: 第一次哈希的目标是确定要操作的Segment的位置下标 
 * note 3: 两次哈希的算法不一样，第一次哈希的过程较为特殊，但是两次哈希之前都推荐做jenkins预处理 
 */ 
template<typename T, typename R>
unsigned int ConcurrentStrongHashMap<T, R>::first_hash(unsigned int code) const
{
	return code&(capacity-1);
}

/**
 * jenkins_prehash: 哈希预处理
 * note: 两次哈希之前都需要进行哈希预处理，预处理的目的是充分混杂，从而优化哈希的效果 
 */
template<typename T, typename R>
unsigned int ConcurrentStrongHashMap<T, R>::jenkins_prehash(unsigned int h) const
{
	h += (h << 15) ^ 0xffffcd7d; 
	h ^= (h >> 10);
	h += (h << 3); h ^= (h >> 6);
	h += (h << 2) + (h << 14); 
	return h ^ (h >> 16);
}

/**
 * ~ConcurrentStrongHashMap: 析构函数 
 */ 
template<typename T, typename R>  
ConcurrentStrongHashMap<T, R>::~ConcurrentStrongHashMap()
{
	for(int i=0;i<capacity;i++) 
	{
		if(segments[i])
		delete segments[i];
	} 
	delete [] segments;
}


#endif
