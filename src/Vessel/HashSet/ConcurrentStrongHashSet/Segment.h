#ifndef SEGMENT_H                            
#define SEGMENT_H          
#include <iostream>  
#include <string>
#include <vector>
#include <mutex>  

using namespace std;

template <typename T>
struct ListNode
{
    ListNode(T _val=T()):val(_val), next(NULL)
    {
    }
    const T val;        // 当前结点存储的哈希元素对象，哈希表不提供修改对象的功能，因此结点值声明为const 
    ListNode *next;     // 指向链表下一个结点的指针
};

/**
 * Segment: 拉链法实现的哈希集合 
 */
template<typename T>      
class Segment 
{
    public:
    // 1. constructor
    Segment();                                // 构造函数
    Segment(const vector<T> &data);         // 从数组构造哈希表

    // 2. copy/move_controller
    Segment(Segment &other);                   // 拷贝构造函数（深拷贝） 
    Segment &operator= (Segment &other);       // 拷贝赋值运算符（深拷贝） 
    Segment(Segment &&other);                  // 移动构造函数 （浅拷贝/移动）
	Segment &operator= (Segment &&other);      // 移动赋值运算符（浅拷贝/移动） 

    // 3. methods
    void insert(T value);              // 向哈希表中插入指定值
    void remove(T value);              // 从哈希表中移除指定值
    bool contain(T value);             // 判断哈希表中是否包含指定值
    vector<T> values();                // 获得哈希表中所有值的列表
    int length();                      // 获取哈希表的元素个数 
	string toString();                 // 输出哈希表
    bool operator== (Segment &other);     // 相等判断运算符

    // 4. destructor
    ~Segment();                                         // 哈希表抽象基类虚析构函数

    private:
    // 5. domains
    ListNode<T> **data;           // 哈希表数组结构
    unsigned int capacity;        // 哈希表桶的数量（即数组的长度），拉链法中，获取到元素的哈希值后将哈希值模桶数量，就可以得到具体的存放的桶的下标位置
    double load_factor;           // 哈希表负载因子
    unsigned int threshold;       // 哈希表扩容阈值，阈值=桶长度*负载因子，通常的拉链法哈希表的扩容策略是：在执行insert方法插入元素后，检查哈希表中的元素个数size是否大于阈值threshold，若大于，则调用resize方法进行扩容，具体的扩容方法请参见resize方法
    unsigned int size;            // 当前哈希表中的元素个数
    mutex mut;        			  // 互斥锁：要保证强一致性，则必须为读操作加锁，这时并发性大幅下降 

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
	size=0;    									// 初始化元素个数为0
	capacity=16;        						// 默认桶的数量初始值为16，16为2的幂，并且每次扩容均乘以2保持桶数量均为2的整数次幂，因为根据实验，桶数量在2的整数次幂时元素哈希得越散，在各个桶中能近似均匀分布 
	load_factor=0.75;   						// 默认的负载因子初始值为0.75
	threshold=capacity*load_factor;        		// 哈希表扩容阈值，其值始终等于桶数量乘以负载因子
	data=new ListNode<T>* [capacity];     	    // 初始化桶，其中每个桶中的链表具有一个头部哑结点 
	
	for(int i=0;i<capacity;i++)
	{
		data[i]=nullptr;                        // 初始化时各个桶均为空，只有出现第一个需要插入的元素时才创建桶 
	}
}

/**
 * Segment: 从数组构造哈希集合 
 * param data: 用于构造的原始输入数组 
 */
template<typename T>
Segment<T>::Segment(const vector<T> &_data)
{
	size=0;    									// 初始化元素个数为0
	capacity=16;        						// 默认桶的数量初始值为16
	load_factor=0.75;   						// 默认的负载因子初始值为0.75
	threshold=capacity*load_factor;        		// 哈希表扩容阈值，其值始终等于桶数量乘以负载因子
	data=new ListNode<T>* [capacity];     	    // 初始化桶，其中每个桶中的链表具有一个头部哑结点
	
	for(int i=0;i<capacity;i++)
	{
		data[i]=nullptr;
	}
	
	for(int i=0;i<_data.size();i++)
	{
		this->insert(_data[i]);
	}
}

/**
 * Segment: 拷贝构造函数 
 */
template<typename T>
Segment<T>::Segment(Segment &other)
{
	lock_guard<mutex> other_guard(other.mut);
	
	size=other.size;    									
	capacity=other.capacity;        						
	load_factor=other.load_factor;   						
	threshold=capacity*load_factor;
	data=new ListNode<T>* [capacity];     		
	
	for(int i=0;i<capacity;i++)
	{
		if(other.data[i])
		{
			data[i]=new ListNode<T>();
			ListNode<T> *now=data[i], *othnow=other.data[i]->next;
			while(othnow)
			{
				now->next=new ListNode<T>(othnow->val);   // 使用拷贝构造函数或者拷贝赋值运算符时必须深拷贝哈希表元素 
				now=now->next;
				othnow=othnow->next;
			}
		}
		else
		data[i]=nullptr;
	}
	other.read_unlock();
}

