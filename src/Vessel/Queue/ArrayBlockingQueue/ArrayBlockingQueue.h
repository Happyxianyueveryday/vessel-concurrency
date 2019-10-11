#define ARRAY_BLOCKING_QUEUE_H             
#define ARRAY_BLOCKING_QUEUE_H       
#include "BlockingQueue.h"
#include <iostream>  
#include <string>
#include <mutex>
#include <condition_variable>

/**
 * ArrayBlockingQueue: ���ڻ��������̰߳�ȫ�������
 * note: ��Ϊ���еĵײ�ʵ�ֻ���ѭ�����飬��ʹ���˿ճ�һ�����鵥Ԫ��ʹ�õĲ��������ֶ��������Ƕ��пգ���� 
 */
template<typename T>
class ArrayBlockingQueue: public BlockingQueue<T>
{
    public:   
    // 1. public constructor 
    static ArrayBlockingQueue<T> *ArrayQueueFactory(int max_size);    // ���캯��

    // 2. copy_controller
    ArrayBlockingQueue(const ArrayBlockingQueue<T> &other);                             // �������캯��
    ArrayBlockingQueue<T> & operator = (const ArrayBlockingQueue<T> &other) = delete;   // ������ֵ�����
    ArrayBlockingQueue(ArrayBlockingQueue<T> &&other);                        		    // �ƶ����캯��
	ArrayBlockingQueue<T> &operator= (ArrayBlockingQueue<T> &&other) = delete;          // �ƶ���ֵ�����  

    // 3. methods
    void push(T val) override;            // ������Ӳ���
    void pop(T &val) override;            // ���г��Ӳ���
    void frontval(T &val) override;       // ��ȡ����Ԫ��
    int length() override;                // ������г��ȣ��������е�Ԫ�ظ���
    bool full() override;                 // �ж϶����Ƿ�Ϊ�� 
    bool empty() override;                // �ж϶����Ƿ�Ϊ�� 

    // 4. destructor
    ~ArrayBlockingQueue();

	private:
    int max_size;    // ��ǰ���д洢����Ĵ�С
    T *data;         // ջ����洢
    // �ж϶����Ƿ�Ϊ�յķ���������ʹ�û����������洢����Ԫ�أ����ҷ�������һ����Ԫ���������ֶӿպͶ������������������˵�front==end��������Ϊ�գ���(end+1)%max_size==beginʱ��������Ϊ��
    int front;       // ����Ԫ���±�
    int end;         // ��βԪ�ص�β��Ԫ�ص��±�
    
    std::recursive_mutex mut;       // ����ͬ�����ƺͲ����Ļ����� 
    std::condition_variable_any push_var;    // ����̵߳��������� 
    std::condition_variable_any pop_var;     // �����̵߳��������� 
    
    // 5. private constructor                     
    ArrayBlockingQueue(int _max_size); 
};

template<typename T>
ArrayBlockingQueue<T> *ArrayBlockingQueue<T>::ArrayQueueFactory(int _max_size) 
{
    if(_max_size<0)
    return NULL;
    else
    return new ArrayBlockingQueue<T>(_max_size+1);    // �����ܹ����_max_size��Ԫ�صĶ��У���Ӧ�Ĵ洢���鳤����Ҫ��_max_size+1
}

template<typename T>
ArrayBlockingQueue<T>::ArrayBlockingQueue(int _max_size)
{ 
    max_size=_max_size;
    data=new T [max_size];
    front=0;   // ��ʼ�Ķ���Ԫ���±�Ϊ0
    end=0;     // ��ʼ�Ķ�βԪ�ص�β��Ԫ���±�Ϊ0
}

template<typename T>
ArrayBlockingQueue<T>::ArrayBlockingQueue(const ArrayBlockingQueue<T> &other)
{
	std::lock_guard<std::mutex> other_guard(other.mut);
    max_size=other.max_size;
    data=new T [max_size];
    front=other.front;
    end=other.end;

    for(int i=0;i<max_size;i++)
    {
        data[i]=other.data[i];
    }
}

template<typename T>
ArrayBlockingQueue<T>::ArrayBlockingQueue(ArrayBlockingQueue<T> &&other)
{
	std::lock_guard<std::mutex> other_guard(other.mut);
	max_size=other.max_size;
    data=other.data;
    front=other.front;
    end=other.end;

    other.data=new T [max_size];
    other.front=0;   // ��ʼ�Ķ���Ԫ���±�Ϊ0
    other.end=0;     // ��ʼ�Ķ�βԪ�ص�β��Ԫ���±�Ϊ0
}

template<typename T>
void ArrayBlockingQueue<T>::push(T val) 
{
	
    std::unique_lock<std::recursive_mutex> ulock(mut);
    push_var.wait(ulock, [this]() {return !full();});   // �����������������ulock��������Ȩ�ȴ� 
    data[end]=val; 
    end=(end+1)%max_size; 
    pop_var.notify_one();    // ��ӽ�����֪ͨ���ڵȴ����ӵ��߳̽��г��� 
}

template<typename T>
void ArrayBlockingQueue<T>::pop(T &val)
{
	std::unique_lock<std::recursive_mutex> ulock(mut);
	pop_var.wait(ulock, [this]() {return !empty();});
    val=data[front];
    front=(front+1)%max_size;
    push_var.notify_one();
}

template<typename T>
void ArrayBlockingQueue<T>::frontval(T &val) 
{
	std::unique_lock<std::recursive_mutex> ulock(mut);
	pop_var.wait(ulock, [this]() {return !empty();});
    val=data[front];
}

template<typename T>
int ArrayBlockingQueue<T>::length()
{
	std::lock_guard<std::recursive_mutex> guard(mut);      
    // ���������Ԫ�ظ����ı�׼��ʽ���ǣ�(end-front+max_size)%max_size���ټ����������ӿ����Ƶ��õ� 
    return (end-front+max_size)%max_size;
}

template<typename T>
bool ArrayBlockingQueue<T>::full() 
{
	//frontָ����ף�endָ���β��β��Ԫ�أ����end����һ��Ԫ�ؾ���frontʱ����(end+1)%max_size==frontʱ����
	std::lock_guard<std::recursive_mutex> guard(mut); 
	if((end+1)%max_size==front)
	return true;
	else
	return false;
}

template<typename T>
bool ArrayBlockingQueue<T>::empty()
{
	// ������β��Ԫ���±�Ͷ���Ԫ���±���ȣ�������Ѿ��� 
	std::lock_guard<std::recursive_mutex> guard(mut);
	if(front==end) 
	return true;
	else
	return false; 
}

template<typename T>
ArrayBlockingQueue<T>::~ArrayBlockingQueue()
{
    delete [] data;
}
