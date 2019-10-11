#ifndef SEGMENT_H                                  
#define SEGMENT_H
#include "HashEntry.h" 
#include <iostream>   
#include <string>
#include <vector>
#include <mutex>  

/**
 * Segment: 拉链法实现的哈希集合 
 */ 
template<typename T>      
class Segment 
{
    public:
    // 1. constructor
    Segment();                                // 构造函数

    // 2. copy/move_controller
    Segment(Segment &other) = delete;                   // 禁用拷贝构造函数（深拷贝） 
    Segment &operator= (Segment &other) = delete;       // 禁用拷贝赋值运算符（深拷贝） 
    Segment(Segment &&other) = delete;                  // 禁用移动构造函数 （浅拷贝/移动）
	Segment &operator= (Segment &&other) = delete;      // 禁用移动赋值运算符（浅拷贝/移动） 

    // 3. methods
    void insert(T value);              // 向哈希表中插入指定值
    void remove(T value);              // 从哈希表中移除指定值
    bool contain(T value) const;             // 判断哈希表中是否包含指定值
    std::vector<T> values() const;                // 获得哈希表中所有值的列表
    int length() const;                      // 获取哈希表的元素个数 
	std::string toString() const;                 // 输出哈希表

    // 4. destructor
    ~Segment();                                         // 哈希表抽象基类虚析构函数

    private:
    // 5. domains
    HashEntry<T> **data;          // 哈希表数组结构
    unsigned int capacity;        // 哈希表桶的数量（即数组的长度），拉链法中，获取到元素的哈希值后将哈希值模桶数量，就可以得到具体的存放的桶的下标位置
    double load_factor;           // 哈希表负载因子
    unsigned int threshold;       // 哈希表扩容阈值，阈值=桶长度*负载因子，通常的拉链法哈希表的扩容策略是：在执行insert方法插入元素后，检查哈希表中的元素个数size是否大于阈值threshold，若大于，则调用resize方法进行扩容，具体的扩容方法请参见resize方法
    unsigned int size;            // 当前哈希表中的元素个数
    std::mutex mut;                    // 保证写互斥的互斥锁 

    // 6. private functions
    void resize();       // 哈希表扩容方法
    unsigned int jenkins_prehash(unsigned int h) const; 
    unsigned int second_hash(unsigned int code) const;      // 第二次哈希 
};

/**
 * Segment: 默认构造函数
 */
template<typename T>
Segment<T>::Segment()
{
	capacity=16;    
	load_factor=0.75;
	threshold=capacity*load_factor;
	size=0;
	data=new HashEntry<T>* [capacity];
	for(int i=0;i<capacity;i++)
	{
		data[i]=nullptr;
	}
}

/**
 * insert: 向哈希集合中插入元素值
 * note: 若插入的元素值在哈希集合中已经存在重复，则不会进行插入 
 */
template<typename T>
void Segment<T>::insert(T value) 
{
	std::lock_guard<std::mutex> guard(mut);    // 写操作加锁 
	int index=second_hash(value.hashCode());    // 第二次哈希：计算待插入元素应当插入的HashEntry下标 
	if(!data[index])
	data[index]=new HashEntry<T>();
	if(data[index]->insert(value))
	size+=1;
	if(size>threshold) 
	resize();
	return;
}
 
/**
 * remove: 从哈希集合中删除指定值的元素
 * note: 若哈希集合中不存在该值，则实际上不会进行删除 
 */
template<typename T>
void Segment<T>::remove(T value) 
{
	std::lock_guard<std::mutex> guard(mut);    // 写操作加锁 
	int index=second_hash(value.hashCode()); 
	if(data[index])
	{
		if(data[index]->remove(value))
		size-=1;
	}
	return;
}

/**
 * contain: 判断给定值是否在哈希表中
 * note: 若给定值在哈希表中则返回true，否则返回false
 */
template<typename T>
bool Segment<T>::contain(T value) const
{
	int index=second_hash(value.hashCode());
	if(!data[index])
	return false;
	else
	return data[index]->contain(value);
}

/**
 * values: 获取哈希集合中的所有值的列表
 * return: 哈希集合中的所有元素的列表 
 */
template<typename T>
std::vector<T> Segment<T>::values() const
{
	std::vector<T> res;
	for(int i=0;i<capacity;i++)
	{
		if(data[i])
		{
			std::vector<T> temp=data[i]->values();
			for(int k=0;k<temp.size();k++)
			{
				res.push_back(temp[k]); 
			}
		}
	}
	return res;
}

/**
 * length: 获取哈希表的元素个数 
 */
