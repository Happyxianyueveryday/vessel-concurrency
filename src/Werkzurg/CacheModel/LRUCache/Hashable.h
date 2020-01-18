#ifndef TEST_ELEMENT_H                 
#define TEST_ELEMENT_H  
#include "Hashable.h"     
#include <string>    
#include <iostream>

/**
 * Hashable: ������ʹ�õĹ�ϣ����Ļ��� 
 * note 1: ���������ṩ��һ��Ӧ����μ̳�Hashable����д���еķ�������֧��ʹ��HashMap��ZipHashMap�� 
 * note 2: ��ϣ���еļ�����֧�ֹ�ϣ�ͱȽϣ����ṩhashCode()������equals()����������ֵ���Բ�֧�ֹ�ϣ��������֧�ֱȽϷ��������ṩequals()������ 
 */
class Hashable
{
	public:
	Hashable();                                              // Ĭ�Ϲ��캯��  
	Hashable(int _val);                              		 // ���캯�� 
    Hashable(const Hashable &other);                         // �������캯�� 
	virtual Hashable& operator= (const Hashable &other);     // ������ֵ����� 
	virtual unsigned int hashCode() const;                   // ���㵱ǰԪ�صĹ�ϣֵ 
	virtual bool equals(const Hashable &other) const;        // �Ƚ��������� 
    virtual std::string toString() const;                         // �ַ��������ǰԪ�� 
    virtual int value() const;                               // ��ȡ��ϣ����ʵ����ֵ 
    virtual ~Hashable();                                     // ���������� 
    
    private: 
    int val;    // ʵ�ʴ洢��ֵ������Ԫ������ʵ���Ͼ��Ǽ򵥷�װ��һ��intֵ 
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
	if(val==other.val)// ���dynamic_cast�ɹ��������ߵ�ֵ��ȣ�������������ȣ�����true 
	return true;
	else                               // ���������������������� 
	return false;
}

std::string Hashable::toString() const
{
	return std::to_string(val);
}

int Hashable::value() const
{
	return val;
}

Hashable::~Hashable() 
{
	//cout<<"Hashable����������������"<<endl; 
}                                 

#endif
