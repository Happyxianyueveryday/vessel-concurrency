#ifndef SEGMENT_H                            
#define SEGMENT_H          
#include <iostream>  
#include <string>
#include <vector>
#include <mutex>  

using namespace std;

template <typename T>
struct ListNode
{
    ListNode(T _val=T()):val(_val), next(NULL)
    {
    }
    const T val;        // ��ǰ���洢�Ĺ�ϣԪ�ض��󣬹�ϣ���ṩ�޸Ķ���Ĺ��ܣ���˽��ֵ����Ϊconst 
    ListNode *next;     // ָ��������һ������ָ��
};

/**
 * Segment: ������ʵ�ֵĹ�ϣ���� 
 */
template<typename T>      
class Segment 
{
    public:
    // 1. constructor
    Segment();                                // ���캯��
    Segment(const vector<T> &data);         // �����鹹���ϣ��

    // 2. copy/move_controller
    Segment(Segment &other);                   // �������캯��������� 
    Segment &operator= (Segment &other);       // ������ֵ������������ 
    Segment(Segment &&other);                  // �ƶ����캯�� ��ǳ����/�ƶ���
	Segment &operator= (Segment &&other);      // �ƶ���ֵ�������ǳ����/�ƶ��� 

    // 3. methods
    void insert(T value);              // ���ϣ���в���ָ��ֵ
    void remove(T value);              // �ӹ�ϣ�����Ƴ�ָ��ֵ
    bool contain(T value);             // �жϹ�ϣ�����Ƿ����ָ��ֵ
    vector<T> values();                // ��ù�ϣ��������ֵ���б�
    int length();                      // ��ȡ��ϣ���Ԫ�ظ��� 
	string toString();                 // �����ϣ��
    bool operator== (Segment &other);     // ����ж������

    // 4. destructor
    ~Segment();                                         // ��ϣ������������������

    private:
    // 5. domains
    ListNode<T> **data;           // ��ϣ������ṹ
    unsigned int capacity;        // ��ϣ��Ͱ��������������ĳ��ȣ����������У���ȡ��Ԫ�صĹ�ϣֵ�󽫹�ϣֵģͰ�������Ϳ��Եõ�����Ĵ�ŵ�Ͱ���±�λ��
    double load_factor;           // ��ϣ��������
    unsigned int threshold;       // ��ϣ��������ֵ����ֵ=Ͱ����*�������ӣ�ͨ������������ϣ������ݲ����ǣ���ִ��insert��������Ԫ�غ󣬼���ϣ���е�Ԫ�ظ���size�Ƿ������ֵthreshold�������ڣ������resize�����������ݣ���������ݷ�����μ�resize����
    unsigned int size;            // ��ǰ��ϣ���е�Ԫ�ظ���
    mutex mut;        			  // ��������Ҫ��֤ǿһ���ԣ������Ϊ��������������ʱ�����Դ���½� 

    // 6. private functions
    void resize();       // ��ϣ�����ݷ���
    unsigned int jenkins_prehash(unsigned int h) const; 
    unsigned int second_hash(unsigned int code) const;      // �ڶ��ι�ϣ 
};

/**
 * Segment: Ĭ�Ϲ��캯��
 */
template<typename T>
Segment<T>::Segment()
{
	size=0;    									// ��ʼ��Ԫ�ظ���Ϊ0
	capacity=16;        						// Ĭ��Ͱ��������ʼֵΪ16��16Ϊ2���ݣ�����ÿ�����ݾ�����2����Ͱ������Ϊ2���������ݣ���Ϊ����ʵ�飬Ͱ������2����������ʱԪ�ع�ϣ��Խɢ���ڸ���Ͱ���ܽ��ƾ��ȷֲ� 
	load_factor=0.75;   						// Ĭ�ϵĸ������ӳ�ʼֵΪ0.75
	threshold=capacity*load_factor;        		// ��ϣ��������ֵ����ֵʼ�յ���Ͱ�������Ը�������
	data=new ListNode<T>* [capacity];     	    // ��ʼ��Ͱ������ÿ��Ͱ�е��������һ��ͷ���ƽ�� 
	
	for(int i=0;i<capacity;i++)
	{
		data[i]=nullptr;                        // ��ʼ��ʱ����Ͱ��Ϊ�գ�ֻ�г��ֵ�һ����Ҫ�����Ԫ��ʱ�Ŵ���Ͱ 
	}
}

