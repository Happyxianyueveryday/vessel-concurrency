#include "ConcurrentLockFreeStack.h"                     
#include <iostream>   
#include <thread>
#include <mutex> 

using namespace std;       
 
void push_func(ConcurrentLockFreeStack<int> &sta)   
{
	for(int i=0;i<600;i++)
	{
		sta.push(rand());
	}
} 

void pop_func(ConcurrentLockFreeStack<int> &sta)
{
	for(int i=0;i<500;i++)
	{
		shared_ptr<int> res=sta.pop();
	}
}

int main(void)
{
	ConcurrentLockFreeStack<int> sta1;
	
	thread thr1(push_func, ref(sta1));
	thread thr2(push_func, ref(sta1));
	thread thr3(push_func, ref(sta1));
	thread thr4(push_func, ref(sta1));
	thread thr5(push_func, ref(sta1));
	thread thr6(push_func, ref(sta1));
	
	thread thr7(pop_func, ref(sta1));
	thread thr8(pop_func, ref(sta1));
	thread thr9(pop_func, ref(sta1));
	thread thr10(pop_func, ref(sta1));
	thread thr11(pop_func, ref(sta1));
	thread thr12(pop_func, ref(sta1));
	
	thr1.join();
	thr2.join();
	thr3.join();
	thr4.join();
	thr5.join();
	thr6.join();
	
	thr7.join();
	thr8.join();
	thr9.join();
	thr10.join();
	thr11.join();
	thr12.join();
	
	sta1.print();
}
