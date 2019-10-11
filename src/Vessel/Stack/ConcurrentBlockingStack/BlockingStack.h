#ifndef BLOCKING_STACK_H           
#define BLOCKING_STACK_H 
 
/**
 * BlockingStack: ����ջ�ĳ������       
 */
template<typename T>           
class BlockingStack
{
	public:
	virtual void push(T val) = 0;    	   // ��ջ����
    virtual void pop(T &val) = 0;          // ��ջ����
    virtual void top(T &val) = 0;          // ��ȡջ��Ԫ�ز���
    virtual int length() = 0;              // ��ȡջ��Ԫ�ظ���
	virtual bool empty() = 0;              // �ж�ջ�Ƿ�Ϊ�� 
	virtual bool full() = 0;               // �ж�ջ�Ƿ�Ϊ�� 
    virtual ~BlockingStack()               // ���������� 
	{
	}
};

#endif 