/**
 * Segment: �����鹹���ϣ���� 
 * param data: ���ڹ����ԭʼ�������� 
 */
template<typename T>
Segment<T>::Segment(const vector<T> &_data)
{
	size=0;    									// ��ʼ��Ԫ�ظ���Ϊ0
	capacity=16;        						// Ĭ��Ͱ��������ʼֵΪ16
	load_factor=0.75;   						// Ĭ�ϵĸ������ӳ�ʼֵΪ0.75
	threshold=capacity*load_factor;        		// ��ϣ��������ֵ����ֵʼ�յ���Ͱ�������Ը�������
	data=new ListNode<T>* [capacity];     	    // ��ʼ��Ͱ������ÿ��Ͱ�е��������һ��ͷ���ƽ��
	
	for(int i=0;i<capacity;i++)
	{
		data[i]=nullptr;
	}
	
	for(int i=0;i<_data.size();i++)
	{
		this->insert(_data[i]);
	}
}

/**
 * Segment: �������캯�� 
 */
template<typename T>
Segment<T>::Segment(Segment &other)
{
	lock_guard<mutex> other_guard(other.mut);
	
	size=other.size;    									
	capacity=other.capacity;        						
	load_factor=other.load_factor;   						
	threshold=capacity*load_factor;
	data=new ListNode<T>* [capacity];     		
	
	for(int i=0;i<capacity;i++)
	{
		if(other.data[i])
		{
			data[i]=new ListNode<T>();
			ListNode<T> *now=data[i], *othnow=other.data[i]->next;
			while(othnow)
			{
				now->next=new ListNode<T>(othnow->val);   // ʹ�ÿ������캯�����߿�����ֵ�����ʱ���������ϣ��Ԫ�� 
				now=now->next;
				othnow=othnow->next;
			}
		}
		else
		data[i]=nullptr;
	}
	other.read_unlock();
}

/**
 * operator=: ������ֵ����� 
 */
template<typename T>
Segment<T> &Segment<T>::operator= (Segment &other)
{
	if(this==&other)
	return (*this);
	
	lock_guard<mutex> guard(mut);
	lock_guard<mutex> other_guard(other.mut);
	for(int i=0;i<capacity;i++)
	{
		ListNode<T> *now=data[i];
		while(now)
		{
			ListNode<T> *temp=now;
			now=now->next;
			delete temp;
		}
	}
	delete data;
	
	size=other.size;    									
	capacity=other.capacity;        						
	load_factor=other.load_factor;   						
	threshold=capacity*load_factor;
	data=new ListNode<T>* [capacity];     		
	
	for(int i=0;i<capacity;i++)
	{
		if(other.data[i])
		{
			data[i]=new ListNode<T>();
			ListNode<T> *now=data[i], *othnow=other.data[i]->next;
			while(othnow)
			{
				now->next=new ListNode<T>(othnow->val);   // ʹ�ÿ������캯�����߿�����ֵ�����ʱ���������ϣ��Ԫ�� 
				now=now->next;
				othnow=othnow->next;
			}
		}
		else
		data[i]=nullptr;
	}
	return (*this);
}

/**
 * Segment: �ƶ����캯�� 
 */
template<typename T>
Segment<T>::Segment(Segment<T> &&other) 
{
	lock_guard<mutex> other_guard(other.mut);
	// 1. �ӿ���Դ����ǳ������Ա������ֵ����ǰ���� 
	size=other.size;           	     // ��ϣ��Ԫ�ظ���  
	capacity=other.capacity;   		 // ��ϣ��Ͱ����
	load_factor=other.load_factor;   // װ������
	threshold=capacity*load_factor;  // ������ֵ 
	data=other.data;                 // �ײ�洢�ṹָ��
	
	// 2. ������Դ����ĳ�Ա������ֵ�趨ΪĬ��ֵ����Ĭ�Ϲ��캯���е�ȡֵ��
	other.size=0;    								
	other.capacity=16;        					
	other.load_factor=0.75;   					
	other.threshold=capacity*load_factor;        		
	other.data=new ListNode<T>* [capacity];     			
	
	for(int i=0;i<capacity;i++)
	{
		other.data[i]=nullptr;
	}
}

/**
 * operator=: �ƶ���ֵ�����������cpp11���ƶ������ṩ��ȫ��ǳ����/�ƶ��ֶ�  
 */ 
