#ifndef RWMUTEX_H                 
#define RWMUTEX_H       
#include <mutex>        

/**
 * rwmutex: ���ڻ�����ʵ�ֵĶ����ȵĶ�д��
 * note 1: ��д�����������߳�ͬʱ���ж���������ֻ����һ���߳�ͬʱ����д�������Ҷ�д����֮�䲻��ͬʱ���е�һ�ֻ����� 
 * note 2: �����ȣ���CPUδ����ռ���߱�д�߳���ռʱ����д�߳̾���ƽ�ȵģ���CPU�����߳���ռ�󣬺����ĵȴ������еĶ��߳̽�������ִ�У�д�߳����ӳ�ִ��
 * note 3: �����ȵ�ʵ�֣�ͨ����д�߳����ӻ������������߳��򲻼���ʵ�֡��ҵ�һ�����߳̿���ͨ������д�̵߳Ļ������ӳ����еĵȴ���д�̵߳�ִ�У����һ�����߳��������ͷ�д�̵߳Ļ����� 
 */   
class rwmutex
{
	public: 
	// 1. constructor
	rwmutex();   		   // Ĭ�Ϲ��캯�� 
	
	// 2. copy/move controller = delete
	rwmutex(const rwmutex &other) = delete;               // ��д�����ÿ������캯�� 
	rwmutex &operator= (const rwmutex &other) = delete;
	rwmutex(rwmutex &&other) = delete;
	rwmutex &operator= (rwmutex &&other) = delete;
	
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
};

/**
 * rwmutex: ��ʼ����д�� 
 */  
rwmutex::rwmutex()
{
	count=0;    // ���ö��߼�����Ϊ0 
}

/**
 * read_lock: �����̼߳��� 
 * note: �����̼߳�����������������Ҫ���裺 
 *       1. ����ؽ����߼�����count����1 
 *       2. ������жϵ�ǰ���������Ƿ�Ϊ1����Ϊ1��ǰ�߳�Ϊ��һ�����̣߳������write_mut����ֹд�ߵ�д����
 */
void rwmutex::read_lock()
{
	count_mut.lock();   // ������ʶ��߼�����count��P���� 
	count+=1;           // ��������������1 
	if(count==1)        // ��������������1��ֵΪ1����Ϊ��һ�����ߣ����ݶ�����ԭ�����ȼ���д�ߵĻ�������ʹ��д�ߵ�д�������ӳٵ����ߺ���� 
	write_mut.lock();   // ����д�ߵĻ�����       
	count_mut.unlock(); // ������ʶ��߼�����count��V���� 
} 

/**
 * read_unlock: �����߳̽��� 
 * note: �����߳̽�����������������Ҫ���裺
 *		 1. ����ؽ����߼�����count����1 
 *       2. ������жϵ�ǰ���߼������Ƿ�Ϊ0����Ϊ0��˵�����ж����Ѿ�ִ�������������ʱ����д�ߵĻ�������ʹ�ñ��ӳٵ�д���ܹ���������д���� 
 */  
void rwmutex::read_unlock()
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
void rwmutex::write_lock()
{
	write_mut.lock();
} 

/**
 * write_unlock: д�߽��� 
 */
void rwmutex::write_unlock()
{
	write_mut.unlock();
}

 

#endif
