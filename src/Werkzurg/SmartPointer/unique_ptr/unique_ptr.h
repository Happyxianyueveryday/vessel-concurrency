#ifndef UNIQUE_PTR_H 
#define UNIQUE_PTR_H
#include <string>   

template<typename T>
class unique_ptr
{
	public:
	// 1. constructor
	unique_ptr();            // Ĭ�Ϲ��캯��������ָ��nullptr������ָ�� 
	unique_ptr(T *_data);    // ���캯��������ָͨ�봴������ָ�룬֮����Դ������Ȩת�����½�������ָ����� 
	
	// 2. copy/move controller
	unique_ptr(const unique_ptr &other) = delete;       		// unique_ptr���ÿ������캯�� 
	unique_ptr &operator= (const unique_ptr &other)=delete;     // unique_ptr���ÿ�����ֵ����� 
	unique_ptr(unique_ptr &&other);                     		// �ƶ����캯�� 
	unique_ptr &operator= (unique_ptr &&other);         		// �ƶ���ֵ����� 
	
	// 3. methods
	bool empty() const;      // �жϵ�ǰ����ָ���Ƿ�Ϊ�� 
	T *release();            // ת������Ȩ������ָ���ͷŶԵ�ǰ���������Ȩ�������ض���ָ�� 
    void reset(T *data);     // �ͷ�����Ȩ������ָ����������Ķ��������Ȩ��������ԭ�����еĶ���
    void reset();            // �ͷ�����Ȩ������ָ������ԭ�����еĶ��󣬲�����������Ȩ��Ϊnullptr 
	T &operator* () const;   // ������������������������Ӧ��ʼ�շ��ص�ǰ��������� 
	T *operator-> () const;  // ��ͷ���������ͷ�����Ӧ��ʼ�շ���ָ��ǰ�����ָ�룬��thisָ�� 
	
	// 4. destructor
    ~unique_ptr();           // ����������ͨ�����ٵ�ǰ���еĶ��󣬴Ӷ��ͷŶԵ�ǰ���еĶ��������Ȩ
	
	private:
	// 5. destructor
	T *data;                 // ��ǰ����ָ�������еĶ���ָ�� 
};

/**
 * unique_ptr: Ĭ�Ϲ��캯�� 
 * note: Ĭ�Ϲ��캯������һ���������κζ����unique_ptr 
 */
template<typename T>
unique_ptr<T>::unique_ptr()
{
	data=nullptr;
}

/** 
 * unique_ptr: ��ͨ���캯��
 * note: ��ͨ���캯����ʼ��һ����������ָ��ָ������unique_ptr��֮�������ָ�벻�پ��жԸö��������Ȩ 
 */
template<typename T>
unique_ptr<T>::unique_ptr(T *_data)
{
	data=_data;
}

/**
 * unique_ptr: �ƶ����캯��
 * note: �ƶ����캯�����ƶ�Դָ�������еĶ��������Ȩת�Ƶ���ǰ�����������ָ����
 */
template<typename T>
unique_ptr<T>::unique_ptr(unique_ptr &&other)
{
	// 1. ��Դָ��ǳ��������ָ�룬ת������Ȩ 
	data=other.data;
	// 2. ����Դָ��Ķ���ָ��ΪĬ��ֵ���ͷ�����Ȩ 
	other.data=nullptr;
}

/** 
 * operator=: �ƶ���ֵ�����
 * note: �ƶ���ֵ��������ƶ�Դָ�������еĶ��������Ȩת�Ƶ���ǰָ���ϣ�ͬʱ�ͷŵ�ǰָ�����������е���Դ 
 */
template<typename T>
unique_ptr<T> &unique_ptr<T>::operator= (unique_ptr &&other)
{
	// 1. �ж��Ƿ�Ϊ����ֵ
	if(&other==this)
	return (*this);
	// 2. �жϵ�ǰָ���Ƿ���ж��������ж���������ͷ�
	if(data)
	delete data; 
	// 3. ��Դָ��ǳ��������ָ�룬ת������Ȩ 
	data=other.data;
	// 4. ����Դָ��Ķ���ָ��ΪĬ��ֵ���ͷ�����Ȩ 
	other.data=nullptr;
	// 5. ���ص�ǰ����ָ��
	return (*this);
}

/** 
 * empty: �жϵ�ǰ����ָ���Ƿ���ж���/�Ƿ�Ϊ�� 
 */
template<typename T>
bool unique_ptr<T>::empty() const
{
	if(data)
	return false;
	else
	return true;
}

/**
 * release: ��ָ���ͷ������ж��������Ȩ
 * return: ��������ָ�������ж����ԭʼָ�� 
 */
template<typename T>	
T *unique_ptr<T>::release()
{
	T *temp=data;
	data=nullptr;
	return temp;
}

/**
 * reset: ���غ������ͷŵ�ǰָ��ԭ�������еĶ���Ȼ���������ָ��ָ��Ķ��� 
 */
template<typename T>
void unique_ptr<T>::reset(T *_data)
{
	// 1. �ͷŵ�ǰָ��ԭ�����еĶ��� 
	if(data)
	delete data;	
	// 2. �ǰָ���������ָ��ָ��Ķ���
	data=_data;	 
}

/**
 * reset: ���غ������ͷŵ�ǰָ�������еĶ��� 
 */
template<typename T>
void unique_ptr<T>::reset()
{
	if(data)
	delete data;
	data=nullptr;
}

/**
 * operator*: �����������  
 */
template<typename T>
T &unique_ptr<T>::operator* () const
{
	return *data;
}

/** 
 * operator->: ��ͷ����� 
 */
template<typename T>
T *unique_ptr<T>::operator-> () const
{
	return data;
}

/**
 * ~unique_ptr:  
 */
template<typename T>
unique_ptr<T>::~unique_ptr()
{
	if(data)
	delete data;
}

#endif
