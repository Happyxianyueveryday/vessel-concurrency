#ifndef SHARED_PTR_H 
#define SHARED_PTR_H
#include <string>

/**
 * shared_ptr: ����ָ����ģ�� 
 */
template<class T> class shared_ptr
{
	public: 
	// 1. constructor
	shared_ptr();     			       // Ĭ�Ϲ��캯��
	
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
	
	private:
	// 5. domain
	T *data;                          // ����ָ����ָ��Ķ������ָ�� 
	int *count;                       // ��������ü��� 
};

/**
 * shared_ptr: ��ʼ��һ������ָ��ָ���Ӧ���͵��¶��� 
 */
template<typename T>
shared_ptr<T>::shared_ptr()
{
	data=new T();       // ��ʼ������ָ�� 
	count=new int(1);   // ��ʼ�����ü�����ע�����ʹ�ü�����ָ�룬ԭ��ܺ���⣬���ﲻ�ٽ��� 
}
	
/** 
 * shared_ptr: �������캯�� 
 * note: �������캯�����Ƚ�Դָ������ü�����1, Ȼ��ǳ��������Ҫ��Դָ��Ķ���ָ������ü���ָ�롣��Ϊ����Դָ����ָ��Ķ����������������1 
 */
template<typename T>
shared_ptr<T>::shared_ptr(shared_ptr &other)
{
	// 1. ��Դָ�����ü�������1 
	*(other.count)+=1;
	// 2. ǳ����Դָ��Ķ���ָ������ü���ָ��
	data=other.data; 
	count=other.count;
}

/**
 * operator=: ������ֵ�����  
 * note: ��Ϊ��ֵ�����ǽ��Ⱥ��Ҳ��ֵ��ֵ����ߣ���ˣ�
 *       1. ������ֵ��������Ƚ�Դָ����ָ���������ü�������1��Ȼ�󽫵�ǰָ����ָ���������ü�����С1
 *	     2. ����С1�����0�����ͷŵ�ǰָ��ָ��Ķ���
 *       3. ����Դָ��ǳ������Ա 
 */
template<typename T>
shared_ptr<T> &shared_ptr<T>::operator= (shared_ptr &other)  
{
	// 1. �ж��Ƿ�Ϊ�������� 
	if(&other==this)
	return (*this);
	// 2. ��Դָ����ָ���������ü�����1������ǰָ����ָ���������ü�����1 
	*(other.count)+=1;
	*count-=1;
	// 3. ����С1��ǰָ��ָ��������ü���Ϊ0����������ָ��Ķ��� 
	if(*count==0)
	{
		delete data;
		delete count;
	}
	// 4. ��Դ����ǳ����Դ����ָ������ü���ָ�� 
	data=other.data;
	count=other.count;
	// 5. ���ص�ǰָ��
	return (*this);
}

/**
 * shared_ptr: �ƶ����캯�� 
 * note: ����ָ����ƶ�����Ƚ����⣬�ƶ����캯����Ҫ���еĲ�����: ����ǳ����Դָ������г�Ա��Ȼ��Դָ���Ա����ΪĬ��ֵ��Ĭ�Ϲ��캯���и���Ա�ĸ�ֵ��
 * note: �Ϳ������캯����ȣ��ƶ����캯������Ҫ����Դָ��ָ���������ü�������Ϊ�ƶ�����û�в����µ�ָ��Դָ������ָ�� 
 */
template<typename T>
shared_ptr<T>::shared_ptr(shared_ptr &&other)
{
	// 1. ǳ����Դָ��ĳ�Ա 
	data=other.data;
	count=other.count;
	// 2. ����Դָ��ĳ�ԱΪĬ��ֵ
	other.data=new T();
	other.count=new int(1);
}

/**
 * operator=: �ƶ���ֵ�����
 * note: �ƶ���ֵ��������ƶ�����Ƚ����⣬����Ҫ���еĲ����ǣ�
 *       1. ���Ƚ���ǰָ����ָ���������ü�����1������1�����ü���Ϊ0��Զ����������
 *       2. Ȼ��ǳ����Դָ������г�Ա
 *       3. ���Դָ�����г�Ա����ΪĬ��ֵ 
 * note: �Ϳ�����ֵ�������ȣ��ƶ���ֵ����������˵�ǰָ������ü��������ǲ�����Դָ������ü��� 
 */
template<typename T>
shared_ptr<T> &shared_ptr<T>::operator= (shared_ptr && other)
{
	// 1. �ж��Ƿ�Ϊ����ֵ
	if(&other==this)
	return (*this);
	// 2. ����ǰָ��ָ����������ֵ��С1�������С��ֵΪ0�����ͷŵ�ǰ��Դ 
	(*count)-=1;
	if(*count==0)
	{
		delete data;
		delete count;
	}
	// 3. ǳ����Դָ������г�Ա 
	data=other.data;
	count=other.count;
	// 4. ����Դָ��ĳ�ԱΪ��ʼֵ
	other.data=new T();
	other.count=new int(1);
	// 5. ���ص�ǰָ�� 
	return (*this);
}

/**
 * operator*: �����������
 * note: �����������ֱ�ӷ��ص�ǰ��������ü��� 
 */ 
template<typename T>
T &shared_ptr<T>::operator* () const
{
	return *data;
}

/**
 * operator->: ��ͷ����� 
 * note: ��ͷ�����ֱ�ӷ��ص�ǰָ����ָ��Ķ����ָ�뼴�� 
 */ 
template<typename T>
T *shared_ptr<T>::operator-> () const
{
	return data;
}

/**
 * unique: �жϵ�ǰָ��ָ���������ü����Ƿ����1 
 */
template<typename T>
bool shared_ptr<T>::unique() const
{
	if(*count==1)
	return true;
	else
	return false;
}

/**
 * ~shared_ptr: ��������    
 * note: �����������Ƚ����ü�����С1������С��ֵΪ0���ͷŶ��󣬷����ͷŶ��� 
 */
template<typename T>
shared_ptr<T>::~shared_ptr()
{
	*count-=1;
	if(*count==0)
	{
		delete data;
		delete count;
	}
}

#endif
