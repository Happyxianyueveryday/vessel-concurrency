#ifndef ZIP_HASH_MAP_H          
#define ZIP_HASH_MAP_H       
#include "HashMap.h"
#include <iostream>
#include <string>
#include <vector> 

/** 
 * HashEntry: 哈希表中存储的键值对 
 * typename T: 哈希表键的类型
 * typename R: 哈希表值的类型 
 */ 
template <typename T, typename R>
struct HashEntry
{
    HashEntry(T _key=T(), R _val=R()):key(_key), val(_val), next(nullptr)
    {
    }
    const T key;        // 键，键值对中的键不能修改
    R val;              // 值，键值对中的值允许修改  
    HashEntry *next;    // 指向链表下一个结点/键值对的指针
};

/**
 * ZipHashMap: 基于拉链法的哈希表 
 * typename T: 哈希表键的类型
 * typename R: 哈希表值的类型 
 */
template <typename T, typename R>
class ZipHashMap: public HashMap<T, R> 
{
    public:
    // 1. constructor
    ZipHashMap();                                                 // 默认构造函数，构造空的哈希表 
    ZipHashMap(const std::vector<T> &_keys, const std::vector<R> &_values); // 从数组构造哈希表

    // 2. copy/move_controller
    ZipHashMap(const ZipHashMap &other);                   // 拷贝构造函数（深拷贝） 
    ZipHashMap &operator= (const ZipHashMap &other);       // 拷贝赋值运算符（深拷贝） 
    ZipHashMap(ZipHashMap &&other);                        // 移动构造函数 （浅拷贝/移动）
	ZipHashMap &operator= (ZipHashMap &&other);            // 移动赋值运算符（浅拷贝/移动） 

    // 3. methods
    void insert(T key, R value) override;         // 向哈希表中插入指定值
    void remove(T key) override;                  // 从哈希表中移除指定值
    bool contain(T key) const override;           // 判断哈希表中是否包含指定值
    bool get(T key, R &value) const override;     // 查找哈希表中给定键对应的值 
    std::vector<T> keys() const override;              // 获得哈希表中所有值的列表
    int length() const override;                  // 获取哈希表的元素个数 
    bool operator== (const ZipHashMap &other);    // 相等判断运算符

    // 4. destructor
    ~ZipHashMap();                                // 哈希表抽象基类虚析构函数

    private:
    // 5. domains
    HashEntry<T, R> **data;       // 哈希表数组结构
    unsigned int capacity;        // 哈希表桶的数量（即数组的长度），拉链法中，获取到元素的哈希值后将哈希值模桶数量，就可以得到具体的存放的桶的下标位置
    double load_factor;           // 哈希表负载因子
    unsigned int threshold;       // 哈希表扩容阈值，阈值=桶长度*负载因子，通常的拉链法哈希表的扩容策略是：在执行insert方法插入元素后，检查哈希表中的元素个数size是否大于阈值threshold，若大于，则调用resize方法进行扩容，具体的扩容方法请参见resize方法
    unsigned int size;            // 当前哈希表中的元素个数

    // 6. private functions
    void resize();       // 哈希表扩容方法
    unsigned int hash(unsigned int input) const;             // 根据Jenkins预处理后的键的哈希值计算所在的桶的下标 
    unsigned int jenkins_prehash(unsigned int input) const;     // Jenkins哈希值预处理，键的hashCode()返回值必须经过该步预处理 
};

/**
 * ZipHashMap: 默认构造函数，构造空的哈希表 
 */
template<typename T, typename R>
ZipHashMap<T, R>::ZipHashMap()
{
	size=0;    									// 初始化元素个数为0
	capacity=16;        						// 默认桶的数量初始值为16，16为2的幂，并且每次扩容均乘以2保持桶数量均为2的整数次幂 
	load_factor=0.75;   						// 默认的负载因子初始值为0.75
	threshold=capacity*load_factor;        		// 哈希表扩容阈值，其值始终等于桶数量乘以负载因子
	data=new HashEntry<T, R>* [capacity];     	// 初始化桶 
	
	for(int i=0;i<capacity;i++)
	{
		data[i]=nullptr;     // 初始化每个桶中的链表为空 
	}
}

/**
 * ZipHashMap: 从数组构造哈希集合 
 * param data: 用于构造的原始输入数组 
 */
