#define CONCURRENT_ARRAY_QUEUE_H          
#define CONCURRENT_ARRAY_QUEUE_H      
#include "ConcurrentQueue.h" 
#include "rwmutex.h"
#include "wrmutex.h"
#include "fair_rwmutex.h"
#include <string> 

/**
 * ConcurrentArrayQueue: ����ջ 
 */
template<typename T>
class ConcurrentArrayQueue: public ConcurrentQueue<T>
{
    public:   
    // 1. public constructor 
    static ConcurrentArrayQueue<T> *ArrayQueueFactory(int max_size, std::string mode);    // ���캯��

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
    std::string mode;     // �趨�Ķ�д���е����ȼ����� 
    
    rwmutex rwmut;    // �����ȵĶ�д�� 
    wrmutex wrmut;    // д���ȵĶ�д�� 
    fair_rwmutex fair_rwmut;  // ��д��ƽ�Ķ�д�� 
    
    // 5. private methods
    void set_lock(std::string lockmode);                 
    ConcurrentArrayQueue(int _max_size, std::string _mode); 
};

template<typename T>
ConcurrentArrayQueue<T> *ConcurrentArrayQueue<T>::ArrayQueueFactory(int _max_size, std::string _mode) 
{
    if(_max_size<0)
    return NULL;
    else
    return new ConcurrentArrayQueue<T>(_max_size+1, _mode);    // �����ܹ����_max_size��Ԫ�صĶ��У���Ӧ�Ĵ洢���鳤����Ҫ��_max_size+1
}

template<typename T>
ConcurrentArrayQueue<T>::ConcurrentArrayQueue(int _max_size, std::string _mode)
{
    max_size=_max_size;
    data=new T [max_size];
    front=0;   // ��ʼ�Ķ���Ԫ���±�Ϊ0
    end=0;     // ��ʼ�Ķ�βԪ�ص�β��Ԫ���±�Ϊ0
    if(_mode=="read-first"||_mode=="write-first"||_mode=="fair")
    mode=_mode;
    else
    mode="read-first";
}

template<typename T>
ConcurrentArrayQueue<T>::ConcurrentArrayQueue(const ConcurrentArrayQueue<T> &other)
{
	other.set_lock("write_lock");
    max_size=other.max_size;
    data=new T [max_size];
    front=other.front;
    end=other.end;
    mode=other.mode;

    for(int i=0;i<max_size;i++)
    {
        data[i]=other.data[i];
    }
    other.set_lock("write_unlock");
}

template<typename T>
ConcurrentArrayQueue<T>::ConcurrentArrayQueue(ConcurrentArrayQueue<T> &&other)
{
	other.set_lock("write_lock");
	max_size=other.max_size;
    data=other.data;
    front=other.front;
    end=other.end;
    mode=other.mode;

    other.data=new T [max_size];
    other.front=0;   // ��ʼ�Ķ���Ԫ���±�Ϊ0
    other.end=0;     // ��ʼ�Ķ�βԪ�ص�β��Ԫ���±�Ϊ0 
    other.set_lock("write_unlock");
}

template<typename T>
bool ConcurrentArrayQueue<T>::push(T val) 
{
	set_lock("write_lock");
    if((end+1)%max_size==front)    // ���ǰ��Ҫ�����ж϶����Ƿ�������frontָ����ף�endָ���β��β��Ԫ�أ����end����һ��Ԫ�ؾ���frontʱ����(end+1)%max_size==frontʱ����
    {
    	set_lock("write_unlock");
    	return false;
	}
    else
    {
        data[end]=val;
        end=(end+1)%max_size;
        set_lock("write_unlock");
        return true;
    }
}

template<typename T>
bool ConcurrentArrayQueue<T>::pop(T &val)
{
	set_lock("write_lock");
    if(front==end)    // ����ǰ������Ҫ�ж϶����Ƿ��ѿգ���front==endʱ�ͱ����ӿ�
    {
    	set_lock("write_unlock");
    	return false;
	}
    else
    {
        val=data[front];
        front=(front+1)%max_size;
        set_lock("write_unlock");
        return true;
    }
}

template<typename T>
bool ConcurrentArrayQueue<T>::frontval(T &val) 
{
	set_lock("read_lock");
    if(front==end)   // ȡ����Ԫ�صĲ��������ͳ��Ӳ�����ͬ
    {
    	set_lock("read_unlock");
    	return false;
    }
    else
    {
        val=data[front];
        set_lock("read_unlock");
        return true;
    }
}

template<typename T>
int ConcurrentArrayQueue<T>::length()
{
    // ���������Ԫ�ظ����ı�׼��ʽ���ǣ�(end-front+max_size)%max_size���ټ����������ӿ����Ƶ��õ� 
    set_lock("read_lock");
    int res=(end-front+max_size)%max_size;
    set_lock("read_unlock");
    return res;
}

template<typename T>
void ConcurrentArrayQueue<T>::set_lock(std::string lockmode)
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
ConcurrentArrayQueue<T>::~ConcurrentArrayQueue()
{
	set_lock("write_lock");
    delete [] data;
    set_lock("write_unlock");
}
