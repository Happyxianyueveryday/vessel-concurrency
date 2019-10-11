#ifndef TICKET_LOCAL_SPIN_LOCK_H                              
#define TICKET_LOCAL_SPIN_LOCK_H 
#include <thread>
#include <atomic> 

thread_local unsigned int ticket;   // ÿ���߳�ȡ�õ��ŶӺţ���ͬ�߳̾���һ����ͬ���ŶӺŸ��������ʹ��thread_local��ÿ���̶߳������һ��������ticket�����ĸ��� 

/** 
 * TicketLocalSpinLock: ��ƽ�������� 
 * note: ��ƽ��������ͨ��ʱ�����������ʵ�֣����Ļ�����ʹ��������ţ��ŶӺźͷ���ţ�������μ�ʵ��  
 */
class TicketLocalSpinLock
{
	public: 
	// 1. constructor
	TicketLocalSpinLock();    // ���캯�� 
	
	// 2. copy/move controller 
	TicketLocalSpinLock(const TicketLocalSpinLock &other) = delete;
	TicketLocalSpinLock(TicketLocalSpinLock &&other) = delete;
	TicketLocalSpinLock &operator= (const TicketLocalSpinLock &other) = delete;
	TicketLocalSpinLock &operator= (TicketLocalSpinLock &&other) = delete;
	
	// 3. methods
	void lock();     // ����������
	void unlock();   // ����������
	
	private:
	// 4. domains
	std::atomic<unsigned int> wait_no;       // �ŶӺţ�������Ҫ�������߳����Ⱦ�����һ�������Ϊ�ŶӺţ�Ȼ����и��ŶӺŽ��еȴ� 
	std::atomic<unsigned int> service_no;    // ����ţ���һ����������߳�����Ӧ�þ��е��ŶӺ�
};

/**
 * TicketLocalSpinLock: ���캯��������δ������������ 
 */ 
TicketLocalSpinLock::TicketLocalSpinLock()
{
	wait_no.store(0);        // ��ʼ״̬�£���һ����Ҫ�����߳���������ŶӺ�Ϊ0 
	service_no.store(0);     // ��ʼ״̬�£���һ����ȡ�������̵߳��������Ϊ0 
}  

/** 
 * lock: ���������� 
 */
void TicketLocalSpinLock::lock()  
{
	// 1. ������ǰ��Ҫ��������̷߳���һ���ŶӺţ�Ȼ���ŶӺ�����1���Է������һ����Ҫ�������߳� 
	ticket=wait_no.fetch_add(1);
	// 2. Ȼ��ʹ��CASѭ���Ƚϵ�ǰ�̷߳��䵽���ŶӺ��Ƿ���������ȣ�������õ�ǰ�������ǲ����ӷ���ţ���Ϊ���߳���δ��ɲ�����    
	while(service_no.load()!=ticket);  
}
 
/**
 * unlock: ���������� 
 */
void TicketLocalSpinLock::unlock()
{
	// ��ǰ�̵߳Ĳ���ȫ����ɣ�������������ͷŵ�ǰ������������һ���̻߳���� 
	unsigned int expected=ticket; 
	while(!service_no.compare_exchange_weak(expected, ticket+1)) 
	{
		expected=ticket;    // ����ֵ����expected��λ 
	}
}

#endif
