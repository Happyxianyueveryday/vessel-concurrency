#ifndef TERM_H  
#define TERM_H

class Term 
{
    public:
    Term(double coe,double exp);           //构造函数：用给定的系数或者次数构造项
    Term();                                //默认构造函数：默认构造函数构造的项的系数和次数都为0
    void ChangeCoe(const double num);      //改变项的系数
    void ChangeExp(const double num);      //改变项的次数
    void PlusExp(const double add);        //增加/减少项的系数
    double GetCoe() const;                 //获得项的系数
    double GetExp() const;                 //获得项的次数
    void ResetTerm();                      //重设项的系数和次数
    Term operator+ (const Term &other);    //相同次数的项相加
    Term operator- (const Term &other);    //相同次数的项相减
    Term operator* (const Term &other);    //相同次数的项相乘
    Term operator* (const double coe);     //项与常数相乘
    Term &operator+= (const Term &other);  //相同次数的项相加并赋值
    Term &operator-= (const Term &other);  //相同次数的项相减并赋值
    Term &operator*= (const Term &other);  //相同次数的项相乘并赋值
    Term &operator*= (const double coe);   //项与常数相乘并赋值
    Term &operator= (const Term &other);   //拷贝赋值运算符
    bool operator< (const Term &other);    //对项的次数大小进行比较
    bool operator== (const Term &other);   //比较两个项是否相等
    Term Dev();                            //对当前项进行求导
    private:
    double Coe;                            //当前项的系数
    double Exp;                            //当前项的次数
};

Term::Term(double coe,double exp):Coe(coe),Exp(exp)
{
}

Term::Term():Coe(0),Exp(0)
{  
}

void Term::ChangeCoe(const double num)   //改变项的系数
{
    Coe=num;
}

void Term::ChangeExp(const double num)   //改变项的次数
{
    Exp=num;
}

void Term::PlusExp(const double add)   //增加/减少项的系数
{
    Coe+=add;
}

double Term::GetCoe() const   //获得项的系数
{
    return Coe;
}

double Term::GetExp() const   //获得项的次数
{
    return Exp;
}

void Term::ResetTerm()        //重设项的系数和次数
{
    Coe=0;
    Exp=0;
}

Term Term::operator+ (const Term &other)    //相同次数的项相加
{
    return Term(Coe+other.Coe,Exp);
}

Term Term::operator- (const Term &other)    //相同次数的项相减
{
    return Term(Coe-other.Coe,Exp);
}

Term Term::operator* (const Term &other)    //相同次数的项相乘
{
    return Term(Coe*(other.Coe),Exp+other.Exp);
}

Term Term::operator* (const double coe)     //项与常数相乘
{
    return Term(Coe*coe,Exp);
}

Term &Term::operator+= (const Term &other)  //相同次数的项相加并赋值
{
    Coe+=other.Coe;
    return (*this);
}

Term &Term::operator-= (const Term &other)  //相同次数的项相减并赋值
{
    Coe-=other.Coe;
    return (*this);
}

Term &Term::operator*= (const Term &other)  //相同次数的项相乘并赋值
{
    Coe*=other.Coe;
    Exp+=other.Exp;
    return (*this);
}

Term &Term::operator*= (const double coe)   //项与常数相乘并赋值
{
    Coe*=coe;
    return (*this);
}

bool Term::operator< (const Term &other)    //对项的次数大小进行比较
{
    return (Exp<other.Exp);          
}

bool Term::operator== (const Term &other)   //比较两个项是否相等
{
    return (Exp==other.Exp)&&(Coe==other.Coe);
}

Term &Term::operator= (const Term &other)   //拷贝赋值运算符
{
    Coe=other.Coe;
    Exp=other.Exp; 
    return (*this);
}

Term Term::Dev()                            //对当前项进行求导
{
    return Term(Coe*Exp,Exp-1);
}

#endif
