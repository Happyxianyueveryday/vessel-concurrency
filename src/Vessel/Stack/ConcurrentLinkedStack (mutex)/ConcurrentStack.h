#ifndef CONCURRENT_STACK_H       
#define CONCURRENT_STACK_H 

/**
 * ConcurrentStack: �̰߳�ȫջ�ĳ������       
 */
template<typename T>           
class ConcurrentStack
{
	public:
	virtual bool push(T val) = 0;    	   // ��ջ����
    virtual bool pop(T &val) = 0;          // ��ջ����
    virtual bool top(T &val) = 0;          // ��ȡջ��Ԫ�ز���
    virtual int length() = 0;              // ��ȡջ��Ԫ�ظ��� 
    virtual ~ConcurrentStack()             // ���������� 
	{
	}
};

#endif 