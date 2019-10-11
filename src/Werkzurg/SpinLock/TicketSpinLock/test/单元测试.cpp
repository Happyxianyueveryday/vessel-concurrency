#include "TicketSpinLock.h"       
#include <iostream>        
#include <string>
#include <thread>
#include <mutex>

using namespace std;

TicketSpinLock lk;

//mutex mut;

void th1_func() 
{
	//lock_guard<mutex> guard(mut);
	const unsigned int ticket=lk.lock();
	cout<<"thread1"<<" output"<<endl;
	cout<<"thread1"<<" output"<<endl;
	cout<<"thread1"<<" output"<<endl; 
	lk.unlock(ticket);
}

void th2_func() 
{
	//lock_guard<mutex> guard(mut);
	const unsigned int ticket=lk.lock();
	cout<<"thread2"<<" output"<<endl;
	cout<<"thread2"<<" output"<<endl;
	cout<<"thread2"<<" output"<<endl;
	lk.unlock(ticket);
}

void th3_func() 
{
	//lock_guard<mutex> guard(mut);
	const unsigned int ticket=lk.lock(); 
	cout<<"thread3"<<" output"<<endl;
	cout<<"thread3"<<" output"<<endl;
	cout<<"thread3"<<" output"<<endl;
	lk.unlock(ticket);
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
