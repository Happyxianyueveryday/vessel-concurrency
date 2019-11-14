#ifndef HASH_MAP_H    
#define HASH_MAP_H  
#include <iostream>    
#include <string>  
#include <vector>         

/** 
 * HashMap: ��ϣ��ӿڳ������  
 * typename T: ��ϣ���������
 * typename R: ��ϣ��ֵ������ 
 */
template <typename T, typename R>
class HashMap
{    
	public:    
	virtual void insert(T key, R value) = 0;        // ���ϣ���в���ָ���ļ�ֵ�ԣ�����ϣ�����Ѿ����ڸü�������¸ü���Ӧ��ֵΪ��ֵ 
	virtual void remove(T key) = 0;                 // �ӹ�ϣ�����Ƴ�ָ�����ļ�ֵ�� 
	virtual bool contain(T key) const = 0;          // �жϹ�ϣ�����Ƿ����ָ����   
	virtual bool get(T key, R &value) const = 0;    // ���ҹ�ϣ���и�������Ӧ��ֵ  
	virtual std::vector<T> keys() const = 0;             // ��ù�ϣ�������м����б�  
	virtual int length() const = 0;                 // ��ȡ��ϣ��ĳ��� 
	virtual ~HashMap() {}                           // ��ϣ������������������
};

#endif
