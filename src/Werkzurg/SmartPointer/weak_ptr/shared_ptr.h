#ifndef SHARED_PTR_H 
#define SHARED_PTR_H
#include "ptr_counter.h"

template<typename T>  // ��Ԫģ��ǰ������ 
class weak_ptr;

/**
 * shared_ptr: ����ָ����ģ�� 
 */
template<class T> 
class shared_ptr
{
	public: 
	// 1. constructor
	shared_ptr();     			       // Ĭ�Ϲ��캯��
	shared_ptr(T *data);               // ����ָͨ�빹��shared_ptr 
	
	// 2. copy/move controller
	shared_ptr(shared_ptr &other);                      // �������캯�� 
	shared_ptr<T> &operator= (shared_ptr &other);       // ������ֵ����� 
	shared_ptr(shared_ptr &&other);                 	// �ƶ����캯�� 
	shared_ptr<T> &operator= (shared_ptr && other);     // �ƶ���ֵ����� 
	
	// 3. methods
	T &operator* () const;            // ������������������������Ӧ��ʼ�շ��ص�ǰ��������� 
	T *operator-> () const;           // ��ͷ���������ͷ�����Ӧ��ʼ�շ���ָ��ǰ�����ָ�룬��thisָ�� 
	bool unique() const;              // �ж���ָ����������ָ���Ƿ�Ψһ 
	
	// 4. destructor
	~shared_ptr();                    // �������� 
	
	friend class weak_ptr<T>;
	
	private:
	// 5. domain
	ptr_counter<T> *counter;          // ����ָ�������õ����ݶ���Ͷ�Ӧ����������������ָ��ʵ�ֵĺ�����counterָ����Զ��Ϊ��
};

/**
 * shared_ptr: ��ʼ��һ������ָ��ָ���Ӧ���͵��¶��� 
 */
template<typename T>
shared_ptr<T>::shared_ptr()
{
	counter=new ptr_counter<T>(new T());       // ��ʼ����������� 
}

/**
 * shared_ptr: ��ʼ��һ������ָ��ָ������Ĳ������� 
 */
template<typename T>
shared_ptr<T>::shared_ptr(T *data)
{
	if(data)
	counter=new ptr_counter<T>(data);       // ��ʼ����������� 
	else
	counter=nullptr;
}
	
/** 
 * shared_ptr: �������캯�� 
 * note: �������캯�����Ƚ�Դָ���shared_ptr���ü���������1��Ȼ��ǳ����Դָ��ļ����������������а������ö��� 
 * note: ע�����other.counter������this.counter�п� 
 */
template<typename T>
shared_ptr<T>::shared_ptr(shared_ptr &other)
{
	// 1. ��Դָ������ü���������1
	if(other.counter)      // other�п� 
	other.counter->shared_plus();
	// 2. ǳ����Դָ������ü��������������ö����� 
	counter=other.counter;
}

/**
 * operator=: ������ֵ�����  
 * note: ������ֵ������Ĺ������£�
 *       1. ��Դָ���shared_ptr���ü���������1������ǰָ������ü�������С1
 *       2. �ڼ�С1��
 *			(1) ��shared_ptr����ֵΪ0����weak_ptr��Ϊ0��������weak_ptr������ü�������free_data�ͷż������е����ö��󣬵��ǲ��ͷ� 
 *			(2) ��shared_ptr����ֵΪ0��weak_ptrΪ0����������weak_ptr�������������� 
 *       3. ǳ����Դָ������ü��������������ö�����
 * note: ע�����other.counter������this.counter�п� 
 */
template<typename T>
shared_ptr<T> &shared_ptr<T>::operator= (shared_ptr &other)  
{
	// 1. �ж��Ƿ�Ϊ������
	if(&other==this)
	return (*this);
	// 2. ��Դָ���shared_ptr���ü���ֵ����1������ǰָ������ü���ֵ��С1 
	if(counter)          // �����п� 
	counter->shared_sub(); 
	if(other.counter)    // other�п� 
	other.counter->shared_plus();
	// 3. �ڼ�С1��
	// (1) ��shared_ptr����ֵΪ0����weak_ptr��Ϊ0��������weak_ptr������ü�������free_data�ͷż������е����ö��󣬵��ǲ��ͷż����� 
	// (2) ��shared_ptr����ֵΪ0��weak_ptrΪ0����������weak_ptr�������������� 
	if(counter&&counter->get_shared_count()==0)   // ע�������п� 
	{
		counter->free_data(); 
		if(counter->get_weak_count()==0)
		delete counter;
	}
	// 4. ǳ����Դָ��ļ�����
	counter=other.counter; 
	// 5. ���ص�ǰ���������
	return (*this); 
}

