#ifndef POLYNOMIAL_H  
#define POLYNOMIAL_h
#include "Term.h"
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <cmath>

using namespace std;
class Term;

class Polyno
{ 
    public:
    Polyno()=default;                                                  //默认构造函数
    Polyno(vector<double> nums,string iname="temp");                   //使用系数和次数交替出现的数组构造多项式
    Polyno(int max_exp,string iname="temp");                           //仅使用最高项的次数构造多项式，所有的系数都会被置为0
    Polyno(const Polyno &other,string iname="temp");                   //拷贝构造函数
    void expandExp(int newexp);                                        //拓展最高次数的成员函数：增大多项式当前的最高项次数（阶）
    friend ostream & operator<< (ostream &out,const Polyno &target);   //重载输出运算符：按照格式输出多项式
    Polyno operator+ (Polyno other);                                   //重载加法运算符：将两个多项式相加
    Polyno operator- (Polyno other);                                   //重载减法运算符：用本多项式减去另一个多项式
    Polyno operator* (Polyno other);                                   //重载乘法运算符：将多项式和常数相乘
    Polyno operator* (const double coe);                               //重载乘法运算符：将多项式和另一个多项式相乘
    Polyno &operator+= (Polyno other);                                 //加法赋值运算符
    Polyno &operator-= (Polyno other);                                 //减法赋值运算符
    Polyno &operator*= (Polyno other);                                 //多项式乘法赋值运算符
    Polyno &operator*= (const double coe);                             //常数乘法赋值运算符
    Polyno &operator= (Polyno other);                                  //拷贝赋值运算符
    bool operator== (Polyno other);                                    //相等性判断
    Polyno dev();                                                      //对多项式进行求导
    double solution(double x) const;                                   //代入自变量的值求解多项式的值
    string getName() const;                                            //获得该多项式的名称
    private:
    int MaxExp;                                                        //多项式的项的最高次数
    vector<Term> pol;                                                  //多项式的每一项所在容器
    string name;                                                       //该多项式的名称
};


Polyno::Polyno(vector<double> nums,string iname)   //nums的要求如下，必须总共含有偶数个数，偶数下标的为系数，奇数下标的为次数
{
    //下面采用的这种构造方法无需nums在次数的排列上有序
    //先构造多项式的名字
    name=iname;
    //再计算提供的项的数据中项的最高次数
    int max_size=0;
    for(int i=1;i<nums.size();i+=2)
    {
        max_size=(nums[i]>max_size)?nums[i]:max_size;
    }
    MaxExp=max_size;
    //现在构造最高次数相同，而系数全部为0的多项式
    for(int i=0;i<=MaxExp;i++)
    {
        pol.push_back(Term(0,i));   //现在vector的下标和项的次数一一对应
    }
    //最后在刚刚得到多项式中的插入给定的系数，构造过程即完成
    for(int i=0;i<nums.size();i+=2)
    {
        //现在pol[nums[i+1]]就是对应次数的项
        pol[nums[i+1]].ChangeCoe(nums[i]);
    }
}

Polyno::Polyno(int max_exp,string iname)   //仅使用最高次数来构造多项式，这时得到的多项式的系数均为0
{
    MaxExp=max_exp;
    name=iname;
    //新构造的多项式的最高次数和给定的最高次数一致，但是每一项的系数均为0
    for(int i=0;i<=max_exp;i++)
    {
        pol.push_back(Term(0,i));      
    }
}

Polyno::Polyno(const Polyno &other,string iname)  //拷贝构造函数
{
    pol=other.pol;
    MaxExp=other.MaxExp;
    //当提供拷贝的一个新名字时使用新的名字，否则拷贝原对象的名字
    if(iname=="temp")
    {
    	name=other.name;
	}
	else
	{
		name=iname;
	}
}

