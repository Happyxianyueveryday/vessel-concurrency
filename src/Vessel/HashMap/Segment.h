#ifndef SEGMENT_H          
#define SEGMENT_H
#include <string>
#include <vector> 
#include <mutex>

/** 
 * HashEntry: ��ϣ���д洢�ļ�ֵ�� 
 * typename T: ��ϣ���������
 * typename R: ��ϣ��ֵ������ 
 */ 
template <typename T, typename R>
struct HashEntry
{
    HashEntry(T _key=T(), R _val=R()):key(_key), val(_val), next(nullptr)
    {
    }
    const T key;        // ������ֵ���еļ������޸�
    R val;              // ֵ����ֵ���е�ֵ�����޸�  
    HashEntry *next;    // ָ��������һ�����/��ֵ�Ե�ָ��
};

/**
 * Segment: �����������ĵ��̹߳�ϣ�� 
 * typename T: ��ϣ���������
 * typename R: ��ϣ��ֵ������ 
 */
template <typename T, typename R>
class Segment
{
    public:
    // 1. constructor
    Segment();                                                 // Ĭ�Ϲ��캯��������յĹ�ϣ�� 
    Segment(const std::vector<T> &_keys, const std::vector<R> &_values); // �����鹹���ϣ��

    // 2. copy/move_controller
    Segment(const Segment &other);                   // �������캯��������� 
    Segment &operator= (const Segment &other);       // ������ֵ������������ 
    Segment(Segment &&other);                        // �ƶ����캯�� ��ǳ����/�ƶ���
	Segment &operator= (Segment &&other);            // �ƶ���ֵ�������ǳ����/�ƶ��� 

    // 3. methods
    void insert(T key, R value);   // ���ϣ���в���ָ��ֵ
    void remove(T key);            // �ӹ�ϣ�����Ƴ�ָ��ֵ
    bool contain(T key);           // �жϹ�ϣ�����Ƿ����ָ��ֵ
    bool get(T key, R &value);     // ���ҹ�ϣ���и�������Ӧ��ֵ 
    std::vector<T> keys();         // ��ù�ϣ��������ֵ���б�
    int length();                  // ��ȡ��ϣ���Ԫ�ظ��� 
	std::string toString();        // �����ϣ��
	R &operator[] (T key);         // ���ص��±������
    bool operator== (const Segment &other);    // ����ж������
    unsigned int get_mod_count();  // ��ȡ�汾�� 

    // 4. destructor
    ~Segment();                                // ��ϣ������������������
    
	std::mutex mut;               // ��֤ǿһ���ԵĻ�����
    private:
    // 5. domains
    HashEntry<T, R> **data;       // ��ϣ������ṹ
    unsigned int capacity;        // ��ϣ��Ͱ��������������ĳ��ȣ����������У���ȡ��Ԫ�صĹ�ϣֵ�󽫹�ϣֵģͰ�������Ϳ��Եõ�����Ĵ�ŵ�Ͱ���±�λ��
    double load_factor;           // ��ϣ��������
    unsigned int threshold;       // ��ϣ��������ֵ����ֵ=Ͱ����*�������ӣ�ͨ������������ϣ������ݲ����ǣ���ִ��insert��������Ԫ�غ󣬼���ϣ���е�Ԫ�ظ���size�Ƿ������ֵthreshold�������ڣ������resize�����������ݣ���������ݷ�����μ�resize����
    unsigned int size;            // ��ǰ��ϣ���е�Ԫ�ظ���
	unsigned int mod_count;       // �汾�ţ���Segment�ڲ����޸ģ����緢����ֵ�Բ���ɾ����ʱ����1  
    	
    // 6. private functions
    void resize();       // ��ϣ�����ݷ���
    unsigned int hash(unsigned int input) const;             // ����JenkinsԤ�����ļ��Ĺ�ϣֵ�������ڵ�Ͱ���±� 
    unsigned int jenkins_prehash(unsigned int input) const;     // Jenkins��ϣֵԤ��������hashCode()����ֵ���뾭���ò�Ԥ���� 
};

