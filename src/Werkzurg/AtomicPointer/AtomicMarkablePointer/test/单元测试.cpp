#include "AtomicMarkablePointer.h"  
#include <iostream> 
#include <atomic>
#include <thread>

using namespace std;


void th1_func(AtomicMarkablePointer<int> &ptr1, AtomicMarkablePointer<int> &ptr2, int *data1, int *data2)
{
	for(int i=0;i<100;i++)
	{
		ptr1.compare_exchange_strong(data1, data2, i, i+1);
	}
}

void th2_func(AtomicMarkablePointer<int> &ptr1, AtomicMarkablePointer<int> &ptr2, int *data1, int *data2)
{
	for(int i=0;i<100;i++) 
	{ 
		ptr1.compare_exchange_weak(data2, data1, i, i+1);
	}
}

int main(void)
{	
	int *data1=new int(5);
	int *data2=new int(6);
	
	AtomicMarkablePointer<int> ptr1(data1);
	AtomicMarkablePointer<int> ptr2(data2);
		
	thread th1(th1_func, ref(ptr1), ref(ptr2), data1, data2);
	thread th2(th2_func, ref(ptr1), ref(ptr2), data1, data2);
	
	th1.join();
	th2.join();
	
	cout<<ptr1.get_flag()<<endl;
	cout<<ptr2.get_flag()<<endl;
}
