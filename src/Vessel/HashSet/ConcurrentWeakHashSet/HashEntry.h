#ifndef HASHENTRY_H               
#define HASHENTRY_H    
#include <string>
#include <vector>
#include <memory>
#include <stack>
#include <mutex> 
#include <atomic>

template<typename T>
struct ListNode
{
	ListNode(T _val, std::shared_ptr<ListNode<T>> _next=nullptr):val(_val), next(_next)
	{
	}
	const T val;
	std::shared_ptr<ListNode<T>> next; // 所有的next初始化后即保证不被改变，shared_ptr读操作线程安全，因此无需使用原子操作 
};

template<typename T>
class HashEntry
{
	public: 
	// 1. constructor
	HashEntry();
	
	// 2. copy/move controller
	HashEntry(const HashEntry &other) = delete; 
	HashEntry &operator= (const HashEntry &other) = delete;
	HashEntry(HashEntry &&other) = delete;
	HashEntry &operator= (HashEntry &&other) = delete;
	
	// 3. methods
	bool insert(T val);
	bool remove(T val);
	bool contain(T val) const;
	std::string toString() const;
	std::vector<T> values() const;
	
	// 4. destructor
	~HashEntry();
	
	private:
	// 5. domains
	std::shared_ptr<ListNode<T>> head;  // head是可以被修改的shared_ptr，shared_ptr的写操作线程不安全，因此所有写操作都必须使用原子操作 
};

/**
 * HashEntry: 默认构造函数 
 */ 
template<typename T>
HashEntry<T>::HashEntry()
{
	// head默认初始化为空指针对象 
}

/**
 * insert: 向HashEntry插入元素
 * note: 首先检查当前HashEntry中是否已经存在待插入元素，若不存在，将待插入元素插入HashEntry链表的头部，本操作为写操作，必须加互斥锁 
 */
template<typename T>
bool HashEntry<T>::insert(T val) 
{ 
	// 1. 首先遍历HashEntry，判断HashEntry中是否已经存在该元素 
	std::shared_ptr<ListNode<T>> now=head;
	bool flag=false;
	while(now)
	{
		if(now->val.hashCode()==val.hashCode()&&now->val.equals(val))
		{
			flag=true;
			break;
		}
		else
		now=now->next;  
	}
	// 2. 已经存在该元素则无需重复插入，直接返回
	if(flag)
	return false;
	// 3. 固定在链表头部插入新的元素 
	std::shared_ptr<ListNode<T>> newnode=std::make_shared<ListNode<T>>(val, head);
	head=newnode;
	return true;
}
 
/**
 * remove: 从HashEntry中移除元素 
 * note: 从HashEntry中移除元素首先需要复制/深拷贝待删除结点前的所有结点，然后将新复制的链表和待删除结点后的链表部分连接起来，本操作为写操作，必须加互斥锁 
 */
template<typename T> 
bool HashEntry<T>::remove(T val)
{
	std::shared_ptr<ListNode<T>> now=head;                       
	while(now)   
	{
		if(now->val.hashCode()==val.hashCode()&&now->val.equals(val))   // 找到了对应的结点，则进行删除操作 
		{ 
			std::shared_ptr<ListNode<T>> new_head=now->next; 
			// 若待删除的结点不是首结点，则需要拷贝待删除结点之前的所有结点，组成一个新链表片段
			if(now!=head)    
			{ 
				new_head=std::make_shared<ListNode<T>>(head->val);
				std::shared_ptr<const ListNode<T>> pos=head->next;   // 禁止修改原链表中的成员，使用底层const的智能指针 
				std::shared_ptr<ListNode<T>> newpos=new_head;
				while(pos!=now)
				{
					newpos->next=std::make_shared<ListNode<T>>(pos->val);
					newpos=newpos->next;
					pos=pos->next;
				}
				// 连接拷贝的链表和待删除结点之后的链表
				newpos->next=now->next;
			}
			// 重置首结点为新复制的链表片段的首结点 
			head=new_head;         // 在该行代码执行之前的读操作，均在旧的哈希表上读，因此是安全的
			return true;
		}
		now=now->next;  
	}
	return false;
}

/**
 * contain: 判断某个元素是否在HashEntry中 
 * note: 直接遍历一遍HashEntry即可得到结果，该方法为读操作，无需加互斥锁 
 */ 
template<typename T>
bool HashEntry<T>::contain(T val) const
{
	std::shared_ptr<ListNode<T>> now=head;
	while(now)
	{
		if(now->val.hashCode()==val.hashCode()&&now->val.equals(val))
		return true;
		now=now->next;
	}
	return false;
}

/**
 * toString: 字符串形式输出HashEntry 
 * note: 直接遍历一遍HashEntry即可得到结果，该方法为读操作，无需加互斥锁 
 */ 
template<typename T>
std::string HashEntry<T>::toString() const
{
	std::shared_ptr<ListNode<T>> now=head;
	std::string res="*";
	while(now)
	{ 
		res=res+"->"+now->val.toString();
		now=now->next;	
	}
	return res;
}

/**
 * values: 获取HashEntry存储的值列表 
 * note: 直接遍历一遍HashEntry即可得到结果，该方法为读操作，无需加互斥锁 
 */        
template<typename T>
std::vector<T> HashEntry<T>::values() const
{
	std::shared_ptr<ListNode<T>> now=head;
	std::vector<T> res;
	while(now)
    {
    	res.push_back(now->val);
		now=now->next;
	}
	return res;
}

template<typename T>
HashEntry<T>::~HashEntry()
{
	head=nullptr;
}

#endif
