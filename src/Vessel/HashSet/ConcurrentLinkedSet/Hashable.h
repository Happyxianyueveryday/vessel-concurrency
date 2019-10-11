#ifndef HASHABLE_H          
#define HASHABLE_H      
#include "Hashable.h"     
#include <string>  
#include <iostream>       
 
/**
 * Hashable: ������ʹ�õĹ�ϣ��Ԫ������ 
 * note: ���������ṩ��һ��Ӧ����μ̳�Hashable����д���еķ�������֧��ʹ��HashSet��ZipHashSet�� 
 */
class Hashable
{ 
	public:
	Hashable();                                              // Ĭ�Ϲ��캯��  
	Hashable(int _val);                              		 // ���캯�� 
	Hashable(const Hashable &other);                         // �������캯�� 
	Hashable& operator= (const Hashable &other);             // ������ֵ����� 
	int hashCode() const;                                    // ���㵱ǰԪ�صĹ�ϣֵ 
	bool equals(const Hashable &other) const;                // �Ƚ��������� 
    std::string toString() const;                                 // �ַ��������ǰԪ�� 
    ~Hashable();                                             // ����������
    
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

int Hashable::hashCode() const
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

Hashable::~Hashable() 
{
	//cout<<"Hashable����������������"<<endl; 
}                                 

#endif
