#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H
#define BASE 3
#include <vector> 
#include <iostream>
#include <ostream>
#include <string>
#include <cmath>
#include <iomanip>

/**
 * BigInteger: 大整数基础类
 * note: 大整数类支持加法，减法，乘法，绝对值和常用比较运算，但是不支持除法，若要使用除法请使用BigDecimal类 
 */
class BigInteger 
{
	public:
    // 1. constructor
    explicit BigInteger(int number = 0);
    explicit BigInteger(std::string number);

    // 2. copy controller
    BigInteger(const BigInteger &integer, bool change_sign=false);

    // 3. methods
    static const BigInteger abs(const BigInteger &integer);
    BigInteger & operator=(const BigInteger &right);
    BigInteger & operator+=(const BigInteger &right);
    BigInteger & operator-=(const BigInteger &right);
    BigInteger & operator*=(const BigInteger &right);
    friend const BigInteger operator+(const BigInteger &integer);
    friend const BigInteger operator-(const BigInteger &integer);
    friend const BigInteger operator+(const BigInteger &left, const BigInteger &right);
    friend const BigInteger operator-(const BigInteger &left, const BigInteger &right);
    friend const BigInteger operator*(const BigInteger &left, const BigInteger &right);
    friend const bool operator< (const BigInteger &left, const BigInteger &right);
    friend const bool operator<=(const BigInteger &left, const BigInteger &right);
    friend const bool operator> (const BigInteger &left, const BigInteger &right);
    friend const bool operator>=(const BigInteger &left, const BigInteger &right);
    friend const bool operator!=(const BigInteger &left, const BigInteger &right);
    friend const bool operator==(const BigInteger &left, const BigInteger &right);
    friend std::ostream & operator<<(std::ostream &os, const BigInteger &integer);

    private:
    // 4. domains
    std::vector<int> data;       // 数据存储 
    const int base = static_cast<int>(pow(10, BASE));   // 遵守需求的基本要求，按照3位作为基础单位进行存储 
    bool sign;                   // 正负号

    // 5. private functions
    void move_zeros();
    bool abs_less_than(const BigInteger &left, const BigInteger &right) const;
    bool abs_equals(const BigInteger &left, const BigInteger &right) const;
};

/**
 * BigInteger: 从普通int构造BigInteger 
 */
BigInteger::BigInteger(int number) 
{
    if(number<0) 
    {
        sign=true;
        number=-number;
    } 
    else 
    {
        sign=false;
    }
    while(number>=base)      
    {
        data.push_back(number%base);
        number/=base;
    }
    if(number>0) 
    {
        data.push_back(number);
    }
    if(data.empty()) 
    {
        data.push_back(0);
    }
}

/**
 * BigInteger: 从字符串构造BigInteger 
 */ 
BigInteger::BigInteger(std::string number)
{
	if(number[0]=='-')
	{
		sign=true;    
		number=number.substr(1, number.size());
	}
	else
	sign=false; 
	
	for(int i=number.size();i>=0;i-=3)  
	{
		int begin=(i-3>0)?(i-3):0, end=i;
		data.push_back(atoi(number.substr(begin, end-begin).c_str()));   // 注意substr方法和python不相同，第二个参数是截取长度 
	}
}

/**
 * BigInteger: 拷贝构造函数
 * param interger: 拷贝源对象
 * param change_sign: 拷贝时是否改变符号，为true则改变符号 
 */
BigInteger::BigInteger(const BigInteger &integer, bool change_sign) 
{
    change_sign?(sign = !integer.sign):(sign = integer.sign);
    for (int item:integer.data) 
    {
        data.push_back(item);
    }
}

/**
 * move_zeros: 移除前置0
 */
void BigInteger::move_zeros() 
{
    while(data.size()>1&&data.back()==0)   // note: 因为data使用倒装的形式，因此前置0在数组data的尾部 
    {
        data.pop_back();
    }
}

/**
 * abs: 求BigInteger的绝对值 
 */
const BigInteger BigInteger::abs(const BigInteger & integer) 
{
    return BigInteger(integer, integer.sign);
}

