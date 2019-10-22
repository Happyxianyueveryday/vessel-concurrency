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
    Polyno()=default;                                                  //Ĭ�Ϲ��캯��
    Polyno(vector<double> nums,string iname="temp");                   //ʹ��ϵ���ʹ���������ֵ����鹹�����ʽ
    Polyno(int max_exp,string iname="temp");                           //��ʹ�������Ĵ����������ʽ�����е�ϵ�����ᱻ��Ϊ0
    Polyno(const Polyno &other,string iname="temp");                   //�������캯��
    void expandExp(int newexp);                                        //��չ��ߴ����ĳ�Ա�������������ʽ��ǰ�������������ף�
    friend ostream & operator<< (ostream &out,const Polyno &target);   //�����������������ո�ʽ�������ʽ
    Polyno operator+ (Polyno other);                                   //���ؼӷ������������������ʽ���
    Polyno operator- (Polyno other);                                   //���ؼ�����������ñ�����ʽ��ȥ��һ������ʽ
    Polyno operator* (Polyno other);                                   //���س˷��������������ʽ�ͳ������
    Polyno operator* (const double coe);                               //���س˷��������������ʽ����һ������ʽ���
    Polyno &operator+= (Polyno other);                                 //�ӷ���ֵ�����
    Polyno &operator-= (Polyno other);                                 //������ֵ�����
    Polyno &operator*= (Polyno other);                                 //����ʽ�˷���ֵ�����
    Polyno &operator*= (const double coe);                             //�����˷���ֵ�����
    Polyno &operator= (Polyno other);                                  //������ֵ�����
    bool operator== (Polyno other);                                    //������ж�
    Polyno dev();                                                      //�Զ���ʽ������
    double solution(double x) const;                                   //�����Ա�����ֵ������ʽ��ֵ
    string getName() const;                                            //��øö���ʽ������
    private:
    int MaxExp;                                                        //����ʽ�������ߴ���
    vector<Term> pol;                                                  //����ʽ��ÿһ����������
    string name;                                                       //�ö���ʽ������
};


Polyno::Polyno(vector<double> nums,string iname)   //nums��Ҫ�����£������ܹ�����ż��������ż���±��Ϊϵ���������±��Ϊ����
{
    //������õ����ֹ��췽������nums�ڴ���������������
    //�ȹ������ʽ������
    name=iname;
    //�ټ����ṩ����������������ߴ���
    int max_size=0;
    for(int i=1;i<nums.size();i+=2)
    {
        max_size=(nums[i]>max_size)?nums[i]:max_size;
    }
    MaxExp=max_size;
    //���ڹ�����ߴ�����ͬ����ϵ��ȫ��Ϊ0�Ķ���ʽ
    for(int i=0;i<=MaxExp;i++)
    {
        pol.push_back(Term(0,i));   //����vector���±����Ĵ���һһ��Ӧ
    }
    //����ڸոյõ�����ʽ�еĲ��������ϵ����������̼����
    for(int i=0;i<nums.size();i+=2)
    {
        //����pol[nums[i+1]]���Ƕ�Ӧ��������
        pol[nums[i+1]].ChangeCoe(nums[i]);
    }
}

Polyno::Polyno(int max_exp,string iname)   //��ʹ����ߴ������������ʽ����ʱ�õ��Ķ���ʽ��ϵ����Ϊ0
{
    MaxExp=max_exp;
    name=iname;
    //�¹���Ķ���ʽ����ߴ����͸�������ߴ���һ�£�����ÿһ���ϵ����Ϊ0
    for(int i=0;i<=max_exp;i++)
    {
        pol.push_back(Term(0,i));      
    }
}

Polyno::Polyno(const Polyno &other,string iname)  //�������캯��
{
    pol=other.pol;
    MaxExp=other.MaxExp;
    //���ṩ������һ��������ʱʹ���µ����֣����򿽱�ԭ���������
    if(iname=="temp")
    {
    	name=other.name;
	}
	else
	{
		name=iname;
	}
}

