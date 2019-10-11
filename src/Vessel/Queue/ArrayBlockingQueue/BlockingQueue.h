#ifndef BLOCKING_QUEUE_H    
#define BLOCKING_QUEUE_H               

/**
 * BlockingQueue: ���еĳ������  
 * note 1: ע�⣬virtual����ָ���麯����cpp�Ĵ��麯��ʹ��=0�ļǺ������������麯�����麯���ĸ���Ҫ������� 
 * note 2: ֻҪ����һ�����麯�����������ǳ�����࣬����������ͨ����һ�£����������������������Ϊ�˶�̬�󶨵���Ҫ 
 */
template<typename T>
class BlockingQueue
{
	public:
    virtual void push(T val) = 0;              // ������Ӳ���
    virtual void pop(T &val) = 0;              // ���г��Ӳ���
    virtual void frontval(T &val) = 0;         // ��ȡ����Ԫ��
    virtual int length() = 0;                  // ������г��ȣ��������е�Ԫ�ظ���
    virtual bool empty() = 0;                  // �ж϶����Ƿ�Ϊ�� 
    virtual bool full() = 0;                   // �ж϶����Ƿ�Ϊ�� 
    virtual ~BlockingQueue() {};               // ��������������ע���κεĻ��඼��������������������������ͬ�� 
};

#endif
