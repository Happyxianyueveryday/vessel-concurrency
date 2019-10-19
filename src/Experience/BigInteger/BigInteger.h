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
 * BigInteger: ������������
 * note: ��������֧�ּӷ����������˷�������ֵ�ͳ��ñȽ����㣬���ǲ�֧�ֳ�������Ҫʹ�ó�����ʹ��BigDecimal�� 
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
    std::vector<int> data;       // ���ݴ洢 
    const int base = static_cast<int>(pow(10, BASE));   // ��������Ļ���Ҫ�󣬰���3λ��Ϊ������λ���д洢 
    bool sign;                   // ������

    // 5. private functions
    void move_zeros();
    bool abs_less_than(const BigInteger &left, const BigInteger &right) const;
    bool abs_equals(const BigInteger &left, const BigInteger &right) const;
};

/**
 * BigInteger: ����ͨint����BigInteger 
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
 * BigInteger: ���ַ�������BigInteger 
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
		data.push_back(atoi(number.substr(begin, end-begin).c_str()));   // ע��substr������python����ͬ���ڶ��������ǽ�ȡ���� 
	}
}

/**
 * BigInteger: �������캯��
 * param interger: ����Դ����
 * param change_sign: ����ʱ�Ƿ�ı���ţ�Ϊtrue��ı���� 
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
 * move_zeros: �Ƴ�ǰ��0
 */
void BigInteger::move_zeros() 
{
    while(data.size()>1&&data.back()==0)   // note: ��Ϊdataʹ�õ�װ����ʽ�����ǰ��0������data��β�� 
    {
        data.pop_back();
    }
}

/**
 * abs: ��BigInteger�ľ���ֵ 
 */
const BigInteger BigInteger::abs(const BigInteger & integer) 
{
    return BigInteger(integer, integer.sign);
}

/**
 * abs_less_than: ����ֵ�Ƚϴ�С 
 */
bool BigInteger::abs_less_than(const BigInteger &left, const BigInteger &right) const 
{
    if(left.data.size()<right.data.size())   // ��left����С��right���ȣ���leftС��right 
	return true;
    if(left.data.size()>right.data.size())   // ��left���ȴ���right���ȣ���left����right 
	return false;
    for(long int i=left.data.size()-1;i>=0;i--) // ��������Ҫ���αȽ�data�е�ÿ�����У�˳���Ǵ�ԭ���ĸ�λ����λ���бȽ� 
	{
        if(left.data[i]<right.data[i]) 
		return true;
        if(left.data[i]>right.data[i])
		return false;
    }
    return false;
}

/**
 * abs_equals: ����ֵ�Ƚ��Ƿ���� 
 */
bool BigInteger::abs_equals(const BigInteger &left, const BigInteger &right) const 
{
    if(left.data.size()!=right.data.size())  	   // ��left���Ⱥ�right���Ȳ���ȣ��������ض������ 
	return false;   
    for (long int i=left.data.size()-1;i>=0;i--)   // �������αȽ�data�����е�ÿһ����λ��ȫ���������ʱ������� 
	{
        if(left.data[i]!=right.data[i]) 
		return false;
    }
    return true;
}

/**
 * operator=: �ж������Ƿ���� 
 */
BigInteger &BigInteger::operator=(const BigInteger &right) 
{
	// ���жϷ����Ƿ���ȣ����������abs_equals�ķ������н�һ���Ƚϼ��� 
    this->sign=right.sign;
    this->data.clear();
    for(auto item:right.data)
	{
        this->data.push_back(item);
    }
    return *this;
}

/**
 * operator+=: +=����� 
 */
BigInteger &BigInteger::operator+=(const BigInteger &right) 
{
    *this=*this+right;    
    return *this;
}

/**
 * operator-=: -=����� 
 */
BigInteger &BigInteger::operator-=(const BigInteger &right) 
{
    *this=*this-right;
    return *this;
}

/**
 * operator*=: *=����� 
 */
BigInteger &BigInteger::operator*=(const BigInteger &right) 
{
    *this=(*this)*right;
    return *this;
}

/**
 * operator+: �ӷ������ 
 */