void Polyno::expandExp(int newexp)   //对多项式的最高次数做拓展
{
    //当提供的最高次数不大于当前多项式的最高次数，保持原多项式不做改变
    if(newexp<=MaxExp)
    {
        return;
    }
    else
    {
        //新拓展出的高次项系数全部置0
        for(int i=MaxExp+1;i<=newexp;i++)
        {
            pol.push_back(Term(0,i));
        }
    }
}

ostream & operator<< (ostream &out,const Polyno &target)   //重载输出运算符
{
    //下面的代码主要处理了以下几个问题：
    //1.系数为1的时候不显示1
    //2.系数为负的时候不显示"+"运算符
    //3.次数为1的时候不显示次数
    string res;
    if(target.name!="temp")
    out<<target.name<<"=";
    for(int i=target.MaxExp;i>=0;i--)
    {
        //先构造符号,只需构造加号即可
        if((target.pol[i].GetCoe())>0&&i!=target.MaxExp)
        out<<"+";
        //再输出系数
        if(target.pol[i].GetCoe()!=0&&target.pol[i].GetCoe()!=1)
        out<<target.pol[i].GetCoe();
        if(target.pol[i].GetCoe()==1&&target.pol[i].GetExp()==0)
        out<<"1";
        //再输出次数
        if(target.pol[i].GetExp()!=0&&target.pol[i].GetCoe()!=0)
        {
        	if(target.pol[i].GetExp()==1)
        	{
        		out<<"x";
			}
			else
			{
				out<<"x^"<<target.pol[i].GetExp();
			}
		}
    }
    return out;
}

Polyno Polyno::operator+ (Polyno other)   //重载加法运算符
{
    //先计算可能使用的多项式的最高次数
    int MaxOfTwo=(MaxExp>other.MaxExp)?MaxExp:other.MaxExp;
    //现在创建足够多项指数的结果
    Polyno res(MaxOfTwo);
    //现在扩展两个多项式到相同系数，这个操作并不会影响另外参与操作的多项式，因为本函数使用的是值传递
    (*this).expandExp(MaxOfTwo);
    other.expandExp(MaxOfTwo);
    //然后逐项进行相加即可构造出执行加法后的结果
    for(int i=0;i<=MaxOfTwo;i++)
    {
        res.pol[i]=pol[i]+other.pol[i];
    }
    return res;
}

Polyno Polyno::operator- (Polyno other)
{
    //先计算可能使用的多项式的最高次数
    int MaxOfTwo=(MaxExp>other.MaxExp)?MaxExp:other.MaxExp;
    //现在先创建足够多项指数的结果
    Polyno res(MaxOfTwo);
    //现在扩展两个多项式到相同系数，这个操作并不会影响另外参与操作的多项式，因为本函数使用的是值传递
    (*this).expandExp(MaxOfTwo);
    other.expandExp(MaxOfTwo);
    //然后逐项进行相减即可构造出执行减法后的结果
    for(int i=0;i<=MaxOfTwo;i++)
    {
        res.pol[i]=pol[i]-other.pol[i];
    }
    return res;
}

Polyno Polyno::operator* (Polyno other)   //重载乘法运算符
{
    //先计算出乘法得到的结果中可能出现的最高次数
    int Max=MaxExp+other.MaxExp;
    //然后构造足够高次的多项式作为结果
    Polyno res(Max);
    //下面计算给定次数的项的系数
    for(int i=0;i<=MaxExp;i++)
    {
        for(int p=0;p<=other.MaxExp;p++)
        {
            //将两个多项式的项逐项相乘，相乘后的系数加到结果中相同次数的项的系数上，这里的下标和对应的项的次数相等，而i+p就是两个项相乘后的系数
            res.pol[i+p].PlusExp(pol[i].GetCoe()*(other.pol[p].GetCoe()));
        }
    }
    return res;
}

Polyno Polyno::operator* (const double coe)  //重载乘法运算符
{
    Polyno res(*this);
    for(int i=0;i<=res.MaxExp;i++)
    {
        res.pol[i]*=coe;    //逐项乘以常数即完成多项式和常数相乘
    }
    return res;
}

