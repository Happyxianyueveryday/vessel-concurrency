#include "AtomicStampPointer.h"     
#include <iostream> 
#include <atomic>
#include <thread>
#include <mutex>

using namespace std;

mutex mut;

void th1_func(AtomicStampPointer<int> &ptr1, AtomicStampPointer<int> &ptr2, int *data1, int *data2)
{
	lock_guard<mutex> guard(mut);
	int *expected=data1;
	cout<<ptr1.compare_exchange_strong(expected, data2, ptr1.get_stamp(), ptr1.get_stamp()+1)<<endl;
	cout<<*expected<<endl;
}

void th2_func(AtomicStampPointer<int> &ptr1, AtomicStampPointer<int> &ptr2, int *data1, int *data2)
{
	lock_guard<mutex> guard(mut);
	int *expected=data1;
	cout<<ptr1.compare_exchange_strong(expected, data1, ptr1.get_stamp(), ptr1.get_stamp()+1)<<endl;
	cout<<*expected<<endl;
}

int main(void)
{	
	int *data1=new int(5);
	int *data2=new int(6);
	
	AtomicStampPointer<int> ptr1(data1);
	AtomicStampPointer<int> ptr2(data2);
		
	thread th1(th1_func, ref(ptr1), ref(ptr2), data1, data2);
	thread th2(th2_func, ref(ptr1), ref(ptr2), data1, data2);
	
	th1.join();
	th2.join();
}
