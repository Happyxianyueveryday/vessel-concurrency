#ifndef CONCURRENT_QUEUE_H        
#define CONCURRENT_QUEUE_H         
 
/**
 * Queue: ���еĳ������
 * note 1: ע�⣬virtual����ָ���麯����cpp�Ĵ��麯��ʹ��=0�ļǺ������������麯�����麯���ĸ���Ҫ������� 
 * note 2: ֻҪ����һ�����麯�����������ǳ�����࣬����������ͨ����һ�£����������������������Ϊ�˶�̬�󶨵���Ҫ 
 */
template<typename T>
class ConcurrentQueue 
{
	public:
    virtual bool push(T val) = 0;            // ������Ӳ���            
    virtual bool pop(T &val) = 0;            // ���г��Ӳ���
    virtual bool frontval(T &val) = 0;       // ��ȡ����Ԫ��
    virtual int length() = 0;                // ������г��ȣ��������е�Ԫ�ظ���  
    virtual ~ConcurrentQueue() {};           // ��������������ע���κεĻ��඼��������������������������ͬ��   
};

#endif 