/**
 * abs_less_than: 绝对值比较大小 
 */
bool BigInteger::abs_less_than(const BigInteger &left, const BigInteger &right) const 
{
    if(left.data.size()<right.data.size())   // 若left长度小于right长度，则left小于right 
	return true;
    if(left.data.size()>right.data.size())   // 若left长度大于right长度，则left大于right 
	return false;
    for(long int i=left.data.size()-1;i>=0;i--) // 否则，则需要依次比较data中的每个序列，顺序是从原数的高位到低位进行比较 
	{
        if(left.data[i]<right.data[i]) 
		return true;
        if(left.data[i]>right.data[i])
		return false;
    }
    return false;
}

/**
 * abs_equals: 绝对值比较是否相等 
 */
bool BigInteger::abs_equals(const BigInteger &left, const BigInteger &right) const 
{
    if(left.data.size()!=right.data.size())  	   // 若left长度和right长度不相等，则两数必定不相等 
	return false;   
    for (long int i=left.data.size()-1;i>=0;i--)   // 否则依次比较data序列中的每一个三位，全部依次相等时两数相等 
	{
        if(left.data[i]!=right.data[i]) 
		return false;
    }
    return true;
}

/**
 * operator=: 判断两数是否相等 
 */
BigInteger &BigInteger::operator=(const BigInteger &right) 
{
	// 先判断符号是否相等，若相等则按照abs_equals的方法进行进一步比较即可 
    this->sign=right.sign;
    this->data.clear();
    for(auto item:right.data)
	{
        this->data.push_back(item);
    }
    return *this;
}

/**
 * operator+=: +=运算符 
 */
BigInteger &BigInteger::operator+=(const BigInteger &right) 
{
    *this=*this+right;    
    return *this;
}

/**
 * operator-=: -=运算符 
 */
BigInteger &BigInteger::operator-=(const BigInteger &right) 
{
    *this=*this-right;
    return *this;
}

/**
 * operator*=: *=运算符 
 */
BigInteger &BigInteger::operator*=(const BigInteger &right) 
{
    *this=(*this)*right;
    return *this;
}

/**
 * operator+: 加法运算符 
 */
const BigInteger operator+(const BigInteger &left, const BigInteger &right) 
{
	// 1. 优先处理符号，带符号情况全部通过对负数取相反数转换为正+正的情况
    if(left.sign)    
	{
        if(right.sign)     // 情况1：负+负 
		return -((-left)+(-right));
		else               // 情况2：负+正 
        return right-(-left);
    } 
	else              
	{
        if(right.sign)     // 情况3：正+负 
		return left-(-right);
    }
    // 2. 处理基础情况：正+正 
    int carry = 0;    	   // 进位值 
    BigInteger result;     // 加法运算结果 
    result.data.clear();
    for (int i=0;i<std::max(left.data.size(), right.data.size())||carry;i++)   // 和leetcode的经典习题two sum使用相同的算法即可 
	{
        int current=(i<left.data.size()?left.data[i]:0)+(i<right.data.size()?right.data[i]:0)+carry;
        carry=(current>=left.base);   // 单个三位运算结果大于1000，则需要向上三位进1 
        result.data.push_back(current%left.base);
    }
    return result;
}

/**
 * operator-: 减法运算符 
 */
const BigInteger operator-(const BigInteger &left, const BigInteger &right)
{
	// 1. 优先处理符号，带符号情况全部通过对负数取相反数转换为正-正且差为正数的情况
    if(left.sign) 
	{
        if(right.sign)   // 负-负 
		return -((-left)-(-right));
		else             // 负-正 
        return -((-left)+right);
    }
	else 
	{
        if(right.sign)   // 正-负 
		return left+(-right);
        if(left < right) // 正-正，但是差为负 
		return -(right-left);
    }
    // 2. 处理基础情况：正+正，且结果为正数 
    int carry=0;
    BigInteger result;
    result.data.clear();
    for(long int i=0;i<left.data.size()||carry;i++)
	{
        int current=(i<left.data.size()?left.data[i]:0)-(i<right.data.size()?right.data[i]:0)-carry;
        carry=(current<0);
        if(carry)         // 向前三位借位，即加上base的值1000 
		result.data.push_back(current+left.base);
		else
		result.data.push_back(current);
    }
    result.move_zeros();   // 移除前导0 
    return result;
}

