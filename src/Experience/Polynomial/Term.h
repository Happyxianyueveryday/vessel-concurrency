#ifndef TERM_H  
#define TERM_H

class Term 
{
    public:
    Term(double coe,double exp);           //���캯�����ø�����ϵ�����ߴ���������
    Term();                                //Ĭ�Ϲ��캯����Ĭ�Ϲ��캯����������ϵ���ʹ�����Ϊ0
    void ChangeCoe(const double num);      //�ı����ϵ��
    void ChangeExp(const double num);      //�ı���Ĵ���
    void PlusExp(const double add);        //����/�������ϵ��
    double GetCoe() const;                 //������ϵ��
    double GetExp() const;                 //�����Ĵ���
    void ResetTerm();                      //�������ϵ���ʹ���
    Term operator+ (const Term &other);    //��ͬ�����������
    Term operator- (const Term &other);    //��ͬ�����������
    Term operator* (const Term &other);    //��ͬ�����������
    Term operator* (const double coe);     //���볣�����
    Term &operator+= (const Term &other);  //��ͬ����������Ӳ���ֵ
    Term &operator-= (const Term &other);  //��ͬ���������������ֵ
    Term &operator*= (const Term &other);  //��ͬ����������˲���ֵ
    Term &operator*= (const double coe);   //���볣����˲���ֵ
    Term &operator= (const Term &other);   //������ֵ�����
    bool operator< (const Term &other);    //����Ĵ�����С���бȽ�
    bool operator== (const Term &other);   //�Ƚ��������Ƿ����
    Term Dev();                            //�Ե�ǰ�������
    private:
    double Coe;                            //��ǰ���ϵ��
    double Exp;                            //��ǰ��Ĵ���
};

Term::Term(double coe,double exp):Coe(coe),Exp(exp)
{
}

Term::Term():Coe(0),Exp(0)
{  
}

void Term::ChangeCoe(const double num)   //�ı����ϵ��
{
    Coe=num;
}

void Term::ChangeExp(const double num)   //�ı���Ĵ���
{
    Exp=num;
}

void Term::PlusExp(const double add)   //����/�������ϵ��
{
    Coe+=add;
}

double Term::GetCoe() const   //������ϵ��
{
    return Coe;
}

double Term::GetExp() const   //�����Ĵ���
{
    return Exp;
}

void Term::ResetTerm()        //�������ϵ���ʹ���
{
    Coe=0;
    Exp=0;
}

Term Term::operator+ (const Term &other)    //��ͬ�����������
{
    return Term(Coe+other.Coe,Exp);
}

Term Term::operator- (const Term &other)    //��ͬ�����������
{
    return Term(Coe-other.Coe,Exp);
}

Term Term::operator* (const Term &other)    //��ͬ�����������
{
    return Term(Coe*(other.Coe),Exp+other.Exp);
}

Term Term::operator* (const double coe)     //���볣�����
{
    return Term(Coe*coe,Exp);
}

Term &Term::operator+= (const Term &other)  //��ͬ����������Ӳ���ֵ
{
    Coe+=other.Coe;
    return (*this);
}

Term &Term::operator-= (const Term &other)  //��ͬ���������������ֵ
{
    Coe-=other.Coe;
    return (*this);
}

Term &Term::operator*= (const Term &other)  //��ͬ����������˲���ֵ
{
    Coe*=other.Coe;
    Exp+=other.Exp;
    return (*this);
}

Term &Term::operator*= (const double coe)   //���볣����˲���ֵ
{
    Coe*=coe;
    return (*this);
}

bool Term::operator< (const Term &other)    //����Ĵ�����С���бȽ�
{
    return (Exp<other.Exp);          
}

bool Term::operator== (const Term &other)   //�Ƚ��������Ƿ����
{
    return (Exp==other.Exp)&&(Coe==other.Coe);
}

Term &Term::operator= (const Term &other)   //������ֵ�����
{
    Coe=other.Coe;
    Exp=other.Exp; 
    return (*this);
}

Term Term::Dev()                            //�Ե�ǰ�������
{
    return Term(Coe*Exp,Exp-1);
}

#endif