template<typename T>
Segment<T> &Segment<T>::operator= (Segment &&other)
{
	// 1. �ж��Ƿ�Ϊ����ֵ
	if(this==&other)
	return (*this);
	
	lock_guard<mutex> guard(mut);
	lock_guard<mutex> other_guard(other.mut);
	// 2. �ͷ����������ʹ�õĶѿռ���Դ
	for(int i=0;i<capacity;i++)
	{
		ListNode<T> *now=data[i];
		while(now)
		{
			ListNode<T> *temp=now;
			now=now->next;
			delete temp;
		}
	}
	
	// 3. �ӿ���Դ����ǳ������Ա������ֵ����ǰ���� 
	size=other.size;           // ��ϣ��Ԫ�ظ��� 
	capacity=other.capacity;   // ��ϣ��Ͱ����
	load_factor=other.load_factor;   // װ������
	threshold=capacity*load_factor;  // ������ֵ 
	data=other.data;                 // �ײ�洢�ṹָ��
	
	// 4. ������Դ����ĳ�Ա������ֵ�趨ΪĬ��ֵ����Ĭ�Ϲ��캯���е�ȡֵ��
	other.size=0;    								
	other.capacity=16;         	 				
	other.load_factor=0.75;   					
	other.threshold=capacity*load_factor;        		
	other.data=new ListNode<T>* [capacity];     			
	for(int i=0;i<capacity;i++)
	{
		other.data[i]=nullptr;
	}
	
	// 5. ���ص�ǰ���������
	return (*this);
}

/**
 * insert: ���ϣ�����в���Ԫ��ֵ
 * note: �������Ԫ��ֵ�ڹ�ϣ�������Ѿ������ظ����򲻻���в���
 */
template<typename T>
void Segment<T>::insert(T value) 
{
	// 1. �������ֵ�Ѿ������ڹ�ϣ�����У���������в��� 
	if(this->contain(value))
	return;
	
	lock_guard<mutex> guard(mut);
	// 2. ����Ӧ�������Ͱλ��
	unsigned int index=second_hash(value.hashCode());
	// 4. ��������뵽��Ӧ��Ͱ��
	if(!data[index])   // ��Ϊ��һ�����뵽��Ͱ��Ԫ�أ�����Ҫ���ȴ���Ͱ��ͷ���ƽ�� 
	data[index]=new ListNode<T>();
	ListNode<T> *now=data[index];
	ListNode<T> *newnode=new ListNode<T>(value);
	newnode->next=now->next;
	now->next=newnode;
	// 5. ����Ԫ�ؼ������ж��Ƿ���Ҫ���ݣ���Ԫ������size������ֵthreshold�������resize������������
	size+=1;
	if(size>threshold)
	resize();
	return; 
}

/**
 * remove: �ӹ�ϣ������ɾ��ָ��ֵ��Ԫ��
 * note: ����ϣ�����в����ڸ�ֵ����ʵ���ϲ������ɾ�� 
 */
template<typename T>
void Segment<T>::remove(T value) 
{
	lock_guard<mutex> guard(mut);
	// 1. �����ɾ����ֵ�ڹ�ϣ���е�λ��
	unsigned int index=second_hash(value.hashCode()); 
	// 2. ������Ͱ�е���������Ŀ��ֵ�Ĺ�ϣ��Ԫ�أ�ǰ��˫ָ�뷨����ɾ�� 
	if(!data[index])   // ����Ͱ��û��Ԫ�أ���ֱ�ӷ��� 
	return;
	ListNode<T> *nowprev=data[index];   
	ListNode<T> *now=data[index]->next;
	while(now)
	{
		// �Ƚ�ʱʹ�úͲ�����ͬ���㷨����ʹ��hashCode�����Ƚϣ���������ٵ���equals�����Ƚϣ����������ֱ����Ϊ�Ƚϵ�����Ԫ�ز���� 
		if(now->val.hashCode()==value.hashCode()&&now->val.equals(value))  
		{
			nowprev->next=now->next;
			delete now;   // ����ListNode����������ϣԪ�صĲ����Ѿ���ListNode������������������ˣ������cpp��RAIIԭ�� 
			size-=1;
			break;
		}	
		else
		{
			nowprev=now;
			now=now->next;
		}
	}
	return;
}