Polyno &Polyno::operator+= (Polyno other)
{
    //这个运算符的实现和+相似
    int MaxOfTwo=(MaxExp>other.MaxExp)?MaxExp:other.MaxExp;
    //现在扩展两个多项式到相同系数，这个操作并不会影响另外参与操作的多项式，因为本函数使用的是值传递
    (*this).expandExp(MaxOfTwo);
    other.expandExp(MaxOfTwo);
    //然后直接逐项进行相加，本处之所以可以直接逐项进行相加是因为在之前的操作中两个多项式都没有任何的空缺项，所以即使是1,3,5和2,4,6这样的交错项依然可以逐项相加
    for(int i=0;i<=MaxOfTwo;i++)
    {
        pol[i]+=other.pol[i];
    }
    return (*this);
}

Polyno &Polyno::operator-= (Polyno other)
{
    //这个运算符的实现和-相似
    int MaxOfTwo=(MaxExp>other.MaxExp)?MaxExp:other.MaxExp;
    //现在扩展两个多项式到相同系数，这个操作并不会影响另外参与操作的多项式，因为本函数使用的是值传递
    (*this).expandExp(MaxOfTwo);
    other.expandExp(MaxOfTwo);
    //然后直接逐项进行相减，本处之所以可以直接逐项进行相加是因为在之前的操作中两个多项式都没有任何的空缺项，所以即使是1,3,5和2,4,6这样的交错项依然可以逐项相加
    for(int i=0;i<=MaxOfTwo;i++)
    {
        pol[i]-=other.pol[i];
    }
    return (*this);
}

Polyno &Polyno::operator*= (Polyno other)
{
    //这个运算符的实现和*运算符相似
    int Max=MaxExp+other.MaxExp;
    Polyno res(Max);
    for(int i=0;i<=MaxExp;i++)
    {
        for(int p=0;p<=other.MaxExp;p++)
        {
            res.pol[i+p].PlusExp(pol[i].GetCoe()*(other.pol[p].GetCoe()));
        }
    }
    (*this)=res; 
    return (*this); 
}

Polyno &Polyno::operator*= (const double coe)
{
    //这个运算符的实现和*相似
    for(int i=0;i<=MaxExp;i++)
    {
        pol[i]*=coe;
    }
    return (*this);
}

bool Polyno::operator== (Polyno other)    
{
    //先计算两个多项式的最高次数
    int Max=(MaxExp>other.MaxExp)?MaxExp:other.MaxExp;
    //然后对该两个多项式进行拓展
    (*this).expandExp(Max);
    other.expandExp(Max);
    //现在逐项进行比对即可
    for(int i=0;i<=Max;i++)
    {
        if(pol[i].GetCoe()!=other.pol[i].GetCoe())
        return false;
    }
    return true;
}

Polyno &Polyno::operator= (Polyno other)  //拷贝赋值运算符
{
	name=other.name;
	pol=other.pol;
	MaxExp=other.MaxExp;
}

Polyno Polyno::dev()    //多项式求导成员函数
{ 
    //先创建作为返回值的结果的多项式，这个多项式的最高次数比当前的最高次数少1
    Polyno res(MaxExp-1);
    //然后逐项对作为返回值的结果的多项式进行求导
    for(int i=1;i<=MaxExp;i++)  //这里从次数1开始操作即可，因为常数的求导为0
    {
        res.pol[i-1].ChangeCoe(pol[i].GetCoe()*i);
    }
    return res;
}

double Polyno::solution(double x) const   //代入未知量的给定值
{
    double res=0;
    for(int i=0;i<=MaxExp;i++)
    {
        res+=(pol[i].GetCoe())*pow(x,i);  //直接逐项进行计算再相加即可
    }
    return res;
}

string Polyno::getName() const    //获得该多项式的名字
{
    return name;   //未提供名字的多项式统一命名为temp
}

#endif 
