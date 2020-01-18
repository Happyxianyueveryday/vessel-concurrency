#include "ZipHashMap.h"
#include "LRUCache.h" 
#include "Hashable.h"  
#include <iostream>

using namespace std;

ZipHashMap<Hashable, Hashable> exstore;    // ���������ô洢 

/** 
 * init_exstore: ��ʼ�����������ô洢
 * note: �����õ����ô洢����Ϊ�洢0-99֮�䣨����0��99�������� 
 */
void init_exstore()   
{
	for(int i=1;i<100;i+=2)
	{
		exstore.insert(Hashable(i), Hashable(i));
	}
}

bool noncache_read_callback(const Hashable &key, Hashable &value)
{
	return exstore.get(key, value);
}

void noncache_write_callback(const Hashable &key, const Hashable &value) 
{
	exstore.insert(key, value);
}

int main(void)
{
	LRUCache<Hashable, Hashable> cache(5, LRUCache<Hashable, Hashable>::WRITE_BACK, noncache_read_callback, noncache_write_callback);
	init_exstore();
	
	// 1. ˳���ȡ���� 
	for(int i=0;i<100;i++)
	{
		Hashable key(i), value;
		if(cache.read(key, value))
		cout<<"key = "<<key.value()<<" "<<"value = "<<value.value()<<endl;
		else
		cout<<"�����ȡʧ�ܣ������в����ڸü�."<<endl;
	}
	vector<pair<Hashable, Hashable>> res;
	res=cache.all();
	cout<<"˳���ȡ���Խ���������������ʣ�µ����м�ֵ������:"<<endl;
	for(int i=0;i<res.size();i++)
	{
		cout<<"key = "<<res[i].first.value()<<" "<<"value = "<<res[i].second.value()<<endl;
	}
	
	cout<<"---------------------------------------------------"<<endl;
	
	// 3. �ƶ���������
	LRUCache<Hashable, Hashable> cache2(move(cache));
	LRUCache<Hashable, Hashable> cache3(1);
	cache3=move(cache2);
	
	// 4. �����ȡ���� 
	for(int i=0;i<100;i++)
	{
		Hashable key(rand()%100), value;
		if(cache3.read(key, value))
		cout<<"key = "<<key.value()<<" "<<"value = "<<value.value()<<endl;
		else
		cout<<"�����ȡʧ�ܣ������в����ڼ� key = "<<key.value()<<endl;
	}
	res=cache3.all();
	cout<<"�����ȡ���Խ���������������ʣ�µ����м�ֵ������:"<<endl;
	for(int i=0;i<res.size();i++)
	{
		cout<<"key = "<<res[i].first.value()<<" "<<"value = "<<res[i].second.value()<<endl;
	}
	
	cout<<"---------------------------------------------------"<<endl;
	
	// 5. ˳��д�����
	{
		LRUCache<Hashable, Hashable> cache4(cache3);
		for(int i=0;i<100;i+=2)
		{
			cache4.write(Hashable(i), Hashable(i));
		}
	}    // ��ʹ�õ���д�ط�����cache4�����󣬻�����ʣ�µļ�ֵ�ԲŻᱻͬ�������ô洢 
	vector<Hashable> keys=exstore.keys();
	cout<<"˳��д����Խ�����"<<"��ֵ������Ϊnums = "<<keys.size()<<"�����ô洢���º�����м�ֵ������:"<<endl;
	for(int i=0;i<keys.size();i++)
	{
		Hashable value;
		exstore.get(keys[i], value);
		cout<<"key = "<<keys[i].value()<<" "<<"value = "<<value.value()<<endl;
	}
	
	cout<<"---------------------------------------------------"<<endl;
	
	// 6. ���д�����
	{
		LRUCache<Hashable, Hashable> cache5(cache3);
		for(int i=0;i<100;i++)
		{
			cache5.write(Hashable(i), Hashable(i+1));
		}
	}    // ��ʹ�õ���д�ط�����cache5�����󣬻�����ʣ�µļ�ֵ�ԲŻᱻͬ�������ô洢 
	keys=exstore.keys();
	cout<<"���д����Խ�����"<<"��ֵ������Ϊnums = "<<keys.size()<<"�����ô洢���º�����м�ֵ������:"<<endl;
	for(int i=0;i<keys.size();i++)
	{
		Hashable value;
		exstore.get(keys[i], value);
		cout<<"key = "<<keys[i].value()<<" "<<"value = "<<value.value()<<endl;
	}
}