/**
 * contain: �жϸ���ֵ�Ƿ��ڹ�ϣ����
 * note: ������ֵ�ڹ�ϣ�����򷵻�true�����򷵻�false 
 */
template<typename T>
bool Segment<T>::contain(T value)
{
	lock_guard<mutex> guard(mut);   
	// 1. ����Ӧ�����ڵ�Ͱλ��
	unsigned int index=second_hash(value.hashCode());
	// 2. �Ӹ�Ͱ�ĵ�һ�������㿪ʼ���ν��бȽ�
	// note: �Ƚ��㷨�ǣ����ȵ���hashCode������������бȽϣ���hashCodeֵ��ȣ������==��������н�һ���Ƚ��ж��Ƿ���ȣ���hashCode����ȣ���ֱ����Ϊ����������� 
	if(!data[index]) 
	return false;
	ListNode<T> *now=data[index]->next; 
	while(now)
	{
		if(now->val.hashCode()==value.hashCode()&&now->val.equals(value))  // �ȱȽ�hashCode()�����ķ���ֵ�Ƿ���ȣ�������ٱȽ�==���������н�һ���Ƚ� 
		return true;
		else
		now=now->next;   
	}
	return false;
}

/**
 * values: ��ȡ��ϣ�����е�����ֵ���б�
 * return: ��ϣ�����е�����Ԫ�ص��б� 
 */
template<typename T>
vector<T> Segment<T>::values()
{
	lock_guard<mutex> guard(mut);
	vector<T> res;
	for(int i=0;i<capacity;i++)
	{
		if(data[i])
		{
			ListNode<T> *now=data[i]->next;
			while(now)
			{
				res.push_back(now->val);
				now=now->next;
			}
		}
	}
	return res;
}

/**
 * length: ��ȡ��ϣ���Ԫ�ظ��� 
 */
template<typename T>
int Segment<T>::length()
{
	lock_guard<mutex> guard(mut);
	return size;
}

/**
 * toString: ���ɹ�ϣ�������ַ��� 
 */
template<typename T>
string Segment<T>::toString()
{
	lock_guard<mutex> guard(mut);
    string res;
    for(int i=0;i<capacity;i++)
    {
    	if(i==0)
    	res+="*";
    	else
    	res+="\n*";
    	
    	if(data[i])
    	{
    		const ListNode<T> *now=data[i]->next;
    		while(now)
    		{
    			res=res+"->"+now->val.toString();
    			now=now->next;
			}
			res=res+"->NULL";
		}
	}
	return res;
}

/** 
 * operator==: �ж�������ϣ�����Ƿ����
 * note: �ж�������������ϣ���Ƿ���ȵ��㷨���ںܴ���Ż��ռ䣬��Ҫ������ȵ��Ż������������ơ�ͬʱ��Ҫ���ѵ��ǣ���Ϊ����ɾ���Ȳ�����˳����ͬ��������ͨ��ֱ�ӱȽ�������ϣ��Ľṹ���Ƚ�������ϣ���Ƿ����
 * note: �������ʹ�õıȽ��㷨���£�
 *       1. �����жϴ����otherָ���ַ�Ƿ��thisָ����ȣ�����ȣ���ֱ�ӷ���true��������벽��2
 *		 2. Ȼ���жϴ���Ĺ�ϣ�����е�Ԫ�������Ƿ�͵�ǰ��ϣ������ȣ�������ȣ���ֱ�ӷ���true��������벽��3
 *       3. ��������жϵ�ǰ��ϣ�����е�ÿһ��Ԫ���Ƿ��ڹ�ϣ����other�У��������ڣ���ֱ�ӷ���true�����򷵻�false		 
 */ 
template<typename T>
bool Segment<T>::operator== (Segment &other)
{
	lock_guard<mutex> guard(mut);
	lock_guard<mutex> other_guard(other.mut);
	if(&other==this)
	return true;
	else if(this->length()!=other.length())
	return false;
	for(int i=0;i<capacity;i++)
	{
		if(data[i])
		{
			ListNode<T> *now=data[i]->next;
			while(now)
			{
				if(this->contain(now->val))
				now=now->next;
				else
				return false;
			}
		}
	}
	return true;
}

