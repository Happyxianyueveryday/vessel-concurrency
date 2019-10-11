#define CONCURRENT_ARRAY_QUEUE_H               
#define CONCURRENT_ARRAY_QUEUE_H       
#include "ConcurrentQueue.h"  
#include <string>
#include <mutex>

/**
 * ConcurrentArrayQueue: ���ڻ��������̰߳�ȫ�������
 * note: ��Ϊ���еĵײ�ʵ�ֻ���ѭ�����飬��ʹ���˿ճ�һ�����鵥Ԫ��ʹ�õĲ��������ֶ��������Ƕ��пգ���� 
 */
template<typename T>
class ConcurrentArrayQueue: public ConcurrentQueue<T>
{
    public:   
    // 1. public constructor 
    static ConcurrentArrayQueue<T> *ArrayQueueFactory(int max_size);    // ���캯��

    // 2. copy_controller
    ConcurrentArrayQueue(const ConcurrentArrayQueue<T> &other);                             // �������캯��
    ConcurrentArrayQueue<T> & operator = (const ConcurrentArrayQueue<T> &other) = delete;   // ������ֵ�����
    ConcurrentArrayQueue(ConcurrentArrayQueue<T> &&other);                        		    // �ƶ����캯��
	ConcurrentArrayQueue<T> &operator= (ConcurrentArrayQueue<T> &&other) = delete;          // �ƶ���ֵ����� 

    // 3. methods
    bool push(T val) override;            // ������Ӳ���
    bool pop(T &val) override;            // ���г��Ӳ���
    bool frontval(T &val) override;       // ��ȡ����Ԫ��
    int length() override;                // ������г��ȣ��������е�Ԫ�ظ���

    // 4. destructor
    ~ConcurrentArrayQueue();

	private:
    int max_size;    // ��ǰ���д洢����Ĵ�С
    T *data;         // ջ����洢
    // �ж϶����Ƿ�Ϊ�յķ���������ʹ�û����������洢����Ԫ�أ����ҷ�������һ����Ԫ���������ֶӿպͶ������������������˵�front==end��������Ϊ�գ���(end+1)%max_size==beginʱ��������Ϊ��
    int front;       // ����Ԫ���±�
    int end;         // ��βԪ�ص�β��Ԫ�ص��±�
    std::mutex mut;       // �������� 
    
    // 5. private constructor                     
    ConcurrentArrayQueue(int _max_size); 
};

template<typename T>
ConcurrentArrayQueue<T> *ConcurrentArrayQueue<T>::ArrayQueueFactory(int _max_size) 
{
    if(_max_size<0)
    return NULL;
    else
    return new ConcurrentArrayQueue<T>(_max_size+1);    // �����ܹ����_max_size��Ԫ�صĶ��У���Ӧ�Ĵ洢���鳤����Ҫ��_max_size+1
}

template<typename T>
ConcurrentArrayQueue<T>::ConcurrentArrayQueue(int _max_size)
{ 
    max_size=_max_size;
    data=new T [max_size];
    front=0;   // ��ʼ�Ķ���Ԫ���±�Ϊ0
    end=0;     // ��ʼ�Ķ�βԪ�ص�β��Ԫ���±�Ϊ0
}

template<typename T>
ConcurrentArrayQueue<T>::ConcurrentArrayQueue(const ConcurrentArrayQueue<T> &other)
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
ConcurrentArrayQueue<T>::ConcurrentArrayQueue(ConcurrentArrayQueue<T> &&other)
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
bool ConcurrentArrayQueue<T>::push(T val) 
{
	std::lock_guard<std::mutex> guard(mut);
    if((end+1)%max_size==front)    // ���ǰ��Ҫ�����ж϶����Ƿ�������frontָ����ף�endָ���β��β��Ԫ�أ����end����һ��Ԫ�ؾ���frontʱ����(end+1)%max_size==frontʱ����
    return false;
    else
    {
        data[end]=val;
        end=(end+1)%max_size;
        return true;
    }
}

template<typename T>
bool ConcurrentArrayQueue<T>::pop(T &val)
{
	std::lock_guard<std::mutex> guard(mut);
    if(front==end)    // ����ǰ������Ҫ�ж϶����Ƿ��ѿգ���front==endʱ�ͱ����ӿ�
    return false;
    else
    {
        val=data[front];
        front=(front+1)%max_size;
        return true;
    }
}

template<typename T>
bool ConcurrentArrayQueue<T>::frontval(T &val) 
{
	std::lock_guard<std::mutex> guard(mut);
    if(front==end)   // ȡ����Ԫ�صĲ��������ͳ��Ӳ�����ͬ
    return false;
    else
    {
        val=data[front];
        return true;
    }
}

template<typename T>
int ConcurrentArrayQueue<T>::length()
{
	std::lock_guard<std::mutex> guard(mut);      
    // ���������Ԫ�ظ����ı�׼��ʽ���ǣ�(end-front+max_size)%max_size���ټ����������ӿ����Ƶ��õ� 
    return (end-front+max_size)%max_size;
}

template<typename T>
ConcurrentArrayQueue<T>::~ConcurrentArrayQueue()
{
    delete [] data;
}
