#ifndef CONCURRENT_ARRAY_LIST_H                
#define CONCURRENT_ARRAY_LIST_H   
#include "ConcurrentStack.h"  
#include "rwmutex.h"
#include "wrmutex.h"
#include "fair_rwmutex.h"
#include <iostream>
#include <string>   
#include <mutex>    

/**
 * ConcurrentArrayStack: �̰߳�ȫ������ջ����д���汾�� 
 */
template<typename T>
class ConcurrentArrayStack: public ConcurrentStack<T>
{
    public:   
    // 1. public constructor
    ConcurrentArrayStack(int _max_size, std::string mode);

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
    
    private:
    int size;         // ��ǰջ��Ԫ�ظ���
    int max_size;     // ��ǰջ����������Ԫ�ظ���
    T *data;          // ջ����洢
    int pos;          // ջ��ָ��λ��
    std::string mode;      // ʹ�õĶ�д��ģʽ 
    rwmutex rwmut;    // �����ȵĶ�д�� 
    wrmutex wrmut;    // д���ȵĶ�д�� 
    fair_rwmutex fair_rwmut;  // ��д��ƽ�Ķ�д�� 
    
    // 5. private methods
    void set_lock(std::string lockmode);
};

template<typename T>
ConcurrentArrayStack<T>::ConcurrentArrayStack(int _max_size, std::string _mode)
{   
    size=0;  // ���캯���е�ǰ����ĳ�Ա��δ����ʼ����������������ǰ�������� 
    max_size=(_max_size>=0)?_max_size:10;
    data=new T [max_size];
    pos=-1;
    if(_mode=="read-first"||_mode=="write-first"||_mode=="fair")
    mode=_mode;
    else
    mode="read-first";
}

template<typename T>
ConcurrentArrayStack<T>::ConcurrentArrayStack(const ConcurrentArrayStack<T> &other)
{      
	other.set_lock("write_lock"); // ����Դд�����������������캯���е�ǰ����ĳ�Ա��δ����ʼ����������������ǰ��������  
	
    size=other.size;
    max_size=other.max_size;
    data=new T [max_size];
    pos=other.pos;
    mode=other.mode;

    for(int i=0;i<pos;i++)
    {
        data[i]=other.data[i];
    }
    
    other.set_lock("write_unlock");  // ����Դд�������� 
}

template<typename T>
ConcurrentArrayStack<T>::ConcurrentArrayStack(ConcurrentArrayStack &&other)
{	
	other.set_lock("write_lock"); // ����Դд�������� 
	
	size=other.size;
    max_size=other.max_size;
    data=other.data;
    pos=other.pos;
    
    other.size=0;
    other.data=new T [other.max_size];
    other.pos=-1;
    
    other.set_lock("write_unlock"); // ����Դд�������� 
}

template<typename T>
bool ConcurrentArrayStack<T>::push(T val)  
{
	set_lock("write_lock");        // д�������� 
	
    if(size>=max_size)    // ����ж���������������������ж���Ϻ������߳��ֽ�������ջ/��ջ�������õ��Ľ����׼ȷ 
    {
    	set_lock("write_unlock");  // д�������� 
    	return false;
	}
    else                  
    { 
        pos+=1;
        data[pos]=val;
        size+=1;
        set_lock("write_unlock");  // д�������� 
        return true;
    }
}

template<typename T>
bool ConcurrentArrayStack<T>::pop(T &val)
{
	set_lock("write_lock");       // д��������
	
    if(size==0)  	
	{
		set_lock("write_unlock");       // д�������� 
		return false;
	}	
    else               
    { 
        val=data[pos];
        pos-=1;
        size-=1;
        set_lock("write_unlock");       // д�������� 
        return true;
    }
}

template<typename T>
bool ConcurrentArrayStack<T>::top(T &val)
{
	set_lock("read_lock");    // ����������  
    if(size==0) 
    {
    	set_lock("read_unlock");  // ���������� 
    	return false;
	}
    else
    { 
    	val=data[pos];
    	set_lock("read_unlock");  // ���������� 
    	return true;
 	}
}

template<typename T>
int ConcurrentArrayStack<T>::length()
{
	set_lock("read_lock");    // ����������  
	int res=size;
	set_lock("read_unlock");  // ���������� 
    return res;
}

template<typename T>
void ConcurrentArrayStack<T>::set_lock(std::string lockmode)
{
	if(lockmode=="read_lock")
	{
		if(mode=="read-first")
		rwmut.read_lock();
		else if(mode=="write-first")
		wrmut.read_lock();
		else 
		fair_rwmut.read_lock();
	}
	else if(lockmode=="read_unlock")
	{
		if(mode=="read-first")
		rwmut.read_unlock();
		else if(mode=="write-first")
		wrmut.read_unlock();
		else 
		fair_rwmut.read_unlock();
	}
	else if(lockmode=="write_lock")
	{
		if(mode=="read-first")
		rwmut.write_lock();
		else if(mode=="write-first")
		wrmut.write_lock();
		else 
		fair_rwmut.write_lock();
	}
	else if(lockmode=="write_unlock")
	{
		if(mode=="read-first")
		rwmut.write_unlock();
		else if(mode=="write-first")
		wrmut.write_unlock();
		else 
		fair_rwmut.write_unlock();
	}
}

template<typename T>
ConcurrentArrayStack<T>::~ConcurrentArrayStack()
{
	set_lock("write_lock");
    delete data;
    set_lock("write_unlock");
}

#endif