template<typename T, typename R>
ZipHashMap<T, R>::ZipHashMap(const std::vector<T> &_keys, const std::vector<R> &_values)
{
	size=0;    									// 初始化元素个数为0
	capacity=16;        						// 默认桶的数量初始值为16，16为2的幂，并且每次扩容均乘以2保持桶数量均为2的整数次幂 
	load_factor=0.75;   						// 默认的负载因子初始值为0.75
	threshold=capacity*load_factor;        		// 哈希表扩容阈值，其值始终等于桶数量乘以负载因子
	data=new HashEntry<T, R>* [capacity];     	// 初始化桶
	
	for(int i=0;i<capacity;i++)
	{
		data[i]=nullptr;     // 初始化每个桶中的链表为空 
	}
	
	for(int i=0;i<_keys.size();i++)
	{
		this->insert(_keys[i], _values[i]);
	}
}

/**
 * ZipHashMap: 拷贝构造函数 
 */
template<typename T, typename R>
ZipHashMap<T, R>::ZipHashMap(const ZipHashMap &other)
{
	size=other.size;    									
	capacity=other.capacity;        						
	load_factor=other.load_factor;   						
	threshold=capacity*load_factor;
	data=new HashEntry<T, R>* [capacity];     		
	
	for(int i=0;i<capacity;i++)
	{
		if(other.data[i])
		{
			data[i]=new HashEntry<T, R> (other.data[i]->key, other.data[i]->val);
			HashEntry<T, R> *now=data[i], *othnow=other.data[i]->next;
			while(othnow)
			{
				now->next=new HashEntry<T, R> (othnow->key, othnow->val);
				now=now->next;
				othnow=othnow->next; 
			}
		}
		else
		data[i]=nullptr;
	}
}

/**
 * operator=: 拷贝赋值运算符 
 */
template<typename T, typename R>
ZipHashMap<T, R> &ZipHashMap<T, R>::operator= (const ZipHashMap &other)
{
	if(this==&other)
	return (*this);
	
	for(int i=0;i<capacity;i++)
	{
		HashEntry<T, R> *now=data[i];
		while(now)
		{
			HashEntry<T, R> *temp=now;
			now=now->next;
			delete temp;
		}
	}
	delete data;
	
	size=other.size;    									
	capacity=other.capacity;        						
	load_factor=other.load_factor;   						
	threshold=capacity*load_factor;
	data=new HashEntry<T, R>* [capacity];     		
	
	for(int i=0;i<capacity;i++)
	{
		if(other.data[i])
		{
			data[i]=new HashEntry<T, R> (other.data[i]->key, other.data[i]->val);
			HashEntry<T, R> *now=data[i], *othnow=other.data[i]->next;
			while(othnow)
			{
				now->next=new HashEntry<T, R> (othnow->key, othnow->val);
				now=now->next;
				othnow=othnow->next; 
			}
		}
		else
		data[i]=nullptr;
	}
	
	return (*this);
}

/**
 * ZipHashMap: 移动构造函数 
 */
template<typename T, typename R>
ZipHashMap<T, R>::ZipHashMap(ZipHashMap<T, R> &&other) 
{
	// 1. 从拷贝源对象浅拷贝成员变量的值到当前变量 
	size=other.size;           // 哈希表元素个数 
	capacity=other.capacity;   // 哈希表桶个数
	load_factor=other.load_factor;   // 装载因子
	threshold=capacity*load_factor;  // 扩容阈值 
	data=other.data;                 // 底层存储结构指针
	
	// 2. 将拷贝源对象的成员变量的值设定为默认值（即默认构造函数中的取值）
	other.size=0;    								
	other.capacity=16;        					
	other.load_factor=0.75;   					
	other.threshold=capacity*load_factor;        		
	other.data=new HashEntry<T, R>* [capacity];     			
	
	for(int i=0;i<capacity;i++)
	{
		other.data[i]=nullptr;
	}
}

/**
 * operator=: 移动赋值运算符，借助cpp11的移动语义提供安全的浅拷贝/移动手段  
 */ 
template<typename T, typename R>
ZipHashMap<T, R> &ZipHashMap<T, R>::operator= (ZipHashMap &&other)
{
	// 1. 判断是否为自身赋值
	if(this==&other)
	return (*this);
	
	// 2. 释放自身对象所使用的堆空间资源
	for(int i=0;i<capacity;i++)
	{
		HashEntry<T, R> *now=data[i];
		while(now)
		{
			HashEntry<T, R> *temp=now;
			now=now->next;
			delete temp;
		}
	}
	
	// 3. 从拷贝源对象浅拷贝成员变量的值到当前变量 
	size=other.size;           // 哈希表元素个数 
	capacity=other.capacity;   // 哈希表桶个数
	load_factor=other.load_factor;   // 装载因子
	threshold=capacity*load_factor;  // 扩容阈值 
	data=other.data;                 // 底层存储结构指针
	
	// 4. 将拷贝源对象的成员变量的值设定为默认值（即默认构造函数中的取值）
	other.size=0;    								
	other.capacity=16;         	 				
	other.load_factor=0.75;   					
	other.threshold=capacity*load_factor;        		
	other.data=new HashEntry<T, R>* [capacity];     			
	for(int i=0;i<capacity;i++)
	{
		other.data[i]=nullptr;
	}
	
	// 5. 返回当前对象的引用
	return (*this);
}