template<typename T>
int Segment<T>::length() const
{
	return size;    // 对size修改的操作均在互斥量保护之下，不存在数据竞争 
}

/**
 * toString: 生成哈希表的输出字符串 
 */
template<typename T>
std::string Segment<T>::toString() const
{
	std::string res;
	for(int i=0;i<capacity;i++)
	{
		if(data[i])
		res=res+data[i]->toString()+"\n";
	}
	return res; 
}

/**
 * resize: 哈希扩容方法
 * note 1: 何时发生哈希扩容？ 
 *         答：只要在执行插入操作后，哈希表中的元素数量size大于桶的数量capacity乘以一个负载因子系数(load_factor)时就发生哈希扩容，即发生条件为: size>capacity*load_factor 
 * note 2: 哈希扩容具体算法步骤： 
 *         答：1. 将桶的数量(capacity)扩容为原先的两倍，即: capacity=2*capacity
 *			   2. 重新计算阈值，即: threshoud=capacity*load_factor，其中load_factor为负载因子，固定为0.75 
 *			   3. 创建新的桶数组
 *			   4. 将旧桶数组中的链表按照与之前相同算法重新计算哈希值和插入位置，并使用浅拷贝移动到新桶数组，即: index=(element->hashCode())%capacity 
 *			   5. 释放旧桶数组，新的桶数组作为当前哈希表的桶数组  
 */
template<typename T>
void Segment<T>::resize()  
{
	// resize只在insert方法中被调用，调用时insert已经加锁，无需重复加锁 
	// 1. 创建新的桶数组，重新计算新的桶数量和扩容阈值 
	unsigned int new_capacity=capacity*2;                  // 新的桶数量 
	unsigned int new_threshold=new_capacity*load_factor;   // 新的扩容阈值 
	HashEntry<T> **new_data=new HashEntry<T>* [new_capacity]; // 新的桶数组指针 
	for(int i=0;i<new_capacity;i++)
	{
		new_data[i]=nullptr;
	}
	// 2. 将旧的桶数组中的元素拷贝到新的桶数组中，并释放旧的桶数组中的HashEntry 
	for(int i=0;i<capacity;i++)      
	{
		std::vector<T> lis;
		if(data[i])
		lis=data[i]->values();
		for(int k=0;k<lis.size();k++)
		{
			// 将旧桶数组中的元素插入到新的桶数组中 
			int index=jenkins_prehash(lis[k].hashCode())&(new_capacity-1);
			if(!new_data[index]) 
			new_data[index]=new HashEntry<T>(); 
			new_data[index]->insert(lis[k]); 
		}
		// 释放旧的桶数组中的HashEntry
		// 需要特别注意，因为此时可能存在读线程读data[i]，因此在delete之前必须进行置空 
		HashEntry<T> *temp=data[i];   
		data[i]=nullptr;   // 保证读安全 
		delete data[i];
	}
	// 3. 重置桶数组
	delete [] data;  
	data=new_data;             // 这里使用的重置顺序是首先重置data为new_data，然后修改capacity为new_capacity，这会带来一定的一致性问题，当data被更新但是capacity未更新时，可能会存在读线程在哈希时读不到哈希表中存在的数据 
	capacity=new_capacity;     
	threshold=new_threshold;
}  

/** 
 * second_hash: 第二次哈希（二段哈希） 
 * note 1: 当桶的数量即capacity的值为2的幂时，始终有：code%capacity==code&(capacity)，从而减少了运算量 
 * note 2: 第二次哈希的目标是确定要操作的桶的位置下标 
 * note 3: 两次哈希的算法不一样，第二次哈希的过程更相似于普通的非线程安全的哈希表，但是两次哈希之前都推荐做jenkins预处理 
 */ 
template<typename T>
unsigned int Segment<T>::second_hash(unsigned int code) const
{
	return jenkins_prehash(code)&(capacity-1);
}

/**
 * jenkins_prehash: 哈希预处理
 * note: 两次哈希之前都需要进行哈希预处理，预处理的目的是充分混杂，从而优化哈希的效果 
 */
template<typename T>
unsigned int Segment<T>::jenkins_prehash(unsigned int h) const
{
	h += (h << 15) ^ 0xffffcd7d; 
	h ^= (h >> 10);
	h += (h << 3); h ^= (h >> 6);
	h += (h << 2) + (h << 14); 
	return h ^ (h >> 16);
}
   
/**
 * ~Segment: 虚析构函数 
 */
template<typename T>
Segment<T>::~Segment()
{
	for(int i=0;i<capacity;i++)
	{
		delete data[i];
	}
	delete [] data;
}

#endif
