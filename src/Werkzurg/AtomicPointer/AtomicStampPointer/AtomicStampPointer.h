#ifndef ATOMIC_STAMP_POINTER_H  
#define ATOMIC_STAMP_POINTER_H  
#include "AtomicPair.h" 
#include <atomic>

/**
 * AtomicStampPointer: ���а汾�ŵ�ԭ��ָ��ģ�� 
 * note: AtomicStampPointer�����ǰ�װ�˰汾�ŵ��̰߳�ȫָ�룬��������ָ����ָ�����Ļ��� 
 */
template<typename T>
class AtomicStampPointer
{
	public:
	// 1. constructor
	AtomicStampPointer(T *_data);   // ���캯�� 
	
	// 2. copy/move controller
	AtomicStampPointer(const AtomicStampPointer &other) = delete;               // ԭ��ģ����ÿ������ƶ� 
	AtomicStampPointer(AtomicStampPointer &&other) = delete;
	AtomicStampPointer &operator= (const AtomicStampPointer &other) = delete;
	AtomicStampPointer &operator= (AtomicStampPointer &&other) = delete;
	
	// 3. methods
	bool is_lock_free();       // �ж��Ƿ�֧������ԭ�Ӳ�������֧�����ʹ��������
	unsigned int get_stamp();  // ��ȡ��ǰ�İ汾�� 
	const T* get_ptr();               // ��ȡ��װ�Ķ�����ָ�� 
	bool compare_exchange_weak(T *&expected_value, T *new_value, unsigned int expected_version, unsigned int new_version);
	bool compare_exchange_strong(T *&expected_value, T *new_value, unsigned int expected_version, unsigned int new_version); 
	
	// 4. destructor
	~AtomicStampPointer();
	
	private:
	// 5. domain
	std::atomic<AtomicPair<T> *> pair; 
}; 

/**
 * AtomicStampPointer: ���캯�� 
 */
template<typename T>
AtomicStampPointer<T>::AtomicStampPointer(T *_data)
{
	pair=AtomicPair<T>::factory(_data, 0);
}

/**
 * is_lock_free: �жϸ�ԭ�Ӱ汾ָ������в����Ƿ�Ϊ������ 
 * note: ֻҪ��װ��AtomicPair<T> *ָ������в�����������ԭ�Ӱ汾ָ��AtomicStampPointer�Ĳ����������� 
 */
template<typename T>
bool AtomicStampPointer<T>::is_lock_free()
{
	return pair.is_lock_free();  
}

/**
 * get_stamp: ��ȡ��ǰ�İ汾�� 
 */
template<typename T>
unsigned int AtomicStampPointer<T>::get_stamp()
{
	return pair.load()->version;
}

/**
 * get_ptr: ��ȡ��װ����ָ�� 
 */
template<typename T>
const T* AtomicStampPointer<T>::get_ptr()
{
	return pair.load()->data;
}

/** 
 * compare_exchange_weak: ԭ��ָ��CAS���� 
 * param expected_value: ������ǰ��ָ��ֵ���ͱ�׼��Ľӿڲ�ͬ����ǰָ��ֵ����ͨ��expected_value����������� 
 * param new_value: ��Ҫ���µ�ָ��ֵ
 * param expected_version: ������ǰ�İ汾�ţ��ͱ�׼��Ľӿڲ�ͬ����ǰ�汾�Ų���ͨ��expected_version�����������  
 * param new_version: ��Ҫ���µİ汾�� 
 * note: 1. ��Ŀ��ָ��ֵ�͵�ǰ�����ָ��ֵ��ȣ������汾��Ҳһ�£������CAS�����������µ�һ��AtomicPair�������ԭ���� 
 *       2. �ر�ģ�һ����ѡ���Ż��ǣ����Ҫ���µ�ֵ�͵�ǰ�����ֵ��ȫ��ȣ���������и���
 *       3. ��Ŀ��ָ��ֵ�͵�ǰ�����ָ��ֵ����ȣ���Ŀ��ָ��ֵ����Ϊ��ǰ�����ָ��ֵ 
 */
template<typename T>
bool AtomicStampPointer<T>::compare_exchange_weak(T *&expected_data, T *new_data, unsigned int expected_version, unsigned int new_version)
{
	AtomicPair<T> *current=pair.load();     	// ��ǰAtomicStampPointer�������AtomicPair 
	return (expected_data==current->data&&       
		expected_version==current->version&&     
		((new_data==current->data&&new_version==current->version)||pair.compare_exchange_weak(current, AtomicPair<T>::factory(new_data, new_version))))||
		(static_cast<bool>(expected_data=current->data)&&false);
}

template<typename T>
bool AtomicStampPointer<T>::compare_exchange_strong(T *&expected_data, T *new_data, unsigned int expected_version, unsigned int new_version)
{
	AtomicPair<T> *current=pair.load();     	 // ��ǰAtomicStampPointer�������AtomicPair 
	return (expected_data==current->data&&       
		expected_version==current->version&&     
		((new_data==current->data&&new_version==current->version)||pair.compare_exchange_strong(current, AtomicPair<T>::factory(new_data, new_version))))||
		(static_cast<bool>(expected_data=current->data)&&false);
}

template<typename T>
AtomicStampPointer<T>::~AtomicStampPointer()
{
}

#endif