/**
 * Segment: Ĭ�Ϲ��캯��������յĹ�ϣ�� 
 */
template<typename T, typename R>
Segment<T, R>::Segment()
{
	size=0;    									// ��ʼ��Ԫ�ظ���Ϊ0
	capacity=16;        						// Ĭ��Ͱ��������ʼֵΪ16��16Ϊ2���ݣ�����ÿ�����ݾ�����2����Ͱ������Ϊ2���������� 
	load_factor=0.75;   						// Ĭ�ϵĸ������ӳ�ʼֵΪ0.75
	threshold=capacity*load_factor;        		// ��ϣ��������ֵ����ֵʼ�յ���Ͱ�������Ը�������
	mod_count=0;                                // ��ʼ���汾��Ϊ0 
	data=new HashEntry<T, R>* [capacity];     	// ��ʼ��Ͱ 
	
	for(int i=0;i<capacity;i++)
	{
		data[i]=nullptr;     // ��ʼ��ÿ��Ͱ�е�����Ϊ�� 
	}
}

/**
 * Segment: �����鹹���ϣ���� 
 * param data: ���ڹ����ԭʼ�������� 
 */
template<typename T, typename R>
Segment<T, R>::Segment(const std::vector<T> &_keys, const std::vector<R> &_values)
{
	size=0;    									// ��ʼ��Ԫ�ظ���Ϊ0
	capacity=16;        						// Ĭ��Ͱ��������ʼֵΪ16��16Ϊ2���ݣ�����ÿ�����ݾ�����2����Ͱ������Ϊ2���������� 
	load_factor=0.75;   						// Ĭ�ϵĸ������ӳ�ʼֵΪ0.75
	threshold=capacity*load_factor;        		// ��ϣ��������ֵ����ֵʼ�յ���Ͱ�������Ը�������
	mod_count=0;                                // ��ʼ���汾��Ϊ0 
	data=new HashEntry<T, R>* [capacity];     	// ��ʼ��Ͱ
	
	for(int i=0;i<capacity;i++)
	{
		data[i]=nullptr;     // ��ʼ��ÿ��Ͱ�е�����Ϊ�� 
	}
	
	for(int i=0;i<_keys.size();i++)
	{
		this->insert(_keys[i], _values[i]);
	}
}

/**
 * Segment: �������캯�� 
 */
template<typename T, typename R>
Segment<T, R>::Segment(const Segment &other)
{
	std::lock_guard<std::mutex> other_guard(other.mut);
	size=other.size;  
	mod_count=other.mod_count;  									
	capacity=other.capacity;        						
	load_factor=other.load_factor;   						
	threshold=capacity*load_factor;
	data=new HashEntry<T, R>* [capacity];     		
	
	for(int i=0;i<capacity;i++)
	{
		if(other.data[i])
		{
			data[i]=new HashEntry<T, R> (other.data[i]->key, other.data[i]->val);
			HashEntry<T, R> *now=data[i], *othnow=other.data[i]->next; 
			while(othnow)
			{
				now->next=new HashEntry<T, R> (othnow->key, othnow->val);
				now=now->next;
				othnow=othnow->next; 
			}
		}
		else
		data[i]=nullptr;
	}
}

/**
 * operator=: ������ֵ����� 
 */
