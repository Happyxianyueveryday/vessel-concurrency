#include "Semaphore.h"                   
#include <iostream>        
#include <string> 
#include <thread>
#include <mutex>

using namespace std;

Semaphore lk;

//mutex mut;

void th1_func() 
{
	//lock_guard<mutex> guard(mut);
	for(int i=0;i<100;i++)
	{
		lk.notify();
	} 
}

void th2_func() 
{
	//lock_guard<mutex> guard(mut);
	for(int i=0;i<100;i++)
	{
		lk.notify();
	}
}

void th3_func() 
{
	//lock_guard<mutex> guard(mut);
	for(int i=0;i<100;i++)
	{
		lk.wait();
	}
}


int main(void)
{
	thread producer_1(th1_func);
	thread producer_2(th2_func);
	thread consumer_1(th3_func);
	
	producer_1.join();
	producer_2.join();
	consumer_1.join();
}
