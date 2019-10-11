#include "ArrayBlockingQueue.h"   
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <windows.h>

using namespace std;

void producer_func(ArrayBlockingQueue<int> *que, int id)
{
	srand(id);
	int sep=(rand()%5)*1000;
	for(int i=0;i<3;i++)
	{
		Sleep(sep);
		que->push(id);
		sep=(rand()%5)*1000;  
	}
}

void consumer_func(ArrayBlockingQueue<int> *que, int id)
{
	srand(id);
	int sep=(rand()%5)*1000;
	for(int i=0;i<2;i++)
	{
		Sleep(sep);
		int val;  
		que->pop(val);
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
	
	while(!que->empty())
	{
		int val;
		que->pop(val);
		cout<<val<<" ";
	}
	cout<<endl;
}
