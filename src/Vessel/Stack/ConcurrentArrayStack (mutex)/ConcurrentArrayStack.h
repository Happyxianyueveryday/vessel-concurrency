#ifndef CONCURRENT_ARRAY_LIST_H          
#define CONCURRENT_ARRAY_LIST_H        
#include <iostream>
#include <string>
#include "ConcurrentStack.h"      
#include <mutex>   

/**
 * ConcurrentArrayStack: �̰߳�ȫ������ջ���������汾��  
 */
template<typename T> 
class ConcurrentArrayStack: public ConcurrentStack<T>
{
    public:   
    // 1. public constructor
    ConcurrentArrayStack(int _max_size);

    // 2. copy/move_controller
    ConcurrentArrayStack(const ConcurrentArrayStack<T> &other);                           // �������캯�� 
    ConcurrentArrayStack<T> &operator= (const ConcurrentArrayStack<T> &other) = delete;   // ���ÿ�����ֵ����� 
    ConcurrentArrayStack(ConcurrentArrayStack<T> &&other);                                // �ƶ����캯�� 
    ConcurrentArrayStack<T> &operator= (ConcurrentArrayStack<T> &&other) = delete;        // �����ƶ���ֵ����� 

    // 3. methods
    bool push(T val);       // ��ջ�������ò������̰߳�ȫ�� 
    bool pop(T &val);		// ��ջ�������ò������̰߳�ȫ�� 
    bool top(T &val);       // ��ȡջ��Ԫ�ز������ò����̰߳�ȫ�����ǽ�����ȡ�õ�˲������Ч�ģ���һ��ʱ������ջ���޸ģ���ʱȡ�õ�ջ��Ԫ�ؾͲ��Ǵ˿̵�ջ��Ԫ���ˡ���˱��뽫ȡջ��������ʹ��ջ��Ԫ�ز����ϲ�Ϊһ��ԭ�Ӳ��� 
    int length();           // ����ջ�е�Ԫ���������ò����̰߳�ȫ�����ǽ�����ȡ�õ�˲������Ч�ģ���˱��뽫����ջԪ������������ʹ��Ԫ�������Ĵ���ϲ�Ϊһ��ԭ�Ӳ���  

    // 4. destructor
    ~ConcurrentArrayStack();

    int size;         // ��ǰջ��Ԫ�ظ���
    int max_size;     // ��ǰջ����������Ԫ�ظ���
    T *data;          // ջ����洢
    int pos;          // ջ��ָ��λ��
    std::mutex mut;        // ��������ź��� 
};

/**
 * ConcurrentArrayStack: ���캯������ʼ��ջ 
 * note: �������_max_size���Ϸ�����Ĭ�ϴ�������СΪ10��ջ 
 * note: ���캯������ʱ����ĳ�Ա��δ��ʼ��������Ҫ���������� 
 */
template<typename T>
ConcurrentArrayStack<T>::ConcurrentArrayStack(int _max_size)
{    
    size=0;
    max_size=(_max_size>=0)?_max_size:10;  
    data=new T [max_size]; 
    pos=-1; 
}

/**
 * ConcurrentArrayStack: �������캯�� 
 * note: �������������뻥�⣬���ʹ������ź�����Ա����һ��lock_guard����֤���⣬���ⴴ������Դ�����һ��lock_guard����֤���� 
 */
template<typename T>
ConcurrentArrayStack<T>::ConcurrentArrayStack(const ConcurrentArrayStack<T> &other)
{
	std::lock_guard<std::mutex> other_guard(other.mut);    // �������캯��������ʱ��ǰ����ĳ�Ա��δ��ʼ����ʹ�ã����ֻ��Ҫ��������Դ�Ļ����� 
    size=other.size;
    max_size=other.max_size;
    data=new T [max_size];
    pos=other.pos;

    for(int i=0;i<pos;i++)
    {
        data[i]=other.data[i];
    }
}

/**
 * ConcurrentArrayStack: �ƶ����캯�� 
 * note: �������������뻥�⣬���ʹ������ź�����Ա����һ��lock_guard����֤���� 
 */
template<typename T>
ConcurrentArrayStack<T>::ConcurrentArrayStack(ConcurrentArrayStack &&other)
{
	std::lock_guard<std::mutex> other_guard(other.mut);    //  �ƶ����캯��������ʱ��ǰ����ĳ�Ա��δ��ʼ����ʹ�ã����ֻ��Ҫ��������Դ�Ļ����� 
	size=other.size;
    max_size=other.max_size;
    data=other.data;
    pos=other.pos;
    
    other.size=0;
    other.data=new T [other.max_size];
    other.pos=-1;
}

/** 
 * push: ��ջ����ջһ��Ԫ��
 * note: ��ջ��ȫ�����̱��뻥��ؽ��� 
 */
template<typename T>
bool ConcurrentArrayStack<T>::push(T val)  
{
	std::lock_guard<std::mutex> guard(mut);    // ������ջ�������뻥����У���ʹ���ж���������Ҳһ�� 
    if(size>=max_size)    // ����ж���������������������ж���Ϻ������߳��ֽ�������ջ/��ջ�������õ��Ľ����׼ȷ 
    return false;
    else                  
    { 
        pos+=1;
        data[pos]=val;
        size+=1;
        return true;
    }
}

template<typename T>
bool ConcurrentArrayStack<T>::pop(T &val)
{
	std::lock_guard<std::mutex> guard(mut); 
    if(size==0)  		
    return false;
    else               
    { 
        val=data[pos];
        pos-=1;
        size-=1;
        return true;
    }
}

template<typename T>
bool ConcurrentArrayStack<T>::top(T &val)
{
	std::lock_guard<std::mutex> guard(mut);   
    if(size==0)  
    return false;
    else
    { 
    	val=data[pos];
    	return true;
	}
}

template<typename T>
int ConcurrentArrayStack<T>::length()
{
	std::lock_guard<std::mutex> guard(mut);   
    return size;
}

template<typename T>
ConcurrentArrayStack<T>::~ConcurrentArrayStack()
{
    delete data;
}

#endif
