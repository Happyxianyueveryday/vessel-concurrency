#ifndef TICKET_SPIN_LOCK_H             
#define TICKET_SPIN_LOCK_H
#include <atomic>

/** 
 * TicketSpinLock: ��ƽ�������� 
 * note: ��ƽ��������ͨ��ʱ�����������ʵ�֣����Ļ�����ʹ��������ţ��ŶӺźͷ���ţ�������μ�ʵ��  
 */
class TicketSpinLock
{
	public: 
	// 1. constructor
	TicketSpinLock();    // ���캯�� 
	
	// 2. copy/move controller 
	TicketSpinLock(const TicketSpinLock &other) = delete;
	TicketSpinLock(TicketSpinLock &&other) = delete;
	TicketSpinLock &operator= (const TicketSpinLock &other) = delete;
	TicketSpinLock &operator= (TicketSpinLock &&other) = delete;
	
	// 3. methods
	unsigned int lock();                // ����������
	void unlock(unsigned int ticket);   // ����������
	
	private:
	// 4. domains
	std::atomic<unsigned int> wait_no;       // �ŶӺţ�������Ҫ�������߳����Ⱦ�����һ�������Ϊ�ŶӺţ�Ȼ����и��ŶӺŽ��еȴ� 
	std::atomic<unsigned int> service_no;    // ����ţ���һ����������߳�����Ӧ�þ��е��ŶӺ�
};

/**
 * TicketSpinLock: ���캯��������δ������������ 
 */ 
TicketSpinLock::TicketSpinLock()
{
	wait_no.store(0);        // ��ʼ״̬�£���һ����Ҫ�����߳���������ŶӺ�Ϊ0 
	service_no.store(0);     // ��ʼ״̬�£���һ����ȡ�������̵߳��������Ϊ0 
}  

/** 
 * lock: ���������� 
 */
unsigned int TicketSpinLock::lock()  
{
	// 1. ������ǰ��Ҫ��������̷߳���һ���ŶӺţ�Ȼ���ŶӺ�����1���Է������һ����Ҫ�������߳� 
	unsigned int ticket=wait_no.fetch_add(1);
	// 2. Ȼ��ʹ��CASѭ���Ƚϵ�ǰ�̷߳��䵽���ŶӺ��Ƿ���������ȣ�������õ�ǰ�������ǲ����ӷ���ţ���Ϊ���߳���δ��ɲ�����  
	while(service_no.load()!=ticket);
	// 3. ��ȡ����ǰ���󣬷�������ȡ���ŶӺţ����ŶӺ����ں����Ľ���
	return ticket;   
}

/**
 * unlock: ���������� 
 */
void TicketSpinLock::unlock(unsigned int ticket)
{
	// ��ǰ�̵߳Ĳ���ȫ����ɣ�������������ͷŵ�ǰ������������һ���̻߳���� 
	unsigned int expected=ticket; 
	while(!service_no.compare_exchange_weak(expected, ticket+1)) 
	{
		expected=ticket;    // ����ֵ����expected��λ 
	}
}

#endif
