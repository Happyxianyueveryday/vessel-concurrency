#include "TicketLocalSpinLock.h"                   
#include <iostream>        
#include <string>
#include <thread>
#include <mutex>

using namespace std;

TicketLocalSpinLock lk;

//mutex mut;

void th1_func() 
{
	//lock_guard<mutex> guard(mut);
	lk.lock();
	cout<<"thread1"<<" output"<<endl;
	cout<<"thread1"<<" output"<<endl;
	cout<<"thread1"<<" output"<<endl; 
	lk.unlock();
}

void th2_func() 
{
	//lock_guard<mutex> guard(mut);
	lk.lock();
	cout<<"thread2"<<" output"<<endl;
	cout<<"thread2"<<" output"<<endl;
	cout<<"thread2"<<" output"<<endl; 
	lk.unlock();
}

void th3_func() 
{
	//lock_guard<mutex> guard(mut);
	lk.lock();
	cout<<"thread3"<<" output"<<endl;
	cout<<"thread3"<<" output"<<endl;
	cout<<"thread3"<<" output"<<endl;
	lk.unlock();
}

int main(void)
{
	thread th1(th1_func);
	thread th2(th2_func);
	thread th3(th3_func);
	
	th1.join();
	th2.join();
	th3.join();
}
