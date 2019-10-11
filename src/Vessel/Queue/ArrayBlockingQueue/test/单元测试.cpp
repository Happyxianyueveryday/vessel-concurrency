#include "ArrayBlockingQueue.h"     
#include <iostream>
#include <string>
#include <thread> 
#include <mutex>
#include <windows.h>

using namespace std;

mutex producer_mut;
mutex consumer_mut;

void producer_func(ArrayBlockingQueue<int> *que, int id)
{
	srand(id);
	int sep=(rand()%5)*1000;
	while(1)
	{
		Sleep(sep);
		producer_mut.lock();
		que->push(id);
		string str="线程"+to_string(id)+"入队"+to_string(id)+"\n";
		cout<<str;
		producer_mut.unlock();
		sep=(rand()%5)*1000;  
	}
}

void consumer_func(ArrayBlockingQueue<int> *que, int id)
{
	srand(id);
	int sep=(rand()%5)*1000;
	while(1)
	{
		Sleep(sep);
		consumer_mut.lock();
		int val;  
		que->pop(val);
		string str="线程"+to_string(id)+"出队"+to_string(val)+"\n";
		cout<<str;
		consumer_mut.unlock();
		sep=(rand()%5)*1000;
	}
}

int main(void)
{
	ArrayBlockingQueue<int> *que=ArrayBlockingQueue<int>::ArrayQueueFactory(5);
	
	thread producer1(producer_func, que, 1);
	thread producer2(producer_func, que, 2);
	thread producer3(producer_func, que, 3);
	
	thread consumer4(consumer_func, que, 4);
	thread consumer5(consumer_func, que, 5);
	thread consumer6(consumer_func, que, 6);
	
	producer1.join();
	producer2.join();
	producer3.join();
	
	consumer4.join();
	consumer5.join();
	consumer6.join();
}