/**
 * insert: 向哈希表中插入键值对 
 * note: 若插入的元素值在哈希集合中已经存在重复，则不会进行插入，只是更新该键对应的值 
 */
template<typename T, typename R>
void ZipHashMap<T, R>::insert(T key, R value) 
{
	// 1. 计算应当插入的桶位置
	unsigned int index=hash(key.hashCode()); 
	// 2. 遍历该位置的桶中的链表，若遇到相同的键，则更新该键对应的值；若无相同的键，则在链表尾部插入新的键值对，并增加键值对计数；需要注意特别处理桶为空的情况 
	HashEntry<T, R> *now=data[index], *nowprev=nullptr;
	while(now) 
	{
		if(now->key.hashCode()==key.hashCode()&&now->key.equals(key))
		{				
			now->val=value;   // 哈希表中已经存在该键，则更新该键对应的值，无需增加键值对计数和扩容 
			return;
		}
		else
		{
			nowprev=now; 
			now=now->next;
		}
	}
	if(data[index])
	nowprev->next=new HashEntry<T, R> (key, value);
	else
	data[index]=new HashEntry<T, R> (key, value);
	// 3. 检查是否需要进行扩容 
	size+=1; 
	if(size>threshold)
	resize();
	return; 
}

/**
 * remove: 从哈希集合中删除指定值的元素
 * note: 若哈希集合中不存在该值，则实际上不会进行删除 
 */
template<typename T, typename R>
void ZipHashMap<T, R>::remove(T key) 
{
	// 1. 计算应当插入的桶位置
	unsigned int index=hash(key.hashCode()); 
	// 2. 若桶为空，则删除失败；否则依次查找桶中的链表，找到键相等的结点即进行删除
	if(data[index]) 
	{
		HashEntry<T, R> *now=data[index], *nowprev=nullptr;
		while(now)
		{
			if(now->key.hashCode()==key.hashCode()&&now->key.equals(key))
			{
				HashEntry<T, R> *temp=now;
				if(nowprev)
				nowprev->next=now->next;
				else
				data[index]=now->next;  
				
				delete temp;
				size-=1;
				return; 
			}
			else
			{
				nowprev=now;
				now=now->next;
			}
		}	
	}
	else
	return;
}

/**
 * contain: 判断给定值是否在哈希表中
 * note: 若给定值在哈希表中则返回true，否则返回false 
 */
template<typename T, typename R>
bool ZipHashMap<T, R>::contain(T key) const
{
	if(!size)
	return false;
	for(int i=0;i<capacity;i++)
	{
		HashEntry<T, R> *now=data[i];
		while(now)
		{
			if(now->key.hashCode()==key.hashCode()&&now->key.equals(key))
			return true;
			else
			now=now->next;
		}
	}
	return false;
}

/**
 * get: 获取哈希表中给定键对应的值
 * param key: 待查找的键
 * output-param value: 若查找到了对应的键，则通过该参数返回该键对应的值
 * return: 若查找成功，返回true；若查找失败，返回false 
 */
template<typename T, typename R>
bool ZipHashMap<T, R>::get(T key, R &value) const
{
	if(!size)
	return false;
	for(int i=0;i<capacity;i++)
	{
		HashEntry<T, R> *now=data[i];
		while(now)
		{
			if(now->key.hashCode()==key.hashCode()&&now->key.equals(key))
			{
				value=now->val;
				return true;
			}
			else
			now=now->next;
		}
	}
	return false;
}

/**
 * values: 获取哈希集合中的所有值的列表
 * return: 哈希集合中的所有元素的列表 
 */
template<typename T, typename R>
std::vector<T> ZipHashMap<T, R>::keys() const
{
	std::vector<T> res;
	if(!size)
	return res;
	for(int i=0;i<capacity;i++)
	{
		HashEntry<T, R> *now=data[i];
		while(now)
		{
			res.push_back(now->key);
			now=now->next;
		}
	}
	return res;
}

