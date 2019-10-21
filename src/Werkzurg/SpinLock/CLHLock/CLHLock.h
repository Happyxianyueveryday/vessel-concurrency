#ifndef CLHLOCK_H
#define CLHLOCK_H
#include <atomic> 

struct CLHNode
{
	CLHNode()
	{	
		flag.store(true);
	}
	std::atomic<bool> flag;        // flagΪfalse��ʾδ������Ϊtrue��ʾ�Ѽ���  
};

/**
 * CLHLock: CLHLock���ݵ�ǰ�߳������еĶ��н���ǰ��������״̬��������
 * note: ������������    
 */
thread_local CLHNode *local_node;    // ��ǰ�߳������е��Լ��Ľ��
thread_local CLHNode *pred_node;     // ��ǰ�̵߳Ľ���ǰ����� 
 
class CLHLock  
{
	public:
	// 1. constructor
	CLHLock();
	
	// 2. copy/move controller
	CLHLock(const CLHLock &other) = delete;
	CLHLock(CLHLock &&other) = delete;
	CLHLock &operator= (const CLHLock &other) = delete;
	CLHLock &operator= (CLHLock &&other) = delete;
	
	// 3. methods
	void lock();
	void unlock();
	
	// 4. destructor
	~CLHLock();
	
	private:
	// 5. domain
	std::atomic<CLHNode*> tail;      		// �����ȴ������е�β��� 
};

/**
 * CLHLock: ���캯��
 * note: ���캯������һ��CLHLock������߳�ʹ�ã����ֻ��Ҫ��ʼ��tail��local_node��prev_nodeΪ�ռ��� 
 */
CLHLock::CLHLock()
{
	tail.store(nullptr);
	local_node=nullptr;
	pred_node=nullptr; 
}

/**
 * lock: CLHLock��������
 * note 1: ĳ���߳̽���CLHLock�������㷨����: 
 *		 1. ����һ���̱߳��صĽ�㣬��local_node(thread_local)
 *       2. ����ǰ���local_node�����ŶӶ��У���ʹ��atomic_exchange�����޸�β���tail(��thread_local)Ϊ��ǰ�ı��ؽ�㣬����β����޸�ǰ��ֵ���ز���ֵ��ǰ��������pred_node(thread_local) 
 *	 	 3. ����ȡ��pred_nodeΪ�գ���ֱ�ӻ���������ǿգ��ڻ�ȡ��ǰ�����pred_node(thread_local)��ָ���CLHNode��flag������������ֱ��flag==falseʱ����  
 * note 2: ������c++������gc���ƣ�һ���߳�ÿ�ν���ʹ���һ���µ�local_node��Ϊ��ǰ��㣬����ͬһ���̶߳�μ�������� 
 */
void CLHLock::lock()
{
	// 1. ������ǰ�̵߳Ľ��Local_node(thread_local) 
	local_node=new CLHNode(); 
	// 2. �޸�tailΪ��ǰ�´����Ľ�㣬�����tail�ľ�ֵ����ֵ���ǵ�ǰ����ǰ�����pred_node
	pred_node=tail.exchange(local_node);
	// 3. ��pred_node==nullptr��ֱ�ӻ������������ǰ������Ͻ�����������ǰ������flag����Ϊfalseʱ���ɽ��м���
	if(pred_node)
	{
		while(pred_node->flag.load()==true);    // ��ǰ�����pred_node��flag������������ֱ��flag==false���ɼ���
		delete pred_node;                       // ���������˵��ǰ������Ӧ���߳��Ѿ����������ͷ�ǰ����� 
	}
	// 4. ���ջ���� 
}

/**
 * unlock: CLHLock�������� 
 * note 1: ĳ���߳̽���CLHLock�������㷨����:  
 *       1. ����ǰ����״̬����Ϊfalse
 * note 2: ������c++û��gc���ƣ���ǰ�̲߳�����ֱ�����ٵ�ǰ��㣬��Ϊ��ǰ��㲻֪���Ƿ���ں�̽����Ҫ�ڵ�ǰ����������жϱ�־flag���������˳��˸մ���CLHLockʱ�⣬����ʱ�̱ض���һ��CLHNodeû�б��ͷţ���������ͷŽ���CLHLock���������� 
 */
void CLHLock::unlock()
{
	if(local_node)
	local_node->flag.store(false); 
}

/**
 * ~CLHLock: �������� 
 * note: �������������������һ��CLHNode��� 
 */
CLHLock::~CLHLock()
{
	CLHNode *temp=tail.load(); 
	if(temp)
	delete temp;
}

#endif
