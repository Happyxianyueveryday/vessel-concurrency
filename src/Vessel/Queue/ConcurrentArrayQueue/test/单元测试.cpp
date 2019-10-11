#include "ConcurrentArrayQueue.h"            
#include <iostream>   
#include <thread>

using namespace std;    
 
mutex mut;    // 测试线程读写锁 
 
void func(ConcurrentArrayQueue<int> *que, int id)   
{
	srand(id*12+5);  
	for(int i=0;i<10;i++)
	{
		int judge=rand()%4;
		
		// 1. 入栈操作 
		// note: 单纯的调用push方法并不需要加锁，但是这里因为需要保证操作完成后立即输出，因此加了锁 
		if(judge==0)
		{
			mut.lock();
			if(que->push(id))
			{
				string str="线程"+to_string(id)+"入队"+to_string(id)+"\n";
				cout<<str;
			}
			else
			{
				string str="队列已满，线程"+to_string(id)+"入队失败"+"\n";  
				cout<<str;
			}
			mut.unlock();
		}
		
		// 2. 计算栈大小 
		// note: 获取栈大小后应立即使用，过一段时间后所获取的栈大小可能失效，因此需要将调用length方法的代码和使用栈大小的代码写成一个原子操作 
		if(judge==1)
		{
			mut.lock();
			string str="线程"+to_string(id)+"获取的队列大小为"+to_string(que->length())+"\n";
			cout<<str;
			mut.unlock();
		}
		
		// 3. 出栈操作
		// note: 单纯的调用pop方法并不需要加锁，但是这里因为需要保证操作完成后立即输出，因此加了锁 
		if(judge==2)
		{
			mut.lock();
			int num; 
			if(que->pop(num))
			{
				string str="线程"+to_string(id)+"出队值"+to_string(num)+"\n";
				cout<<str;
			}
			else
			{
				string str="队列已空，线程"+to_string(id)+"出队失败"+"\n";
				cout<<str;
			}
			mut.unlock();
		}
		
		// 4. 获取栈顶元素 
		// note: 获取栈顶元素后应立即使用，过一段时间后所获取的栈顶元素可能失效，因此需要将调用top方法的代码和使用栈顶元素的代码写成一个原子操作 
		if(judge==3)
		{
			mut.lock();
			int val;
			if(que->frontval(val))
			{
				string str="线程"+to_string(id)+"获取的队首元素为"+to_string(val)+"\n";
				cout<<str;
			}
			else
			{
				string str="队列已空，线程"+to_string(id)+"获取队首元素失败"+"\n";
				cout<<str;
			} 
			mut.unlock(); 
		}
	}
} 

int main(void)
{
	//ConcurrentArrayQueue<int> *que1=ConcurrentArrayQueue<int>::ArrayQueueFactory(3, "read-first");
	ConcurrentArrayQueue<int> *que1=ConcurrentArrayQueue<int>::ArrayQueueFactory(3, "write-first");  
	//ConcurrentArrayQueue<int> *que1=ConcurrentArrayQueue<int>::ArrayQueueFactory(3, "fair");
	 
	thread thr1(func, ref(que1), 1);
	thread thr2(func, ref(que1), 2);
	thread thr3(func, ref(que1), 3);
	thread thr4(func, ref(que1), 4);
	thread thr5(func, ref(que1), 5);
	thread thr6(func, ref(que1), 6);
	thread thr7(func, ref(que1), 7);
	thread thr8(func, ref(que1), 8);
	thread thr9(func, ref(que1), 9);
	thread thr10(func, ref(que1), 10);
	thread thr11(func, ref(que1), 11);
	thread thr12(func, ref(que1), 12);
	
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
	
	while(que1->length())
	{
		int val;
		que1->pop(val); 
		cout<<val<<" ";
	}
	cout<<endl;
}
