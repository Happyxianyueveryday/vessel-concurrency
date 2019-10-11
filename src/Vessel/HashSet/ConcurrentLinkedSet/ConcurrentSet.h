#ifndef CONCURRENTSET_H                 
#define CONCURRENTSET_H         
#include <string>        

template<typename T> 
class ConcurrentSet
{
	public:
	virtual bool contain(const T &val) = 0;     // �жϸ���Ԫ���Ƿ��ڼ�����   
    virtual bool add(const T &val) = 0;         // �򼯺�������ָ��Ԫ�� 
    virtual bool remove(const T &val) = 0;      // ����ָ��λ��ɾ��Ԫ��
    virtual int length() = 0;                   // ��ȡ���ϵĴ�С 
    virtual std::string toString() = 0;              // �����ǰ���� 
	virtual ~ConcurrentSet() {};                // ����������  
};

#endif 
