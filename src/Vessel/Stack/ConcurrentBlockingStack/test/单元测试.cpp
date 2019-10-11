#include "ConcurrentBlockingStack.h"        
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <windows.h>

using namespace std;

mutex mut;

void producer_func(ConcurrentBlockingStack<int> &que, int id)
{
	srand(id);
	int sep=(rand()%5)*1000;
	while(1)
	{
		Sleep(sep);
		mut.lock();
		que.push(id);
		string str="线程"+to_string(id)+"入栈"+to_string(id)+"\n";
		cout<<str;
		mut.unlock();
		sep=(rand()%5)*1000;  
	}
}

void consumer_func(ConcurrentBlockingStack<int> &que, int id)
{
	srand(id);
	int sep=(rand()%5)*1000;
	while(1)
	{
		Sleep(sep); 
		mut.lock();
		int val;  
		que.pop(val);
		string str="线程"+to_string(id)+"出栈"+to_string(val)+"\n";
		cout<<str;
		mut.unlock();
		sep=(rand()%5)*1000;
	}
}

int main(void)
{
	ConcurrentBlockingStack<int> que(5);
	
	thread producer1(producer_func, ref(que), 1);
	thread producer2(producer_func, ref(que), 2);
	thread producer3(producer_func, ref(que), 3);
	
	thread consumer4(consumer_func, ref(que), 4);
	thread consumer5(consumer_func, ref(que), 5);
	thread consumer6(consumer_func, ref(que), 6);
	
	producer1.join();
	producer2.join();
	producer3.join();
	
	consumer4.join();
	consumer5.join();
	consumer6.join();
}