template<typename T, typename R>
Segment<T, R> &Segment<T, R>::operator= (const Segment &other)
{
	if(this==&other)
	return (*this);
	
	std::lock_guard<std::mutex> guard(mut);
	std::lock_guard<std::mutex> other_guard(other.mut);
	for(int i=0;i<capacity;i++)
	{
		HashEntry<T, R> *now=data[i];
		while(now)
		{
			HashEntry<T, R> *temp=now;
			now=now->next;
			delete temp;
		}
	}
	delete data;
	
	size=other.size;    									
	capacity=other.capacity;        						
	load_factor=other.load_factor;   						
	threshold=capacity*load_factor;
	mod_count=other.mod_count; 
	data=new HashEntry<T, R>* [capacity];     		
	
	for(int i=0;i<capacity;i++)
	{
		if(other.data[i])
		{
			data[i]=new HashEntry<T, R> (other.data[i]->key, other.data[i]->val);
			HashEntry<T, R> *now=data[i], *othnow=other.data[i]->next;
			while(othnow)
			{
				now->next=new HashEntry<T, R> (othnow->key, othnow->val);
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
template<typename T, typename R>
Segment<T, R>::Segment(Segment<T, R> &&other) 
{
	std::lock_guard<std::mutex> other_guard(other.mut);
	// 1. �ӿ���Դ����ǳ������Ա������ֵ����ǰ���� 
	size=other.size;           // ��ϣ��Ԫ�ظ��� 
	capacity=other.capacity;   // ��ϣ��Ͱ����
	load_factor=other.load_factor;   // װ������
	threshold=capacity*load_factor;  // ������ֵ 
	data=other.data;                 // �ײ�洢�ṹָ��
	mod_count=other.mod_count; 
	
	// 2. ������Դ����ĳ�Ա������ֵ�趨ΪĬ��ֵ����Ĭ�Ϲ��캯���е�ȡֵ��
	other.size=0;    								
	other.capacity=16;        					
	other.load_factor=0.75;   					
	other.threshold=capacity*load_factor;  
	other.mod_count=0;      		
	other.data=new HashEntry<T, R>* [capacity];     			
	
	for(int i=0;i<capacity;i++)
	{
		other.data[i]=nullptr;
	}
}

/**
 * operator=: �ƶ���ֵ�����������cpp11���ƶ������ṩ��ȫ��ǳ����/�ƶ��ֶ�  
 */ 
template<typename T, typename R>
Segment<T, R> &Segment<T, R>::operator= (Segment &&other)
{
	// 1. �ж��Ƿ�Ϊ����ֵ
	if(this==&other)
	return (*this);
	
	std::lock_guard<std::mutex> other_guard(other.mut);
	std::lock_guard<std::mutex> guard(mut);
	// 2. �ͷ����������ʹ�õĶѿռ���Դ
	for(int i=0;i<capacity;i++)
	{
		HashEntry<T, R> *now=data[i];
		while(now)
		{
			HashEntry<T, R> *temp=now;
			now=now->next;
			delete temp;
		}
	}
	
	// 3. �ӿ���Դ����ǳ������Ա������ֵ����ǰ���� 
	size=other.size;           // ��ϣ��Ԫ�ظ��� 
	capacity=other.capacity;   // ��ϣ��Ͱ����
	load_factor=other.load_factor;   // װ������
	threshold=capacity*load_factor;  // ������ֵ 
	mod_count=other.mod_count;
	data=other.data;                 // �ײ�洢�ṹָ��
	
	// 4. ������Դ����ĳ�Ա������ֵ�趨ΪĬ��ֵ����Ĭ�Ϲ��캯���е�ȡֵ��
	other.size=0;    								
	other.capacity=16;         	 				
	other.load_factor=0.75;   					
	other.threshold=capacity*load_factor;        
	other.mod_count=0;      		
	other.data=new HashEntry<T, R>* [capacity];     			
	for(int i=0;i<capacity;i++)
	{
		other.data[i]=nullptr;
	}
	
	// 5. ���ص�ǰ���������
	return (*this);
}

/**
 * insert: ���ϣ���в����ֵ�� 
 * note 1: �������Ԫ��ֵ�ڹ�ϣ�������Ѿ������ظ����򲻻���в��룬ֻ�Ǹ��¸ü���Ӧ��ֵ 
 * note 2: ��ΪConcurrentStrongHashMap�д���������������������������insert�������뱣֤���Ե��̰߳�ȫ���ƻ��ṹ�������޸ı��������Ӱ�������账�����������������Ѿ����� 
 */
template<typename T, typename R>
void Segment<T, R>::insert(T key, R value) 
{
	std::lock_guard<std::mutex> guard(mut);
	// 1. ����Ӧ�������Ͱλ��
	unsigned int index=hash(key.hashCode()); 
	// 2. ������λ�õ�Ͱ�е�������������ͬ�ļ�������¸ü���Ӧ��ֵ��������ͬ�ļ�����������β�������µļ�ֵ�ԣ������Ӽ�ֵ�Լ�������Ҫע���ر���ͰΪ�յ���� 
	HashEntry<T, R> *now=data[index], *nowprev=nullptr;
	while(now) 
	{
		if(now->key.hashCode()==key.hashCode()&&now->key.equals(key))
		{				
			now->val=value;   // ��ϣ�����Ѿ����ڸü�������¸ü���Ӧ��ֵ���������Ӽ�ֵ�Լ���������
			mod_count+=1; 
			return;
		}
		else
		{
			nowprev=now; 
			now=now->next;
		}
	}
	if(data[index])
	nowprev->next=new HashEntry<T, R> (key, value);
	else
	data[index]=new HashEntry<T, R> (key, value);
	// 3. ����Ƿ���Ҫ�������� 
	size+=1; 
	mod_count+=1; 
	if(size>threshold)
	resize();
	return; 
}

/**
 * remove: �ӹ�ϣ������ɾ��ָ��ֵ��Ԫ��
 * note: ����ϣ�����в����ڸ�ֵ����ʵ���ϲ������ɾ�� 
 */
template<typename T, typename R>
void Segment<T, R>::remove(T key) 
{
	std::lock_guard<std::mutex> guard(mut);
	// 1. ����Ӧ�������Ͱλ��
	unsigned int index=hash(key.hashCode()); 
	// 2. ��ͰΪ�գ���ɾ��ʧ�ܣ��������β���Ͱ�е������ҵ�����ȵĽ�㼴����ɾ��
	if(data[index]) 
	{
		HashEntry<T, R> *now=data[index], *nowprev=nullptr;
		while(now)
		{
			if(now->key.hashCode()==key.hashCode()&&now->key.equals(key))
			{
				HashEntry<T, R> *temp=now;
				if(nowprev)
				nowprev->next=now->next;
				else
				data[index]=now->next;  
				
				delete temp;
				size-=1;
				mod_count+=1;
				return; 
			}
			else
			{
				nowprev=now;
				now=now->next;
			}
		}	
	}
	else
	return;
}

/**
 * contain: �жϸ���ֵ�Ƿ��ڹ�ϣ����
 * note: ������ֵ�ڹ�ϣ�����򷵻�true�����򷵻�false 
 */
template<typename T, typename R>
bool Segment<T, R>::contain(T key) 
{
	std::lock_guard<std::mutex> guard(mut);
	if(!size)
	return false;
	for(int i=0;i<capacity;i++)
	{
		HashEntry<T, R> *now=data[i];
		while(now)
		{
			if(now->key.hashCode()==key.hashCode()&&now->key.equals(key))
			return true;
			else
			now=now->next;
		}
	}
	return false;
}

/**
 * get: ��ȡ��ϣ���и�������Ӧ��ֵ
 * param key: �����ҵļ�
 * output-param value: �����ҵ��˶�Ӧ�ļ�����ͨ���ò������ظü���Ӧ��ֵ
 * return: �����ҳɹ�������true��������ʧ�ܣ�����false 
 */
template<typename T, typename R>
bool Segment<T, R>::get(T key, R &value) 
{
	std::lock_guard<std::mutex> guard(mut);
	if(!size)
	return false;
	for(int i=0;i<capacity;i++)
	{
		HashEntry<T, R> *now=data[i];
		while(now)
		{
			if(now->key.hashCode()==key.hashCode()&&now->key.equals(key))
			{
				value=now->val;
				return true;
			}
			else
			now=now->next;
		}
	}
	return false;
}

/**
 * keys: ��ȡ��ϣ�����е����м����б�
 * return: ��ϣ�����е�����Ԫ�ص��б�
 * note: ��ȡ��ϣ���ϵ����м����б���һ��Эͬ��������Segment�в��������������ƽ�������ConcurrentStrongHashMap��keys�������� 
 */
template<typename T, typename R>
std::vector<T> Segment<T, R>::keys() 
{
	std::vector<T> res;
	if(!size)
	return res;
	for(int i=0;i<capacity;i++)
	{
		HashEntry<T, R> *now=data[i];
		while(now)
		{
			res.push_back(now->key);
			now=now->next;
		}
	}
	return res;
}

/**
 * length: ��ȡ��ϣ���Ԫ�ظ��� 
 * note: ͳ��Ԫ�ظ�����һ��Эͬ������ͳ�Ƹ���ʱ��������ͳ�Ƹ���ʱ��������ConcurrentStrongHashMap��length���������� 
 */
template<typename T, typename R>
int Segment<T, R>::length() 
{
	return size;
}

/**
 * toString: ���ɹ�ϣ�������ַ��� 
 * note: �����ϣ����һ��Эͬ��������Segment�в��������������ƽ�������ConcurrentStrongHashMap��toString�������� 
 */
template<typename T, typename R>
std::string Segment<T, R>::toString() 
{
    std::string res;
    for(int i=0;i<capacity;i++)
    {
    	if(i==0)
    	res+="*";
    	else
    	res+="\n*";
    	
    	const HashEntry<T, R> *now=data[i];
    	while(now)
    	{
    		res=res+"->"+"("+now->key.toString()+", "+now->val.toString()+")";
    		now=now->next;
		}
		res=res+"->NULL";
	}
	return res;
}

template<typename T, typename R>
R &Segment<T, R>::operator[] (T key)
{
	std::lock_guard<std::mutex> guard(mut);
	for(int i=0;i<capacity;i++)
	{
		HashEntry<T, R> *now=data[i];
		while(now)
		{
			if(now->key.hashCode()==key.hashCode()&&now->key.equals(key))
			{
				return now->val;
			}
			else
			now=now->next;
		}
	}
}

/** 
 * operator==: �ж�������ϣ�����Ƿ����
 * note: �ж�������������ϣ���Ƿ���ȵ��㷨���ںܴ���Ż��ռ䣬��Ҫ������ȵ��Ż������������ơ�ͬʱ��Ҫ���ѵ��ǣ���Ϊ����ɾ���Ȳ�����˳����ͬ��������ͨ��ֱ�ӱȽ�������ϣ��Ľṹ���Ƚ�������ϣ���Ƿ����
 * note: �������ʹ�õıȽ��㷨���£�
 *       1. �����жϴ����otherָ���ַ�Ƿ��thisָ����ȣ�����ȣ���ֱ�ӷ���true��������벽��2
 *		 2. Ȼ���жϴ���Ĺ�ϣ�����е�Ԫ�������Ƿ�͵�ǰ��ϣ������ȣ�������ȣ���ֱ�ӷ���true��������벽��3
 *       3. ��������жϵ�ǰ��ϣ�����е�ÿһ����ֵ���Ƿ��ڹ�ϣ����other�У��������ڣ���ֱ�ӷ���true�����򷵻�false		 
 */ 
template<typename T, typename R>
bool Segment<T, R>::operator== (const Segment &other)
{
	std::lock_guard<std::mutex> other_guard(other.mut);
	std::lock_guard<std::mutex> guard(mut);
	if(&other==this)
	return true;
	else if(other.size!=size)
	return false;
	else
	{
		for(int i=0;i<capacity;i++)
		{
			HashEntry<T, R> *now=data[i];
			while(now)
			{
				R othval;
				if(other.get(now->key, othval)&&othval.equals(now->val))
				now=now->next;
				else
				return false;
			}
		}
		return true;
	}
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
 * note 3: ��Ϊ 
 */
template<typename T, typename R>
void Segment<T, R>::resize() 
{
	// 1. ��Ͱ����capacity�ӱ�
	unsigned int old_capacity=capacity; 
	capacity*=2;
	// 2. ���¼�����ֵthreshoud
	threshold=capacity*load_factor;
	// 3. �����µ�Ͱ����
	HashEntry<T, R> **new_data=new HashEntry<T, R>* [capacity]; 
	for(int i=0;i<capacity;i++)
	{
		new_data[i]=nullptr; 
	}
	// 4. ��ԭͰ�����е�ÿ����ֵ������ʹ����ԭ����ͬ�Ĺ�ϣ�㷨�ƶ��������ǿ��������µ�Ͱ������ 
	for(int i=0;i<old_capacity;i++)
	{
		while(data[i])
		{
			unsigned int index=hash(data[i]->key.hashCode());        // �����µĹ�ϣ��ַ��λ�� 
			HashEntry<T, R> *temp=data[i]->next;
			if(new_data[index])
			{
				data[i]->next=new_data[index];
				new_data[index]=data[i];
			}
			else
			{
				data[i]->next=nullptr;      // ע���ƶ��Ľ����Ҫ��nextָ���ÿ� 
				new_data[index]=data[i]; 
			} 
			data[i]=temp;
		}
		data[i]=nullptr;       // ԭͰ�����е�ͷ�������ƽ���ÿ� 
	}
	delete [] data;
	data=new_data;
}

/** 
 * hash: ���ݶ����hashCode���صĹ�ϣֵ�������Ͱ�е�λ�� 
 * note: �÷����õ���λ���±�Ӧ�������ܵؾ��ȷ�ɢ�����jdk��ʹ�õ�ʵ�ַ�ʽ�ǣ�
 *       1. ��ϣֵ�Ŷ�����hashCode����16λ�Ľ����ԭ����hashCode���������õ����fianlhash�����͹�ϣֵ�ĳ�ͻ�� 
 *       2. ȡģ���㣺��finalhash%capacity������capacity��2���������ݣ���˸��������ʹ��Ч�ʽϸߵ�finalhash&(capacity-1)��λ����ȡ�� 
 */ 
template<typename T, typename R>
unsigned int Segment<T, R>::hash(unsigned int code) const
{
	return jenkins_prehash(code)&(capacity-1);
}

/**
 * jenkins_prehash: ��ϣԤ����
 * note: ���ι�ϣ֮ǰ����Ҫ���й�ϣԤ����Ԥ�����Ŀ���ǳ�ֻ��ӣ��Ӷ��Ż���ϣ��Ч�� 
 */
template<typename T, typename R>
unsigned int Segment<T, R>::jenkins_prehash(unsigned int h) const
{
	h += (h << 15) ^ 0xffffcd7d; 
	h ^= (h >> 10);
	h += (h << 3); h ^= (h >> 6);
	h += (h << 2) + (h << 14); 
	return h ^ (h >> 16);
}

/**
 * get_mod_count: ��ȡ�汾�� 
 * note: �÷������� 
 */
template<typename T, typename R>
unsigned int Segment<T, R>::get_mod_count()
{
	return mod_count;
}

/**
 * ~Segment: ���������� 
 */
template<typename T, typename R>
Segment<T, R>::~Segment()
{
	for(int i=0;i<capacity;i++)
	{
		HashEntry<T, R> *now=data[i];
		while(now)
		{
			HashEntry<T, R> *temp=now;
			now=now->next;
			delete temp;
		}
	}
	delete [] data;
}

#endif
