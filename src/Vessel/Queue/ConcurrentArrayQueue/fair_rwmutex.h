#ifndef FAIR_RWMUTEX_H 
#define FAIR_RWMUTEX_H               
#include <mutex>    

/**
 * fair_rwmutex: ���ڻ�����ʵ�ֵĶ����ȵĶ�д��  
 * note 1: ��д��: ��ָ�������߳�ͬʱ���ж���������ֻ����һ���߳�ͬʱ����д�������Ҷ�д����֮�䲻��ͬʱ���е�һ�ֻ����� 
 * note 2: ��д��ƽ��: ����CPU��ǰû�б���ռ����д�̻߳��߶��߳���ռ�����ڵȴ����̶߳���ѭ�����˳����ռCPU����������ռ���������ֶ��̻߳���д�߳� 
 * note 3: ��д��ƽ����ʵ�ֺ��ģ��½�һ�����л�����seq��ͨ���û������������е����д�̻߳��߶��߳��ڸû��������Ŷӣ��Ӷ�ʵ����������ռ�Ķ�дƽ�� 
 */
class fair_rwmutex      
{
	public: 
	// 1. constructor
	fair_rwmutex();   		   // Ĭ�Ϲ��캯�� 
	
	// 2. copy/move controller = delete
	fair_rwmutex(const fair_rwmutex &other) = delete;               // ��д�����ÿ������캯�� 
	fair_rwmutex &operator= (const fair_rwmutex &other) = delete;
	fair_rwmutex(fair_rwmutex &&other) = delete;
	fair_rwmutex &operator= (fair_rwmutex &&other) = delete;
	
	// 3. methods
	void read_lock();      // �����̼߳�������ȡ����ǰ���� 
	void read_unlock();    // �����߳̽�������ȡ��������� 
	void write_lock();     // д���̼߳�����д�����ǰ���� 
	void write_unlock();   // д���̼߳�����д���������� 
	
	private:
	// 4. domains 
	std::mutex count_mut;       // ������߼�����count�޸ĵĻ����� 
	int count;             // ���������ļ����� 
	std::mutex write_mut;       // ����д�߼以��д�Ļ����� 
	std::mutex seq_mut;         // ʵ�ֶ�д��ƽ�Ļ����� 
};

/**
 * fair_rwmutex: ��ʼ����д�� 
 */  
fair_rwmutex::fair_rwmutex()
{
	count=0;    // ���ö��߼�����Ϊ0 
}

/**
 * read_lock: �����̼߳��� 
 * note 1: �����̼߳�����������������Ҫ���裺 
 *       1. ����ؽ����߼�����count����1 
 *       2. ������жϵ�ǰ���������Ƿ�Ϊ1����Ϊ1��ǰ�߳�Ϊ��һ�����̣߳������write_mut����ֹд�ߵ�д����
 * note 2: ��д��ƽ���б���ʹ��һ���µĻ�������֤read_lock()������write_lock()����֮����Ի���ִ�� 
 */
void fair_rwmutex::read_lock()
{
	seq_mut.lock();     // ��֤����ִ��read_lock��write_lock��P���� 
	count_mut.lock();   // ������ʶ��߼�����count��P���� 
	count+=1;           // ��������������1 
	if(count==1)        // ��������������1��ֵΪ1����Ϊ��һ�����ߣ����ݶ�����ԭ�����ȼ���д�ߵĻ�������ʹ��д�ߵ�д�������ӳٵ����ߺ���� 
	write_mut.lock();   // ����д�ߵĻ�����       
	count_mut.unlock(); // ������ʶ��߼�����count��V���� 
	seq_mut.unlock();   // ��֤����ִ��read_lock��write_lock��V���� 
} 

/**
 * read_unlock: �����߳̽��� 
 * note: �����߳̽�����������������Ҫ���裺
 *		 1. ����ؽ����߼�����count����1 
 *       2. ������жϵ�ǰ���߼������Ƿ�Ϊ0����Ϊ0��˵�����ж����Ѿ�ִ�������������ʱ����д�ߵĻ�������ʹ�ñ��ӳٵ�д���ܹ���������д���� 
 */  
void fair_rwmutex::read_unlock()
{
	count_mut.lock();   // ������ʶ��߼�����count��P����
	count-=1;           // ��С���߼�����count 
	if(count==0)        // ������������С��Ϊ0�������еĶ����Ѿ��������˶���������ʱ�ͷ�д�ߵĻ�����������д�߽���д���� 
	write_mut.unlock(); // ����д�ߵĻ����� 
	count_mut.unlock(); // ������ʶ��߼�����count��V���� 
} 

/**
 * write_lock: д�߼��� 
 */
void fair_rwmutex::write_lock()
{
	seq_mut.lock();     // ��֤����ִ��read_lock��write_lock��P���� 
	write_mut.lock();
	seq_mut.unlock();   // ��֤����ִ��read_lock��write_lock��V���� 
} 

/**
 * write_unlock: д�߽��� 
 */
void fair_rwmutex::write_unlock()
{
	write_mut.unlock();
}

 

#endif