/**
 * operator*: 乘法运算符 
 */
const BigInteger operator*(const BigInteger &left, const BigInteger &right) 
{
    BigInteger result;
    result.data.resize(left.data.size()+right.data.size());    // a位数乘以b位数，结果最大为(a+b)位数 
    for(long int i=0;i<left.data.size();i++) 
	{
        int carry=0;
        for (long int j=0;j<right.data.size()||carry;j++) 
		{
            long int current=result.data[i+j]+carry+left.data[i]*(j<right.data.size()?right.data[j]:0);
            carry = static_cast<int>(current/left.base);
            result.data[i+j]=static_cast<int>(current%left.base);
        }
    }
    result.move_zeros();                     // 移除前导0 
    result.sign=(left.sign!=right.sign);     // 同号得正，异号得负 
    return result;
}

/**
 * operator+: 正号运算符 
 */
const BigInteger operator+(const BigInteger &integer) 
{
    return BigInteger(integer);
}

/**
 * operator-: 负号运算符 
 */
const BigInteger operator-(const BigInteger &integer) 
{
    return BigInteger(integer, true);
}

/**
 * operator<: 小于运算符 
 */
const bool operator<(const BigInteger & left, const BigInteger & right) 
{
	// 1. 比较符号进行判断 
    if(left.sign) 
	{
        if(!right.sign)
		return true;
    }
	else 
	{
        if (right.sign)
		return false;
    }
    // 2. 比较长度进行判断 
    if(left.data.size()>right.data.size()) 
	return false;
    if(left.data.size()<right.data.size())
	return true;
	// 3. 长度相同则从高位到低位，依次比较每个三位 
    for(long int i=left.data.size()-1;i>=0;i--)
	{
        if (left.data[i] > right.data[i]) 
		return false;
        if (left.data[i] < right.data[i]) 
		return true;
    }
    return false;
}

/**
 * operator<=: 小于等于运算符 
 */
const bool operator<=(const BigInteger &left, const BigInteger &right)
{
    return !(left>right);
}

/** 
 * opeartor>: 大于运算符
 * note: 比较方式和小于运算符相同 
 */
const bool operator>(const BigInteger &left, const BigInteger &right) 
{
    if(left.sign)
	{
        if(!right.sign)
		return false;
    }
	else
	{
        if(right.sign)
		return true;
    }
    if(left.data.size()>right.data.size())
	return true;
    if(left.data.size()<right.data.size())
	return false;
    for(long int i=left.data.size()-1;i>=0;i--) 
	{
        if (left.data[i]>right.data[i]) 
		return true;
        if (left.data[i]<right.data[i]) 
		return false;
    }
    return false;
}

/**
 * operator>=: 大于等于运算符 
 */
const bool operator>=(const BigInteger & left, const BigInteger & right)
{
    return !(left<right);
}

/**
 * operator!=: 不等于运算符 
 */
const bool operator!=(const BigInteger & left, const BigInteger & right) 
{
    if(left.sign!=right.sign) 
	return true;
	else
    return left.data!=right.data;
}

/**
 * operator==: 相等运算符 
 */
const bool operator==(const BigInteger &left, const BigInteger &right) 
{
    if(left.sign!=right.sign)
	return false;
	else
    return left.data==right.data;
}

/**
 * operator<<: 输出流运算符 
 */
std::ostream & operator<<(std::ostream & ost, const BigInteger & integer) 
{
    if(integer.sign) 
	{
        ost<< "-";
    }
    ost<<integer.data.back();
    char oldFill=ost.fill('0');
    for(long int i=integer.data.size()-2;i>=0;i--) 
	{
        ost<<std::setw(BASE)<<integer.data[i];
    }
    ost.fill(oldFill);
    return ost;
}

#endif