void Polyno::expandExp(int newexp)   //�Զ���ʽ����ߴ�������չ
{
    //���ṩ����ߴ��������ڵ�ǰ����ʽ����ߴ���������ԭ����ʽ�����ı�
    if(newexp<=MaxExp)
    {
        return;
    }
    else
    {
        //����չ���ĸߴ���ϵ��ȫ����0
        for(int i=MaxExp+1;i<=newexp;i++)
        {
            pol.push_back(Term(0,i));
        }
    }
}

ostream & operator<< (ostream &out,const Polyno &target)   //������������
{
    //����Ĵ�����Ҫ���������¼������⣺
    //1.ϵ��Ϊ1��ʱ����ʾ1
    //2.ϵ��Ϊ����ʱ����ʾ"+"�����
    //3.����Ϊ1��ʱ����ʾ����
    string res;
    if(target.name!="temp")
    out<<target.name<<"=";
    for(int i=target.MaxExp;i>=0;i--)
    {
        //�ȹ������,ֻ�蹹��Ӻż���
        if((target.pol[i].GetCoe())>0&&i!=target.MaxExp)
        out<<"+";
        //�����ϵ��
        if(target.pol[i].GetCoe()!=0&&target.pol[i].GetCoe()!=1)
        out<<target.pol[i].GetCoe();
        if(target.pol[i].GetCoe()==1&&target.pol[i].GetExp()==0)
        out<<"1";
        //���������
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

Polyno Polyno::operator+ (Polyno other)   //���ؼӷ������
{
    //�ȼ������ʹ�õĶ���ʽ����ߴ���
    int MaxOfTwo=(MaxExp>other.MaxExp)?MaxExp:other.MaxExp;
    //���ڴ����㹻����ָ���Ľ��
    Polyno res(MaxOfTwo);
    //������չ��������ʽ����ͬϵ�����������������Ӱ�������������Ķ���ʽ����Ϊ������ʹ�õ���ֵ����
    (*this).expandExp(MaxOfTwo);
    other.expandExp(MaxOfTwo);
    //Ȼ�����������Ӽ��ɹ����ִ�мӷ���Ľ��
    for(int i=0;i<=MaxOfTwo;i++)
    {
        res.pol[i]=pol[i]+other.pol[i];
    }
    return res;
}

Polyno Polyno::operator- (Polyno other)
{
    //�ȼ������ʹ�õĶ���ʽ����ߴ���
    int MaxOfTwo=(MaxExp>other.MaxExp)?MaxExp:other.MaxExp;
    //�����ȴ����㹻����ָ���Ľ��
    Polyno res(MaxOfTwo);
    //������չ��������ʽ����ͬϵ�����������������Ӱ�������������Ķ���ʽ����Ϊ������ʹ�õ���ֵ����
    (*this).expandExp(MaxOfTwo);
    other.expandExp(MaxOfTwo);
    //Ȼ���������������ɹ����ִ�м�����Ľ��
    for(int i=0;i<=MaxOfTwo;i++)
    {
        res.pol[i]=pol[i]-other.pol[i];
    }
    return res;
}

Polyno Polyno::operator* (Polyno other)   //���س˷������
{
    //�ȼ�����˷��õ��Ľ���п��ܳ��ֵ���ߴ���
    int Max=MaxExp+other.MaxExp;
    //Ȼ�����㹻�ߴεĶ���ʽ��Ϊ���
    Polyno res(Max);
    //�������������������ϵ��
    for(int i=0;i<=MaxExp;i++)
    {
        for(int p=0;p<=other.MaxExp;p++)
        {
            //����������ʽ����������ˣ���˺��ϵ���ӵ��������ͬ���������ϵ���ϣ�������±�Ͷ�Ӧ����Ĵ�����ȣ���i+p������������˺��ϵ��
            res.pol[i+p].PlusExp(pol[i].GetCoe()*(other.pol[p].GetCoe()));
        }
    }
    return res;
}

Polyno Polyno::operator* (const double coe)  //���س˷������
{
    Polyno res(*this);
    for(int i=0;i<=res.MaxExp;i++)
    {
        res.pol[i]*=coe;    //������Գ�������ɶ���ʽ�ͳ������
    }
    return res;
}

Polyno &Polyno::operator+= (Polyno other)
{
    //����������ʵ�ֺ�+����
    int MaxOfTwo=(MaxExp>other.MaxExp)?MaxExp:other.MaxExp;
    //������չ��������ʽ����ͬϵ�����������������Ӱ�������������Ķ���ʽ����Ϊ������ʹ�õ���ֵ����
    (*this).expandExp(MaxOfTwo);
    other.expandExp(MaxOfTwo);
    //Ȼ��ֱ�����������ӣ�����֮���Կ���ֱ����������������Ϊ��֮ǰ�Ĳ�������������ʽ��û���κεĿ�ȱ����Լ�ʹ��1,3,5��2,4,6�����Ľ�������Ȼ�����������
    for(int i=0;i<=MaxOfTwo;i++)
    {
        pol[i]+=other.pol[i];
    }
    return (*this);
}

Polyno &Polyno::operator-= (Polyno other)
{
    //����������ʵ�ֺ�-����
    int MaxOfTwo=(MaxExp>other.MaxExp)?MaxExp:other.MaxExp;
    //������չ��������ʽ����ͬϵ�����������������Ӱ�������������Ķ���ʽ����Ϊ������ʹ�õ���ֵ����
    (*this).expandExp(MaxOfTwo);
    other.expandExp(MaxOfTwo);
    //Ȼ��ֱ������������������֮���Կ���ֱ����������������Ϊ��֮ǰ�Ĳ�������������ʽ��û���κεĿ�ȱ����Լ�ʹ��1,3,5��2,4,6�����Ľ�������Ȼ�����������
    for(int i=0;i<=MaxOfTwo;i++)
    {
        pol[i]-=other.pol[i];
    }
    return (*this);
}

Polyno &Polyno::operator*= (Polyno other)
{
    //����������ʵ�ֺ�*���������
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
    //����������ʵ�ֺ�*����
    for(int i=0;i<=MaxExp;i++)
    {
        pol[i]*=coe;
    }
    return (*this);
}

bool Polyno::operator== (Polyno other)    
{
    //�ȼ�����������ʽ����ߴ���
    int Max=(MaxExp>other.MaxExp)?MaxExp:other.MaxExp;
    //Ȼ��Ը���������ʽ������չ
    (*this).expandExp(Max);
    other.expandExp(Max);
    //����������бȶԼ���
    for(int i=0;i<=Max;i++)
    {
        if(pol[i].GetCoe()!=other.pol[i].GetCoe())
        return false;
    }
    return true;
}

Polyno &Polyno::operator= (Polyno other)  //������ֵ�����
{
	name=other.name;
	pol=other.pol;
	MaxExp=other.MaxExp;
}

Polyno Polyno::dev()    //����ʽ�󵼳�Ա����
{ 
    //�ȴ�����Ϊ����ֵ�Ľ���Ķ���ʽ���������ʽ����ߴ����ȵ�ǰ����ߴ�����1
    Polyno res(MaxExp-1);
    //Ȼ���������Ϊ����ֵ�Ľ���Ķ���ʽ������
    for(int i=1;i<=MaxExp;i++)  //����Ӵ���1��ʼ�������ɣ���Ϊ��������Ϊ0
    {
        res.pol[i-1].ChangeCoe(pol[i].GetCoe()*i);
    }
    return res;
}

double Polyno::solution(double x) const   //����δ֪���ĸ���ֵ
{
    double res=0;
    for(int i=0;i<=MaxExp;i++)
    {
        res+=(pol[i].GetCoe())*pow(x,i);  //ֱ��������м�������Ӽ���
    }
    return res;
}

string Polyno::getName() const    //��øö���ʽ������
{
    return name;   //δ�ṩ���ֵĶ���ʽͳһ����Ϊtemp
}

#endif 
