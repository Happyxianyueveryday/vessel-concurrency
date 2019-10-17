#ifndef CONCURRENT_MAP_H     
#define CONCURRENT_MAP_H 
#include <string> 
#include <vector>         

/** 
 * ConcurrentMap: ��ϣ��ӿڳ������  
 * typename T: ��ϣ���������
 * typename R: ��ϣ��ֵ������ 
 */
template <typename T, typename R>
class ConcurrentMap
{    
	public:    
	virtual void insert(T key, R value) = 0;        // ���ϣ���в���ָ���ļ�ֵ�ԣ�����ϣ�����Ѿ����ڸü�������¸ü���Ӧ��ֵΪ��ֵ 
	virtual void remove(T key) = 0;                 // �ӹ�ϣ�����Ƴ�ָ�����ļ�ֵ�� 
	virtual bool contain(T key) = 0;          // �жϹ�ϣ�����Ƿ����ָ����   
	virtual bool get(T key, R &value) = 0;    // ���ҹ�ϣ���и�������Ӧ��ֵ  
	virtual std::vector<T> keys() = 0;             // ��ù�ϣ�������м����б�  
	virtual int length() = 0;                 // ��ȡ��ϣ��ĳ��� 
	virtual std::string toString() = 0;            // �����ϣ��    
	virtual ~ConcurrentMap() {}                     // ��ϣ������������������
};

#endif
