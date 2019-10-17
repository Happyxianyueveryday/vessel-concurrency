#ifndef HASHABLE_H                
#define HASHABLE_H     
#include <string>   

/**
 * Hashable: 测试所使用的哈希表键的基类 
 * note 1: 本派生类提供了一个应当如何继承Hashable并重写其中的方法，来支持使用HashMap，ConcurrentStrongHashMap等 
 * note 2: 哈希表中的键必须支持哈希和比较（即提供hashCode()方法和equals()方法），而值可以不支持哈希，但必须支持比较方法（即提供equals()方法） 
 */
class Hashable
{
	public:
	Hashable();                                              // 默认构造函数  
	Hashable(int _val);                              		 // 构造函数 
    Hashable(const Hashable &other);                         // 拷贝构造函数 
	virtual Hashable& operator= (const Hashable &other);     // 拷贝赋值运算符 
	virtual unsigned int hashCode() const;                   // 计算当前元素的哈希值 
	virtual bool equals(const Hashable &other) const;        // 比较相等运算符 
    virtual std::string toString() const;                         // 字符串输出当前元素 
    virtual ~Hashable();                                     // 虚析构函数
    
    private: 
    int val;    // 实际存储的值，测试元素类型实际上就是简单封装的一个int值 
};

Hashable::Hashable()
{
	val=0;
}

Hashable::Hashable(int _val)
{
	val=_val;
}

Hashable::Hashable(const Hashable &other)
{
	val=other.val;
}

Hashable& Hashable::operator= (const Hashable &other)
{
	if(&other==this)
	return (*this);
	
	val=other.val;
	return (*this);
}

unsigned int Hashable::hashCode() const
{
	return val; 
}

bool Hashable::equals(const Hashable &other) const
{
	if(val==other.val)// 如果dynamic_cast成功，且两者的值相等，则两个对象相等，返回true 
	return true;
	else                               // 其他情况，则两个对象不相等 
	return false;
}

std::string Hashable::toString() const
{
	return std::to_string(val);
}

Hashable::~Hashable() 
{
	//cout<<"Hashable的析构函数被调用"<<endl; 
}                                 

#endif
