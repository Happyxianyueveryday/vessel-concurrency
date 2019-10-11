#include "ConcurrentLinkedSet.h"                          
#include "Hashable.h"
#include <string> 
#include <thread>   
#include <atomic>

using namespace std;

mutex mut; 

void get_func(ConcurrentLinkedSet<Hashable> &lis, int id)
{
	srand(id);
	for(int i=0;i<rand()%10000;i++)
	{
		int val=rand()%200;
		lis.contain(Hashable(val));
	}
} 

void print_func(ConcurrentLinkedSet<Hashable> &lis, int id)
{
	srand(id);
	for(int i=0;i<rand()%10;i++)
	{
		string str="�߳�4��ӡ: \n"+lis.toString()+"\n"; 
		cout<<str;
	}
}

void insert_func(ConcurrentLinkedSet<Hashable> &lis, int id)
{
	srand(id);
	for(int i=0;i<rand()%1000;i++)   
	{
		lis.add(Hashable(rand()%400+100)); // ���100-500֮���Ԫ��  
	}
} 

void delete_func(ConcurrentLinkedSet<Hashable> &lis, int id) 
{
	srand(id);
	for(int i=0;i<rand()%1000;i++)
	{         
		lis.remove(Hashable(rand()%100)); // ɾ��0-100֮���Ԫ�� 
		lis.remove(Hashable(rand()%100+400)); // ɾ��400-500֮���Ԫ��  
	}
}

int length_func(ConcurrentLinkedSet<Hashable> &lis, int id)
{
	srand(id);
	for(int i=0;i<rand()%10;i++)
	{
		cout<<("�߳�6��ӡ: "+to_string(lis.length())+"\n");
	}
}

int main(void)
{
	ConcurrentLinkedSet<Hashable> lis;
	
	for(int i=0;i<100;i++)
	{
		lis.add(Hashable(i));
		lis.add(Hashable(i+1));
	}
	
	// 1. �����߳� 
	thread th1(get_func, ref(lis), 1);
	thread th2(get_func, ref(lis), 2);
	thread th3(get_func, ref(lis), 3);
	
	// 2. ��ӡ�߳� 
	thread th4(print_func, ref(lis), 4); 
	thread th5(print_func, ref(lis), 5); 
	
	// 3. ���ȼ����߳� 
	thread th6(length_func, ref(lis), 5);
	
	// 4. ɾ���߳� 
	thread th7(delete_func, ref(lis), 6);
	thread th8(delete_func, ref(lis), 7);
	thread th9(delete_func, ref(lis), 8);
	
	// 5. �����߳� 
	thread th10(insert_func, ref(lis), 10);
	thread th11(insert_func, ref(lis), 11);
	thread th12(insert_func, ref(lis), 12);
	thread th13(insert_func, ref(lis), 13);
	thread th14(insert_func, ref(lis), 14);
	thread th15(insert_func, ref(lis), 15);
	thread th16(insert_func, ref(lis), 16);
	thread th17(insert_func, ref(lis), 17);
	thread th18(insert_func, ref(lis), 18);
	thread th19(insert_func, ref(lis), 19);
	
	
	th1.join();
	th2.join();
	th3.join(); 
	
	th4.join();
	th5.join();
	
	th6.join();
	
	th7.join();
	th8.join();
	th9.join(); 
	
	th10.join();
	th11.join();
	th12.join();
	th13.join();
	th14.join();
	th15.join();
	th16.join();
	th17.join();
	th18.join();
	th19.join();
	
	cout<<"���̴߳�ӡ: \n"+lis.toString()<<endl;
}