/**
 * resize: ��ϣ���ݷ���
 * note 1: ��ʱ������ϣ���ݣ�
 *         ��ֻҪ��ִ�в�������󣬹�ϣ���е�Ԫ������size����Ͱ������capacity����һ����������ϵ��(load_factor)ʱ�ͷ�����ϣ���ݣ�����������Ϊ: size>capacity*load_factor 
 * note 2: ��ϣ���ݾ����㷨���裺 
 *         ��1. ��Ͱ������(capacity)����Ϊԭ�ȵ���������: capacity=2*capacity
 *			   2. ���¼�����ֵ����: threshoud=capacity*load_factor������load_factorΪ�������ӣ��̶�Ϊ0.75 
 *			   3. �����µ�Ͱ����
 *			   4. ����Ͱ�����е���������֮ǰ��ͬ�㷨���¼����ϣֵ�Ͳ���λ�ã���ʹ��ǳ�����ƶ�����Ͱ���飬��: index=(element->hashCode())%capacity 
 *			   5. �ͷž�Ͱ���飬�µ�Ͱ������Ϊ��ǰ��ϣ���Ͱ���� 
 */
template<typename T>
void Segment<T>::resize()  
{
	// 1. ��Ͱ����capacity�ӱ�
	unsigned int old_capacity=capacity; 
	capacity*=2;
	// 2. ���¼�����ֵthreshoud
	threshold=capacity*load_factor;
	// 3. �����µ�Ͱ���飬���ҽ�ԭͰ�����е�ÿ��Ԫ������ʹ����ԭ����ͬ�Ĺ�ϣ�㷨���뵽�µ�Ͱ������
	ListNode<T> **new_data=new ListNode<T>* [capacity]; 
	for(int i=0;i<capacity;i++)
	{
		new_data[i]=nullptr;
	}
	for(int i=0;i<old_capacity;i++)
	{
		if(data[i])
		{
			ListNode<T> *now=data[i]->next;
			ListNode<T> *nowprev=data[i];
			
			while(now)
			{
				unsigned int index=second_hash(now->val.hashCode());        // �����µĹ�ϣ��ַ��λ�� 
				nowprev->next=now->next;                  // ��ԭ��ϣ�����Ƴ��ý��
				if(!new_data[index])                      // ����ǰ���ж��Ƿ����Ͱ���������򴴽�Ͱ 
				new_data[index]=new ListNode<T>();      
				now->next=new_data[index]->next;          // ���뵽�µ�Ͱ����������� 
				new_data[index]->next=now;
				now=nowprev->next;                        // �����α�ָ��now 
			}
			delete data[i];       // ɾ��ԭͰ�����е�ͷ�������ƽ�� 
		}
	}
	delete [] data;
	data=new_data;
}  

/** 
 * second_hash: �ڶ��ι�ϣ�����ι�ϣ�� 
 * note 1: ��Ͱ��������capacity��ֵΪ2����ʱ��ʼ���У�code%capacity==code&(capacity)���Ӷ������������� 
 * note 2: �ڶ��ι�ϣ��Ŀ����ȷ��Ҫ������Ͱ��λ���±� 
 * note 3: ���ι�ϣ���㷨��һ�����ڶ��ι�ϣ�Ĺ��̸���������ͨ�ķ��̰߳�ȫ�Ĺ�ϣ���������ι�ϣ֮ǰ���Ƽ���jenkinsԤ���� 
 */ 
template<typename T>
unsigned int Segment<T>::second_hash(unsigned int code) const
{
	return jenkins_prehash(code)&(capacity-1);
}

/**
 * jenkins_prehash: ��ϣԤ����
 * note: ���ι�ϣ֮ǰ����Ҫ���й�ϣԤ����Ԥ�����Ŀ���ǳ�ֻ��ӣ��Ӷ��Ż���ϣ��Ч�� 
 */
template<typename T>
unsigned int Segment<T>::jenkins_prehash(unsigned int h) const
{
	h += (h << 15) ^ 0xffffcd7d; 
	h ^= (h >> 10);
	h += (h << 3); h ^= (h >> 6);
	h += (h << 2) + (h << 14); 
	return h ^ (h >> 16);
}
   
/**
 * ~Segment: ���������� 
 */
template<typename T>
Segment<T>::~Segment()
{
	for(int i=0;i<capacity;i++)
	{
		ListNode<T> *now=data[i];
		while(now)
		{
			ListNode<T> *temp=now;
			now=now->next;
			delete temp;
		}
	}
	delete data;
}

#endif