/**
 * length: 获取哈希表的元素个数 
 */
template<typename T, typename R>
int ZipHashMap<T, R>::length() const
{
	return size;
}

/** 
 * operator==: 判断两个哈希集合是否相等
 * note: 判断两个拉链法哈希表是否相等的算法存在很大的优化空间，需要进行深度的优化而不能随便设计。同时需要提醒的是：因为插入删除等操作的顺序不相同，并不能通过直接比较两个哈希表的结构来比较两个哈希表是否相等
 * note: 因此这里使用的比较算法如下：
 *       1. 首先判断传入的other指针地址是否和this指针相等，若相等，则直接返回true；否则进入步骤2
 *		 2. 然后判断传入的哈希集合中的元素数量是否和当前哈希集合相等，若不相等，则直接返回true；否则进入步骤3
 *       3. 最后，依次判断当前哈希集合中的每一个键值对是否在哈希集合other中，若均存在，则直接返回true，否则返回false		 
 */ 
template<typename T, typename R>
bool ZipHashMap<T, R>::operator== (const ZipHashMap &other)
{
	if(&other==this)
	return true;
	else if(other.size!=size)
	return false;
	else
	{
		for(int i=0;i<capacity;i++)
		{
			HashEntry<T, R> *now=data[i];
			while(now)
			{
				R othval;
				if(other.get(now->key, othval)&&othval.equals(now->val))
				now=now->next;
				else
				return false;
			}
		}
		return true;
	}
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
template<typename T, typename R>
void ZipHashMap<T, R>::resize() 
{
	// 1. 将桶数量capacity加倍
	unsigned int old_capacity=capacity; 
	capacity*=2;
	// 2. 重新计算阈值threshoud
	threshold=capacity*load_factor;
	// 3. 创建新的桶数组
	HashEntry<T, R> **new_data=new HashEntry<T, R>* [capacity]; 
	for(int i=0;i<capacity;i++)
	{
		new_data[i]=nullptr; 
	}
	// 4. 将原桶数组中的每个键值对依次使用与原来相同的哈希算法移动（而不是拷贝）到新的桶数组中 
	for(int i=0;i<old_capacity;i++)
	{
		while(data[i])
		{
			unsigned int index=hash(data[i]->key.hashCode());        // 计算新的哈希地址和位置 
			HashEntry<T, R> *temp=data[i]->next;
			if(new_data[index])
			{
				data[i]->next=new_data[index];
				new_data[index]=data[i];
			}
			else
			{
				data[i]->next=nullptr;      // 注意移动的结点需要将next指针置空 
				new_data[index]=data[i]; 
			} 
			data[i]=temp;
		}
		data[i]=nullptr;       // 原桶数组中的头部链表哑结点置空 
	}
	delete [] data;
	data=new_data;
}

/** 
 * hash: 根据对象的hashCode返回的哈希值计算插入桶中的位置 
 * note: 该方法得到的位置下标应当尽可能地均匀分散，因此jdk中使用的实现方式是：
 *       1. 哈希值扰动：将hashCode右移16位的结果和原来的hashCode做异或操作得到结果fianlhash，降低哈希值的冲突率 
 *       2. 取模运算：即finalhash%capacity，由于capacity是2的整数次幂，因此该运算可以使用效率较高的finalhash&(capacity-1)的位运算取代 
 */ 
template<typename T, typename R>
unsigned int ZipHashMap<T, R>::hash(unsigned int code) const
{
	return jenkins_prehash(code)&(capacity-1);
}

/**
 * jenkins_prehash: 哈希预处理
 * note: 两次哈希之前都需要进行哈希预处理，预处理的目的是充分混杂，从而优化哈希的效果 
 */
template<typename T, typename R>
unsigned int ZipHashMap<T, R>::jenkins_prehash(unsigned int h) const
{
	h += (h << 15) ^ 0xffffcd7d; 
	h ^= (h >> 10);
	h += (h << 3); h ^= (h >> 6);
	h += (h << 2) + (h << 14); 
	return h ^ (h >> 16);
}

/**
 * ~ZipHashMap: 虚析构函数 
 */
template<typename T, typename R>
ZipHashMap<T, R>::~ZipHashMap()
{
	for(int i=0;i<capacity;i++)
	{
		HashEntry<T, R> *now=data[i];
		while(now)
		{
			HashEntry<T, R> *temp=now;
			now=now->next;
			delete temp;
		}
	}
	delete [] data;
}

#endif