/**
 * shared_ptr: �ƶ����캯�� 
 * note: �ƶ����������ֱ��ǳ����Դָ��ļ�������Ȼ��Դָ��ļ�������ΪĬ��ֵ���ɣ��ƶ�������û������Դָ��ָ�����ļ����� 
 * note: ע�����other.counter������this.counter�п� 
 */
template<typename T>
shared_ptr<T>::shared_ptr(shared_ptr &&other)
{
	// 1. ǳ����Դָ��ļ�����
	counter=other.counter; 
	// 2. ��Դָ��ļ�������Ϊ�� 
	other.counter=nullptr;
}

/**
 * operator=: �ƶ���ֵ�����
 * note: �ƶ���ֵ������Ĺ������£�
 *       1. ������ָ���shared_ptr����ֵ��1���ڼ�ȥ1�� 
 *			(1) ��shared_ptr����ֵΪ0����weak_ptr��Ϊ0��������weak_ptr������ü�������free_data�ͷż������е����ö��󣬵��ǲ��ͷż�����������weak_ptr�ͷ� 
 *	        (2) ��shared_ptr����ֵΪ0��weak_ptrΪ0����������weak_ptr��������������
 *		 2. ǳ����Դָ��ļ�����
 *       3. ��Դָ��ļ���������ΪĬ��ֵ 
 * note: ע�����other.counter������this.counter�п� 
 */
template<typename T>
shared_ptr<T> &shared_ptr<T>::operator= (shared_ptr && other)
{
	// 1. �ж��Ƿ�����ֵ 
	if(&other==this) 
	return (*this); 
	// 2. ������ָ���shared_ptr����ֵ��1���ڼ�ȥ1�� 
	// (1) ��shared_ptr����ֵΪ0����weak_ptr��Ϊ0��������weak_ptr������ü�������free_data�ͷż������е����ö��󣬵��ǲ��ͷż�����������weak_ptr�ͷ� 
	// (2) ��shared_ptr����ֵΪ0��weak_ptrΪ0����������weak_ptr�������������� 
	if(counter)    // �����п� 
	counter->shared_sub();
	if(counter&&counter->get_shared_count()==0)   // ע�������п� 
	{
		counter->free_data();
		if(counter->get_weak_count()==0)
		delete counter;
	}
	// 3. ǳ����Դָ��ļ�����
	counter=other.counter; 
	// 4. ����Դָ��ļ�����ΪĬ��ֵ 
	other.counter=nullptr;
	// 5. ���ص�ǰָ�����
	return (*this);
}

/**
 * operator*: �����������
 * note: ��������������ص�ǰָ�����ö�������� 
 * note: ע������ָ��this.counter�п� 
 */ 
template<typename T>
T &shared_ptr<T>::operator* () const
{
	if(counter)
	return *(counter->getdata());
}

/**
 * operator->: ��ͷ����� 
 * note: ��ͷ��������ص�ǰָ������ָ��
 * note: ע������ָ��this.counter�п�  
 */ 
template<typename T>
T *shared_ptr<T>::operator-> () const
{
	if(counter)
	return counter->getdata();
	else
	return nullptr;
}

/**
 * unique: �жϵ�ǰָ��ָ���������ü����Ƿ����1 
 * note: ע������ָ��this.counter�п� 
 */
template<typename T>
bool shared_ptr<T>::unique() const
{
	if(counter&&counter->get_shared_count()==1)
	return true;
	else
	return false; 
}

/**
 * ~shared_ptr: ��������    
 * note: �����������Ƚ���������shared_ptr����ֵ��С1������С��ֵΪ0���ͷ����õĶ��󣬵��ǲ��ͷż���������weak_ptr����ֵҲΪ0�����һ���ͷż����� 
 * note: ע������ָ��this.counter�п� 
 */
template<typename T>
shared_ptr<T>::~shared_ptr()
{
	if(counter)
	{
		counter->shared_sub();	
		if(counter->get_shared_count()==0)
		{
			counter->free_data();	
			if(counter->get_weak_count()==0)
			delete counter;
		}
	}
}

#endif