const BigInteger operator+(const BigInteger &left, const BigInteger &right) 
{
	// 1. ���ȴ�����ţ����������ȫ��ͨ���Ը���ȡ�෴��ת��Ϊ��+�������
    if(left.sign)    
	{
        if(right.sign)     // ���1����+�� 
		return -((-left)+(-right));
		else               // ���2����+�� 
        return right-(-left);
    } 
	else              
	{
        if(right.sign)     // ���3����+�� 
		return left-(-right);
    }
    // 2. ��������������+�� 
    int carry = 0;    	   // ��λֵ 
    BigInteger result;     // �ӷ������� 
    result.data.clear();
    for (int i=0;i<std::max(left.data.size(), right.data.size())||carry;i++)   // ��leetcode�ľ���ϰ��two sumʹ����ͬ���㷨���� 
	{
        int current=(i<left.data.size()?left.data[i]:0)+(i<right.data.size()?right.data[i]:0)+carry;
        carry=(current>=left.base);   // ������λ����������1000������Ҫ������λ��1 
        result.data.push_back(current%left.base);
    }
    return result;
}

/**
 * operator-: ��������� 
 */
const BigInteger operator-(const BigInteger &left, const BigInteger &right)
{
	// 1. ���ȴ�����ţ����������ȫ��ͨ���Ը���ȡ�෴��ת��Ϊ��-���Ҳ�Ϊ���������
    if(left.sign) 
	{
        if(right.sign)   // ��-�� 
		return -((-left)-(-right));
		else             // ��-�� 
        return -((-left)+right);
    }
	else 
	{
        if(right.sign)   // ��-�� 
		return left+(-right);
        if(left < right) // ��-�������ǲ�Ϊ�� 
		return -(right-left);
    }
    // 2. ��������������+�����ҽ��Ϊ���� 
    int carry=0;
    BigInteger result;
    result.data.clear();
    for(long int i=0;i<left.data.size()||carry;i++)
	{
        int current=(i<left.data.size()?left.data[i]:0)-(i<right.data.size()?right.data[i]:0)-carry;
        carry=(current<0);
        if(carry)         // ��ǰ��λ��λ��������base��ֵ1000 
		result.data.push_back(current+left.base);
		else
		result.data.push_back(current);
    }
    result.move_zeros();   // �Ƴ�ǰ��0 
    return result;
}

/**
 * operator*: �˷������ 
 */
const BigInteger operator*(const BigInteger &left, const BigInteger &right) 
{
    BigInteger result;
    result.data.resize(left.data.size()+right.data.size());    // aλ������bλ����������Ϊ(a+b)λ�� 
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
    result.move_zeros();                     // �Ƴ�ǰ��0 
    result.sign=(left.sign!=right.sign);     // ͬ�ŵ�������ŵø� 
    return result;
}

/**
 * operator+: ��������� 
 */
const BigInteger operator+(const BigInteger &integer) 
{
    return BigInteger(integer);
}

/**
 * operator-: ��������� 
 */
const BigInteger operator-(const BigInteger &integer) 
{
    return BigInteger(integer, true);
}

/**
 * operator<: С������� 
 */
const bool operator<(const BigInteger & left, const BigInteger & right) 
{
	// 1. �ȽϷ��Ž����ж� 
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
    // 2. �Ƚϳ��Ƚ����ж� 
    if(left.data.size()>right.data.size()) 
	return false;
    if(left.data.size()<right.data.size())
	return true;
	// 3. ������ͬ��Ӹ�λ����λ�����αȽ�ÿ����λ 
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
 * operator<=: С�ڵ�������� 
 */
const bool operator<=(const BigInteger &left, const BigInteger &right)
{
    return !(left>right);
}

/** 
 * opeartor>: ���������
 * note: �ȽϷ�ʽ��С���������ͬ 
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
 * operator>=: ���ڵ�������� 
 */
const bool operator>=(const BigInteger & left, const BigInteger & right)
{
    return !(left<right);
}

/**
 * operator!=: ����������� 
 */
const bool operator!=(const BigInteger & left, const BigInteger & right) 
{
    if(left.sign!=right.sign) 
	return true;
	else
    return left.data!=right.data;
}

/**
 * operator==: �������� 
 */
const bool operator==(const BigInteger &left, const BigInteger &right) 
{
    if(left.sign!=right.sign)
	return false;
	else
    return left.data==right.data;
}

/**
 * operator<<: ���������� 
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
