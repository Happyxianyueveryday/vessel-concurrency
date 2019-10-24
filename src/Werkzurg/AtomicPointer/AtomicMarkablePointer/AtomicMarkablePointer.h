#ifndef ATOMIC_MARKABLE_POINTER_H 
#define ATOMIC_MARKABLE_POINTER_H 
#include "AtomicPair.h" 
#include <atomic>

/**
 * AtomicMarkablePointer: ���а汾�ŵ�ԭ��ָ��ģ�� 
 * note: AtomicMarkablePointer�����ǰ�װ�˰汾�ŵ��̰߳�ȫָ�룬��������ָ����ָ�����Ļ��� 
 */
template<typename T>
class AtomicMarkablePointer
{
	public:
	// 1. constructor
	AtomicMarkablePointer(T *_data);   // ���캯�� 
	
	// 2. copy/move controller
	AtomicMarkablePointer(const AtomicMarkablePointer &other) = delete;               // ԭ��ģ����ÿ������ƶ� 
	AtomicMarkablePointer(AtomicMarkablePointer &&other) = delete;
	AtomicMarkablePointer &operator= (const AtomicMarkablePointer &other) = delete;
	AtomicMarkablePointer &operator= (AtomicMarkablePointer &&other) = delete;
	
	// 3. methods
	bool is_lock_free();    // �ж��Ƿ�֧������ԭ�Ӳ�������֧�����ʹ��������
	bool get_flag();
	T* get_ptr();
	bool compare_exchange_weak(T *expected_data, T *new_data, bool expected_flag, bool new_flag);
	bool compare_exchange_strong(T *expected_data, T *new_data, bool expected_flag, bool new_flag); 
	
	// 4. destructor
	~AtomicMarkablePointer(); 
	
	private:
	// 5. domain
	std::atomic<AtomicPair<T> *> pair; 
}; 

/**
 * AtomicMarkablePointer: ���캯�� 
 */
template<typename T>
AtomicMarkablePointer<T>::AtomicMarkablePointer(T *_data)
{
	pair=AtomicPair<T>::factory(_data, 0);
}

/**
 * is_lock_free: �жϸ�ԭ�Ӱ汾ָ������в����Ƿ�Ϊ������ 
 * note: ֻҪ��װ��AtomicPair<T> *ָ������в�����������ԭ�Ӱ汾ָ��AtomicMarkablePointer�Ĳ����������� 
 */
template<typename T>
bool AtomicMarkablePointer<T>::is_lock_free()
{
	return pair.is_lock_free();  
}

/**
 * get_stamp: ��ȡ��ǰ�İ汾�� 
 */
template<typename T>
bool AtomicMarkablePointer<T>::get_flag()
{
	return pair.load()->flag;
}

/**
 * get_ptr: ��ȡ��װ����ָ�� 
 */
template<typename T>
T* AtomicMarkablePointer<T>::get_ptr()
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
 */
template<typename T>
bool AtomicMarkablePointer<T>::compare_exchange_weak(T *expected_data, T *new_data, bool expected_flag, bool new_flag)
{
	AtomicPair<T> *current=pair.load();     	// ��ǰAtomicMarkablePointer�������AtomicPair 
	return expected_data==current->data&&      
		expected_flag==current->flag&&     
		((new_data==current->data&&new_data==current->data)||pair.compare_exchange_weak(current, AtomicPair<T>::factory(new_data, new_flag)));       
}

template<typename T>
bool AtomicMarkablePointer<T>::compare_exchange_strong(T *expected_data, T *new_data, bool expected_flag, bool new_flag)
{
	AtomicPair<T> *current=pair.load();     	 // ��ǰAtomicMarkablePointer�������AtomicPair 
	return expected_data==current->data&&      
		expected_flag==current->flag&&    
		((new_data==current->data&&new_flag==current->flag)||pair.compare_exchange_strong(current, AtomicPair<T>::factory(new_data, new_flag)));       // ���ö�·��ֵ����CAS 
}

template<typename T>
AtomicMarkablePointer<T>::~AtomicMarkablePointer()
{
}

#endif