/**
 * operator=: 拷贝赋值运算符 
 */
template<typename T>
Segment<T> &Segment<T>::operator= (Segment &other)
{
	if(this==&other)
	return (*this);
	
	lock_guard<mutex> guard(mut);
	lock_guard<mutex> other_guard(other.mut);
	for(int i=0;i<capacity;i++)
	{
		ListNode<T> *now=data[i];
		while(now)
		{
			ListNode<T> *temp=now;
			now=now->next;
			delete temp;
		}
	}
	delete data;
	
	size=other.size;    									
	capacity=other.capacity;        						
	load_factor=other.load_factor;   						
	threshold=capacity*load_factor;
	data=new ListNode<T>* [capacity];     		
	
	for(int i=0;i<capacity;i++)
	{
		if(other.data[i])
		{
			data[i]=new ListNode<T>();
			ListNode<T> *now=data[i], *othnow=other.data[i]->next;
			while(othnow)
			{
				now->next=new ListNode<T>(othnow->val);   // 使用拷贝构造函数或者拷贝赋值运算符时必须深拷贝哈希表元素 
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
 * Segment: 移动构造函数 
 */
template<typename T>
Segment<T>::Segment(Segment<T> &&other) 
{
	lock_guard<mutex> other_guard(other.mut);
	// 1. 从拷贝源对象浅拷贝成员变量的值到当前变量 
	size=other.size;           	     // 哈希表元素个数  
	capacity=other.capacity;   		 // 哈希表桶个数
	load_factor=other.load_factor;   // 装载因子
	threshold=capacity*load_factor;  // 扩容阈值 
	data=other.data;                 // 底层存储结构指针
	
	// 2. 将拷贝源对象的成员变量的值设定为默认值（即默认构造函数中的取值）
	other.size=0;    								
	other.capacity=16;        					
	other.load_factor=0.75;   					
	other.threshold=capacity*load_factor;        		
	other.data=new ListNode<T>* [capacity];     			
	
	for(int i=0;i<capacity;i++)
	{
		other.data[i]=nullptr;
	}
}

/**
 * operator=: 移动赋值运算符，借助cpp11的移动语义提供安全的浅拷贝/移动手段  
 */ 
template<typename T>
Segment<T> &Segment<T>::operator= (Segment &&other)
{
	// 1. 判断是否为自身赋值
	if(this==&other)
	return (*this);
	
	lock_guard<mutex> guard(mut);
	lock_guard<mutex> other_guard(other.mut);
	// 2. 释放自身对象所使用的堆空间资源
	for(int i=0;i<capacity;i++)
	{
		ListNode<T> *now=data[i];
		while(now)
		{
			ListNode<T> *temp=now;
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
	other.data=new ListNode<T>* [capacity];     			
	for(int i=0;i<capacity;i++)
	{
		other.data[i]=nullptr;
	}
	
	// 5. 返回当前对象的引用
	return (*this);
}

/**
 * insert: 向哈希集合中插入元素值
 * note: 若插入的元素值在哈希集合中已经存在重复，则不会进行插入
 */
template<typename T>
void Segment<T>::insert(T value) 
{
	// 1. 若传入的值已经存在于哈希集合中，则无需进行插入 
	if(this->contain(value))
	return;
	
	lock_guard<mutex> guard(mut);
	// 2. 计算应当插入的桶位置
	unsigned int index=second_hash(value.hashCode());
	// 4. 将对象插入到对应的桶中
	if(!data[index])   // 若为第一个插入到该桶的元素，则需要首先创建桶的头部哑结点 
	data[index]=new ListNode<T>();
	ListNode<T> *now=data[index];
	ListNode<T> *newnode=new ListNode<T>(value);
	newnode->next=now->next;
	now->next=newnode;
	// 5. 增加元素计数，判断是否需要扩容，若元素数量size大于阈值threshold，则调用resize方法进行扩容
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
	lock_guard<mutex> guard(mut);
	// 1. 计算待删除的值在哈希表中的位置
	unsigned int index=second_hash(value.hashCode()); 
	// 2. 遍历该桶中的链表，查找目标值的哈希表元素，前置双指针法进行删除 
	if(!data[index])   // 若该桶中没有元素，则直接返回 
	return;
	ListNode<T> *nowprev=data[index];   
	ListNode<T> *now=data[index]->next;
	while(now)
	{
		// 比较时使用和插入相同的算法，先使用hashCode方法比较，若相等则再调用equals方法比较，若不相等则直接认为比较的两个元素不相等 
		if(now->val.hashCode()==value.hashCode()&&now->val.equals(value))  
		{
			nowprev->next=now->next;
			delete now;   // 析构ListNode对象，析构哈希元素的操作已经由ListNode对象的析构函数处理了，这就是cpp的RAII原则 
			size-=1;
			break;
		}	
		else
		{
			nowprev=now;
			now=now->next;
		}
	}
	return;
}

/**
 * contain: 判断给定值是否在哈希表中
 * note: 若给定值在哈希表中则返回true，否则返回false 
 */
template<typename T>
bool Segment<T>::contain(T value)
{
	lock_guard<mutex> guard(mut);   
	// 1. 计算应该所在的桶位置
	unsigned int index=second_hash(value.hashCode());
	// 2. 从该桶的第一个链表结点开始依次进行比较
	// note: 比较算法是：首先调用hashCode对两个对象进行比较，若hashCode值相等，则调用==运算符进行进一步比较判断是否相等；若hashCode不相等，则直接认为两个对象不相等 
	if(!data[index]) 
	return false;
	ListNode<T> *now=data[index]->next; 
	while(now)
	{
		if(now->val.hashCode()==value.hashCode()&&now->val.equals(value))  // 先比较hashCode()方法的返回值是否相等，若相等再比较==方法来进行进一步比较 
		return true;
		else
		now=now->next;   
	}
	return false;
}

/**
 * values: 获取哈希集合中的所有值的列表
 * return: 哈希集合中的所有元素的列表 
 */
template<typename T>
vector<T> Segment<T>::values()
{
	lock_guard<mutex> guard(mut);
	vector<T> res;
	for(int i=0;i<capacity;i++)
	{
		if(data[i])
		{
			ListNode<T> *now=data[i]->next;
			while(now)
			{
				res.push_back(now->val);
				now=now->next;
			}
		}
	}
	return res;
}

/**
 * length: 获取哈希表的元素个数 
 */
template<typename T>
int Segment<T>::length()
{
	lock_guard<mutex> guard(mut);
	return size;
}

/**
 * toString: 生成哈希表的输出字符串 
 */
template<typename T>
string Segment<T>::toString()
{
	lock_guard<mutex> guard(mut);
    string res;
    for(int i=0;i<capacity;i++)
    {
    	if(i==0)
    	res+="*";
    	else
    	res+="\n*";
    	
    	if(data[i])
    	{
    		const ListNode<T> *now=data[i]->next;
    		while(now)
    		{
    			res=res+"->"+now->val.toString();
    			now=now->next;
			}
			res=res+"->NULL";
		}
	}
	return res;
}

/** 
 * operator==: 判断两个哈希集合是否相等
 * note: 判断两个拉链法哈希表是否相等的算法存在很大的优化空间，需要进行深度的优化而不能随便设计。同时需要提醒的是：因为插入删除等操作的顺序不相同，并不能通过直接比较两个哈希表的结构来比较两个哈希表是否相等
 * note: 因此这里使用的比较算法如下：
 *       1. 首先判断传入的other指针地址是否和this指针相等，若相等，则直接返回true；否则进入步骤2
 *		 2. 然后判断传入的哈希集合中的元素数量是否和当前哈希集合相等，若不相等，则直接返回true；否则进入步骤3
 *       3. 最后，依次判断当前哈希集合中的每一个元素是否在哈希集合other中，若均存在，则直接返回true，否则返回false		 
 */ 
template<typename T>
bool Segment<T>::operator== (Segment &other)
{
	lock_guard<mutex> guard(mut);
	lock_guard<mutex> other_guard(other.mut);
	if(&other==this)
	return true;
	else if(this->length()!=other.length())
	return false;
	for(int i=0;i<capacity;i++)
	{
		if(data[i])
		{
			ListNode<T> *now=data[i]->next;
			while(now)
			{
				if(this->contain(now->val))
				now=now->next;
				else
				return false;
			}
		}
	}
	return true;
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
	// 1. 将桶数量capacity加倍
	unsigned int old_capacity=capacity; 
	capacity*=2;
	// 2. 重新计算阈值threshoud
	threshold=capacity*load_factor;
	// 3. 创建新的桶数组，并且将原桶数组中的每个元素依次使用与原来相同的哈希算法插入到新的桶数组中
	ListNode<T> **new_data=new ListNode<T>* [capacity]; 
	for(int i=0;i<capacity;i++)
	{
		new_data[i]=nullptr;
	}
	for(int i=0;i<old_capacity;i++)
	{
		if(data[i])
		{
			ListNode<T> *now=data[i]->next;
			ListNode<T> *nowprev=data[i];
			
			while(now)
			{
				unsigned int index=second_hash(now->val.hashCode());        // 计算新的哈希地址和位置 
				nowprev->next=now->next;                  // 从原哈希表中移除该结点
				if(!new_data[index])                      // 插入前先判断是否存在桶，不存在则创建桶 
				new_data[index]=new ListNode<T>();      
				now->next=new_data[index]->next;          // 插入到新的桶数组的链表中 
				new_data[index]->next=now;
				now=nowprev->next;                        // 更新游标指针now 
			}
			delete data[i];       // 删除原桶数组中的头部链表哑结点 
		}
	}
	delete [] data;
	data=new_data;
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
		ListNode<T> *now=data[i];
		while(now)
		{
			ListNode<T> *temp=now;
			now=now->next;
			delete temp;
		}
	}
	delete data;
}

#endif
