#ifndef PTR_COUNTER_H 
#define PTR_COUNTER_H
#include <string>

template<typename T>    // ��Ԫģ��ǰ������ 
class weak_ptr;

/** 
 * ptr_counter: ���ü�����
 * note: ����ά����������weak_count��shared_count���ֱ����ָ��ǰ�����weak_ptr��������shared_ptr������
 * note: weak_ptr�ĺ���ԭ���ǣ���shared_ptr�Ͷ�Ӧ��weak_ptr��ͬ����ptr_counter����
 	    1. ��shared_count������Ϊ0ʱ����shared_ptr�����ͷż������Ķ���ָ�룻
		2.��shared_count��weak_count������ͬʱΪ0ʱ����weak_ptr����shared_ptr�ͷ�ptr_counter���������� 
 */
template<typename T>
class ptr_counter
{
	public:
	// 1. constructor 
	ptr_counter();         // Ĭ�Ϲ��캯�� 
	ptr_counter(T *_data); // ���캯�� 
	
	// 2. copy/move controller 
	ptr_counter(const ptr_counter<T> &other) = delete;             		// ���ÿ������캯�� 
	ptr_counter<T> &operator= (const ptr_counter<T> &other) = delete;   // ���ÿ�����ֵ�����
	ptr_counter(ptr_counter<T> &&other) = delete;              		    // �����ƶ����캯��
	ptr_counter<T> &operator= (ptr_counter<T> &&other) = delete;  		// �����ƶ���ֵ�����
	
	// 3. methods 
	void shared_plus();    // ����ָ��ǰ�����shared_ptr�ļ��� 
	void weak_plus();      // ����ָ��ǰ�����weak_ptr�ļ���
	void shared_sub();     // ����ָ��ǰ�����shared_ptr�ļ���
	void weak_sub();       // ����ָ��ǰ�����weak_ptr�ļ��� 
	T* getdata() const;          	// ��ȡ��ǰ���������󶨵Ķ���
	int get_shared_count() const; 	// ��ȡshared_ptr�ļ��� 
	int get_weak_count() const;   	// ��ȡweak_ptr�ļ���
	void free_data();      // �ͷ����õĶ��󣬵��ǲ��������������� 
	
	// 4. destructor 
	~ptr_counter();        // ��������
	
	friend class weak_ptr<T>;
	
	private:
	// 5. domains
	T* data;
	int *shared_count;
	int *weak_count;
};

/**
 * ptr_counter: Ĭ�Ϲ��캯�� 
 */
template<typename T>
ptr_counter<T>::ptr_counter()
{
	data=new T();
	shared_count=new int(1);
	weak_count=new int(0);
}

/**
 * ptr_counter: ���캯�� 
 * param data: ��Ҫ���õĶ��� 
 * note: ����shared_ptr��Ȩ����ptr_counter�Ĺ��캯�� 
 */
template<typename T>
ptr_counter<T>::ptr_counter(T *_data)
{
	data=_data;
	shared_count=new int(1);
	weak_count=new int(0);
}

/**
 * shared_plus: ����shared_ptr�ļ����� 
 */
template<typename T>
void ptr_counter<T>::shared_plus()
{
	*shared_count+=1;
} 

/**
 * shared_plus: ����weak_ptr�ļ����� 
 */
template<typename T>
void ptr_counter<T>::weak_plus()
{
	*weak_count+=1; 
} 

/** 
 * shared_sub: ��Сshared_ptr�ļ�����
 */ 
template<typename T>
void ptr_counter<T>::shared_sub()
{
	*shared_count-=1;
} 

/** 
 * weak_sub: ��Сweak_ptr�ļ�����
 */ 
template<typename T>
void ptr_counter<T>::weak_sub()
{
	*weak_count-=1;
}

/**
 * getdata: ��ȡ��ǰ���ö����ָ�� 
 */
template<typename T>
T* ptr_counter<T>::getdata() const
{
	return data;
}

/**
 * get_shared_count: ��ȡshared_ptr�ļ����� 
 */
template<typename T>
int ptr_counter<T>::get_shared_count() const
{
	return *shared_count;
}

/**
 * get_weak_count: ��ȡweak_ptr�ļ����� 
 */
template<typename T>
int ptr_counter<T>::get_weak_count() const
{
	return *weak_count;
}

/**
 * free_data: ���ͷż��������õĶ��󣬲��ͷż���������   
 */
template<typename T> 
void ptr_counter<T>::free_data()
{
	if(data)
	delete data;
	data=nullptr;   // ������������ָ��Ϊ�� 
}

/**
 * ~ptr_count: ����������ͬʱ������������������ж��� 
 */
template<typename T>	
ptr_counter<T>::~ptr_counter()
{
	if(data)
	delete data;
	delete shared_count;
	delete weak_count;
}

#endif 
