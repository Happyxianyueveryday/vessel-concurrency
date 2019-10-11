#include "ConcurrentWeakHashSet.h"                          
#include "Hashable.h"             
#include <iostream>      
#include <cstdlib> 
#include <thread>
using namespace std; 

void insert_func(ConcurrentWeakHashSet<Hashable> &hashset, int id)
{
	srand(id);
	
	for(int i=0;i<10000;i++)
	{
		hashset.insert(rand()%3000);
	}
}

void remove_func(ConcurrentWeakHashSet<Hashable> &hashset, int id)
{
	srand(id);
	for(int i=0;i<10000;i++)
	{
		hashset.remove(rand()%3000);
	}
}


void contain_func(ConcurrentWeakHashSet<Hashable> &hashset, int id)
{
	srand(id);      
	for(int i=0;i<10000;i++)
	{
		hashset.contain(rand()%3000);
	}
}

void length_func(ConcurrentWeakHashSet<Hashable> &hashset, int id)
{
	srand(id);
	for(int i=0;i<10000;i++)
	{
		hashset.length();
	}
}

void print_func(ConcurrentWeakHashSet<Hashable> &hashset, int id)
{
	srand(id);
	for(int i=0;i<5;i++)
	{
		string res=hashset.toString()+"\n ---------------------------------------------------------------------------------- \n";
		cout<<res;
	}
}
 
int main(void)
{
	ConcurrentWeakHashSet<Hashable> hashset;
	
	thread th1(insert_func, ref(hashset), 1);
	thread th2(insert_func, ref(hashset), 2);
	thread th3(insert_func, ref(hashset), 3);
	thread th4(insert_func, ref(hashset), 4);
	thread th5(insert_func, ref(hashset), 5); 
	thread th6(insert_func, ref(hashset), 6);
	
	thread th7(remove_func, ref(hashset), 7); 
	thread th8(remove_func, ref(hashset), 8);
	thread th9(remove_func, ref(hashset), 9);
	
	thread th10(contain_func, ref(hashset), 11);
	thread th11(contain_func, ref(hashset), 12); 
	thread th12(contain_func, ref(hashset), 13);
	
	thread th13(length_func, ref(hashset), 13);
	thread th14(length_func, ref(hashset), 14);
	
	thread th15(print_func, ref(hashset), 15);
	
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
	
	cout<<hashset.toString()<<endl;
}
