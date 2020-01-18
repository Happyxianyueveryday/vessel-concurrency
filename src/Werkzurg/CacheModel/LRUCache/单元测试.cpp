#include "ZipHashMap.h"
#include "LRUCache.h" 
#include "Hashable.h"  
#include <iostream>

using namespace std;

ZipHashMap<Hashable, Hashable> exstore;    // 测试用外置存储 

/** 
 * init_exstore: 初始化测试用外置存储
 * note: 测试用的外置存储设置为存储0-99之间（包括0和99）的奇数 
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
	
	// 1. 顺序读取测试 
	for(int i=0;i<100;i++)
	{
		Hashable key(i), value;
		if(cache.read(key, value))
		cout<<"key = "<<key.value()<<" "<<"value = "<<value.value()<<endl;
		else
		cout<<"缓存读取失败，缓存中不存在该键."<<endl;
	}
	vector<pair<Hashable, Hashable>> res;
	res=cache.all();
	cout<<"顺序读取测试结束，缓存中现在剩下的所有键值对如下:"<<endl;
	for(int i=0;i<res.size();i++)
	{
		cout<<"key = "<<res[i].first.value()<<" "<<"value = "<<res[i].second.value()<<endl;
	}
	
	cout<<"---------------------------------------------------"<<endl;
	
	// 3. 移动操作测试
	LRUCache<Hashable, Hashable> cache2(move(cache));
	LRUCache<Hashable, Hashable> cache3(1);
	cache3=move(cache2);
	
	// 4. 随机读取测试 
	for(int i=0;i<100;i++)
	{
		Hashable key(rand()%100), value;
		if(cache3.read(key, value))
		cout<<"key = "<<key.value()<<" "<<"value = "<<value.value()<<endl;
		else
		cout<<"缓存读取失败，缓存中不存在键 key = "<<key.value()<<endl;
	}
	res=cache3.all();
	cout<<"随机读取测试结束，缓存中现在剩下的所有键值对如下:"<<endl;
	for(int i=0;i<res.size();i++)
	{
		cout<<"key = "<<res[i].first.value()<<" "<<"value = "<<res[i].second.value()<<endl;
	}
	
	cout<<"---------------------------------------------------"<<endl;
	
	// 5. 顺序写入测试
	{
		LRUCache<Hashable, Hashable> cache4(cache3);
		for(int i=0;i<100;i+=2)
		{
			cache4.write(Hashable(i), Hashable(i));
		}
	}    // 若使用的是写回法，则当cache4析构后，缓存中剩下的键值对才会被同步到外置存储 
	vector<Hashable> keys=exstore.keys();
	cout<<"顺序写入测试结束，"<<"键值对数量为nums = "<<keys.size()<<"，外置存储更新后的所有键值对如下:"<<endl;
	for(int i=0;i<keys.size();i++)
	{
		Hashable value;
		exstore.get(keys[i], value);
		cout<<"key = "<<keys[i].value()<<" "<<"value = "<<value.value()<<endl;
	}
	
	cout<<"---------------------------------------------------"<<endl;
	
	// 6. 随机写入测试
	{
		LRUCache<Hashable, Hashable> cache5(cache3);
		for(int i=0;i<100;i++)
		{
			cache5.write(Hashable(i), Hashable(i+1));
		}
	}    // 若使用的是写回法，则当cache5析构后，缓存中剩下的键值对才会被同步到外置存储 
	keys=exstore.keys();
	cout<<"随机写入测试结束，"<<"键值对数量为nums = "<<keys.size()<<"，外置存储更新后的所有键值对如下:"<<endl;
	for(int i=0;i<keys.size();i++)
	{
		Hashable value;
		exstore.get(keys[i], value);
		cout<<"key = "<<keys[i].value()<<" "<<"value = "<<value.value()<<endl;
	}
}
