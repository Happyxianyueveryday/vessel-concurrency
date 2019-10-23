#ifndef MCSLOCK_H 
#define MCSLOCK_H 
#include <atomic>  

struct MCSNode
{
	MCSNode():next(nullptr) 
	{	
		flag.store(true);
	}
	std::atomic<bool> flag; // flag: ��״̬��flagΪfalse��ʾ�ɼ�����Ϊtrue��ʾ���ɼ���  
	MCSNode *next;   	    // next: ָ����һ��MCSNode����ָ�� 
};

/**
 * MCSLock: MCSLock���ݵ�ǰ�߳������еĶ��н����״̬��������
 * note 1: �򵥽���һ��CLHLock��MCSLock������
 *         1. CLHLock�ĺ���ԭ���Ǽ���ʱ��ǰ��������������ǰ������״̬��Ȼ������Ƿ����������ʱֱ���޸ĵ�ǰ����״̬��״̬���� 
 *         2. MCSLock�ĺ���ԭ���Ǽ���ʱֱ��������鵱ǰ����״̬��Ȼ������Ƿ����������ʱ�����޸ĵ�ǰ����״̬������Ҫ�޸ĺ�̽���״̬ 
 * note 2: ������������    
 */
thread_local MCSNode *local_node;    // ��ǰ�߳������е��Լ��Ľ��
 
class MCSLock  
{
	public:
	// 1. constructor
	MCSLock();
	
	// 2. copy/move controller
	MCSLock(const MCSLock &other) = delete;
	MCSLock(MCSLock &&other) = delete;
	MCSLock &operator= (const MCSLock &other) = delete;
	MCSLock &operator= (MCSLock &&other) = delete;
	
	// 3. methods
	void lock();
	void unlock();
	
	// 4. destructor
	~MCSLock();
	
	private:
	// 5. domain
	std::atomic<MCSNode*> tail;      		// �����ȴ������е�β��� 
};

/**
 * MCSLock: ���캯��
 * note: ���캯������һ��MCSLock������߳�ʹ��
 */
MCSLock::MCSLock()
{
	tail.store(nullptr);
}

/**
 * lock: MCSLock��������
 * note: MCSLock�ļ�������ֻ��Ҫ�ڵ�ǰ����Ͻ����������״̬���� 
 */
void MCSLock::lock()
{
	// 1. ������ǰ�̵߳�״̬��� 
	local_node=new MCSNode();
	// 2. ����ǰ�̵߳�״̬�����뵽���������
	MCSNode *prev=tail.exchange(local_node);
	// 3. �������ǰ����㣬��ǰ�����͵�ǰ���������������鵱ǰ�̵߳�״̬��㣬ֱ��״̬Ϊfalse,Ҳ���ɼ���ʱ�����ɹ�
	if(prev)   
	{
		prev->next=local_node;
		while(local_node->flag.load()==true);
	}
	// 4. ��������ǰ�������ֱ�Ӽ����ɹ� 
}

/**
 * unlock: MCSLock��������
 * note: MCSLock�Ľ������������ж������ǰ�����ں�̽�㣬����Ҫ�޸ĺ�̽���״̬Ϊfalse��ʹ�ú�̽����Լ�����Ȼ���鵱ǰ����Ƿ�Ϊ���һ����㣬�����ÿ�β��㣻����ͷŵ�ǰ��� 
 */
void MCSLock::unlock()
{
	// 1. ����Ƿ���ں�̽�㣬�����ں�̽�㣬���޸ĺ�̽���״̬Ϊ�ɼ��� 
	if(local_node->next)
	local_node->next->flag.store(false); 
	// 2. Ȼ���鵱ǰ����Ƿ�Ϊ���һ����㣬�����ÿ�β��� 
	else 
	{
		MCSNode *expected=local_node;
		while(tail.compare_exchange_weak(expected, nullptr))
		{
			expected=local_node;
		}
	}
	// 3. �ͷŵ�ǰ���
	delete local_node;
}

/**
 * ~MCSLock: �������� 
 */
MCSLock::~MCSLock()
{
}

#endif
