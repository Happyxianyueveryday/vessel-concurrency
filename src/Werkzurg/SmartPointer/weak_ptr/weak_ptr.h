#ifndef WEAK_PTR_H  
#define WEAK_PTR_H
#include "shared_ptr.h"

template<typename T>
class weak_ptr
{
	public:
	// 1. constructor
	weak_ptr(shared_ptr<T> &ptr);    // ��shared_ptr����һ��ָ����ͬ�����weak_ptr 
	
	// 2. copy/move controller
	weak_ptr(const weak_ptr<T> &other);                      // �������캯�� 
	weak_ptr<T> &operator= (weak_ptr<T> &other);       		 // ������ֵ����� 
	weak_ptr(weak_ptr<T> &&other) = delete;                  // �����ƶ����캯�� 
	weak_ptr<T> &operator= (weak_ptr<T> &&other) = delete;   // �����ƶ���ֵ����� 
	weak_ptr<T> &operator= (shared_ptr<T> &other);     		 // ��ͨ��ֵ����� 
	weak_ptr<T> &operator= (shared_ptr<T> &&other) = delete; // �����ƶ���ֵ����� 
	
	// 3. methods
	void reset();    			// ����ǰweak_ptr��Ϊ��
	int use_count() const;      // ������ָ������shared_ptr����
	shared_ptr<T> lock() const; // ��ȫ������ָ������shared_ptrָ�� 
	bool expired() const;       // �жϵ�ǰweak_ptr�Ƿ�Ϊ��  
	
	// 4. destructor
	~weak_ptr();            // �������� 
	
	private:
	// 5. domain
	ptr_counter<T> *counter;// ��ǰweak_ptr���󶨵ļ����������ڲ��������ö��󣩣�ע��weak_ptr�о��Բ��ܱ���shared_ptr�Ŀ�������ָ�� 
};

/**
 * weak_ptr: ���캯������shared_ptr����һ��weak_ptr 
 */
template<typename T>
weak_ptr<T>::weak_ptr(shared_ptr<T> &ptr)
{
	counter=ptr.counter;    // ָ����������shared_ptr�ļ��������ڲ��������ö���
	if(counter) 
	counter->weak_plus();   // ���Ӽ�������weak_ptr����ֵ
}

/**
 * weak_ptr: �������캯�� 
 * note: ��Ҫ���Ĳ�����shared_ptr���ƣ����Ƚ�Դָ���weak_ptr����ֵ��1��Ȼ��ǳ�������󼴿�
 */
template<typename T>
weak_ptr<T>::weak_ptr(const weak_ptr<T> &other)
{
	// 1. ��Դָ���weak_ptr����ֵ����1 
	if(other.counter)       // Դָ��counter�п� 
	other->counter->weak_plus();
	// 2. ǳ�������� 
	counter=other.counter;
}

/**
 * operator=: ������ֵ����� 
 * note: ������ֵ�������Ҫ���еĹ������£�
 * 		 1. ��Դָ���weak_ptr����ֵ����1������ǰָ���weak_ptr����ֵ����1 
 *       2. �����ٺ�ǰָ���weak_ptr����ֵ��shared_ptr����ֵ��Ϊ0�����ͷż��������� 
 *       3. ��Դָ��ǳ������������Ա 
 */
template<typename T>
weak_ptr<T> &weak_ptr<T>::operator= (weak_ptr<T> &other)
{
	// 1. �ж��Ƿ�Ϊ����ֵ
	if(&other==this)
	return (*this); 
	// 2. ��Դָ���weak_ptr����ֵ����1������ǰָ���weak_ptr����ֵ����1
	if(other.counter)
	other.counter->weak_plus();
	if(counter)
	counter->weak_sub();
    // 3. �����ٺ�ǰָ���weak_ptr����ֵ��shared_ptr����ֵ��Ϊ0�����ͷż��������� 
    if(counter&&counter->get_weak_count()==0&&counter->get_shared_count()==0)
    delete counter;
	// 4. ��Դָ��ǳ������������Ա 
	counter=other.counter;
	// 5. ���ص�ǰָ�����
	return (*this); 
}

/**
 * operator=: ��ͨ��ֵ����� 
 * note: ��ͨ��ֵ����������޸ĵ�ǰweak_ptr�������shared_ptr������Ҫ���еĲ������£�
 * 		 1. ��Դָ���weak_ptr����ֵ����1������ǰָ���weak_ptr����ֵ����1 
 *       2. ����С��ǰָ���������weak_ptr����ֵ��shared_ptr����ֵ��Ϊ0�����ͷŵ�ǰָ��ļ���������
 *       3. ǳ����Դָ��ļ����� 
 */
template<typename T>
weak_ptr<T> &weak_ptr<T>::operator= (shared_ptr<T> &other)
{
	// 1. �ж��Ƿ�Ϊ����ֵ������ǰweak_ptr�󶨵�shared_ptrָ������Դָ����ͬ��
	if(other.counter==counter)
	return (*this); 
	// 2. ��Դָ���weak_ptr����ֵ����1������ǰָ���weak_ptr����ֵ����1 
	if(other.counter)    // ע���Դָ���п� 
	other.counter->weak_plus();
	if(counter)
	counter->weak_sub();
	// 3. ����С��ǰָ���������weak_ptr����ֵ��shared_ptr����ֵ��Ϊ0�����ͷŵ�ǰָ��ļ���������
	if(counter&&counter->get_weak_count()==0&&counter->get_shared_count()==0)
    delete counter;
    // 4. ��Դָ��ǳ������������Ա 
	counter=other.counter;
	// 5. ���ص�ǰָ�����
	return (*this);
}

/**
 * reset: ��weak_ptr��Ϊ��ָ�� 
 * note: ���Ƚ������������weak_ptr����ֵ��С1����С������ǰָ���������weak_ptr����ֵ��shared_ptr����ֵ��Ϊ0�����ͷŵ�ǰָ��ļ��������� 
 */	
template<typename T>
void weak_ptr<T>::reset()
{
	if(counter)
	counter->weak_sub();
	if(counter&&counter->get_weak_count()==0&&counter->get_shared_count()==0)
	delete counter;
	counter=nullptr;
}

/**
 * use_count: ���ص�ǰweak_ptr��ָ��Ķ����shared_ptr���ø��� 
 */ 
template<typename T>
int weak_ptr<T>::use_count() const
{
	if(counter)
	return counter->get_shared_count();
	else
	return 0;
}

/**
 * lock: ��ȫ��õ�ǰweak_ptr��ָ������shared_ptr 
 */
template<typename T>
shared_ptr<T> weak_ptr<T>::lock() const
{
	if(counter&&counter->get_shared_count())
	// return shared_ptr(counter->getdata()); ����д����Σ�յģ�������ʹ��һ����ָ�봴�����shared_ptr 
	// ��˴˴���Ҫ��������Ĳ�����������ָ���shared_ptr�����ֶ�����shared_ptr������ֵ��������ü��� 
	{
		shared_ptr<T> res(nullptr);
		res.counter=counter;
		counter->shared_plus();  
		return res;  
	} 
	else
	return shared_ptr<T>(nullptr);
}

/**
 * expired: �жϵ�ǰweak_ptr�Ƿ���Ч 
 */
template<typename T>
bool weak_ptr<T>::expired() const
{
	if(counter&&counter->get_shared_count())
	return true;
	else
	return false;
}

template<typename T>
weak_ptr<T>::~weak_ptr()
{
	if(counter&&counter->get_shared_count()==0&&counter->get_weak_count()==0)
	delete counter;
}        

#endif
