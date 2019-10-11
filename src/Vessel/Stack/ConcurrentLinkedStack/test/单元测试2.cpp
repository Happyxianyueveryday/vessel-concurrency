#include "ConcurrentLinkedStack.h"               
#include <iostream>          
#include <thread>
#include <mutex>

using namespace std;       
 
mutex mut;    // 测试线程互斥锁 
 
void func(ConcurrentLinkedStack<int> &sta, int id)   
{
	srand(id*12+5); 
	for(int i=0;i<10;i++)
	{
		int judge=rand()%4;
		// 余数值为0：入栈+出栈+入栈操作 
		// note: 单纯的调用push方法并不需要加锁，但是这里因为需要保证操作完成后立即输出，因此加了锁 
		if(judge==0)
		{
			mut.lock();
			if(sta.push(id))
			{
				string str1="线程"+to_string(id)+"入栈值"+to_string(id)+"\n";
				cout<<str1;
			}
			else
			{
				string str1="栈已满，线程"+to_string(id)+"入栈失败"+"\n";
				cout<<str1;
			}
			mut.unlock();
			
			// 1.2
			mut.lock();
			int num2; 
			if(sta.pop(num2))  
			{
				string str2="线程"+to_string(id)+"出栈值"+to_string(num2)+"\n";
				cout<<str2;
			}
			else
			{
				string str2="栈已空，线程"+to_string(id)+"出栈失败"+"\n";
				cout<<str2;
			}
			mut.unlock();
			
			// 1.3
			mut.lock();
			if(sta.push(id))
			{
				string str3="线程"+to_string(id)+"入栈值"+to_string(id)+"\n";
				cout<<str3;
			}
			else
			{
				string str3="栈已满，线程"+to_string(id)+"入栈失败"+"\n";
				cout<<str3;
			}
			mut.unlock();
		}
		
		// 余数值为1：计算栈大小 
		// note: 获取栈大小后应立即使用，过一段时间后所获取的栈大小可能失效，因此需要将调用length方法的代码和使用栈大小的代码写成一个原子操作 
		else if(judge==1)
		{
			mut.lock();
			string str="线程"+to_string(id)+"获取的栈大小为"+to_string(sta.length())+"\n";
			cout<<str;
			mut.unlock();
		}
		
		// 余数值为2：出栈+入栈+出栈操作 
		// note: 单纯的调用pop方法并不需要加锁，但是这里因为需要保证操作完成后立即输出，因此加了锁 
		else if(judge==2)
		{
			// 3.1  
			mut.lock();
			int num1; 
			if(sta.pop(num1))    // 第一次出栈 
			{
				string str1="线程"+to_string(id)+"出栈值"+to_string(num1)+"\n";
				cout<<str1;
			}
			else
			{
				string str1="栈已空，线程"+to_string(id)+"出栈失败"+"\n";
				cout<<str1;
			}
			mut.unlock();
			
			// 3.2
			mut.lock();
			if(sta.push(id))
			{
				string str2="线程"+to_string(id)+"入栈值"+to_string(id)+"\n";
				cout<<str2;
			}
			else
			{
				string str2="栈已满，线程"+to_string(id)+"入栈失败"+"\n";
				cout<<str2;
			}
			mut.unlock();
			
			// 3.3  
			mut.lock();
			int num3;
			if(sta.pop(num3))    // 第二次出栈 
			{
				string str3="线程"+to_string(id)+"出栈值"+to_string(num3)+"\n";
				cout<<str3;
			}
			else
			{
				string str3="栈已空，线程"+to_string(id)+"出栈失败"+"\n";
				cout<<str3;
			}
			mut.unlock();
		}
		
		// 4. 获取栈顶元素 
		// note: 获取栈顶元素后应立即使用，过一段时间后所获取的栈顶元素可能失效，因此需要将调用top方法的代码和使用栈顶元素的代码写成一个原子操作 
		else
		{
			mut.lock();
			int val;
			if(sta.top(val))
			{
				string str1="线程"+to_string(id)+"获取的栈顶元素为"+to_string(val)+"\n";
				cout<<str1;
			}
			else
			{
				string str1="栈已空，线程"+to_string(id)+"获取栈顶元素失败"+"\n";
				cout<<str1;
			}
			mut.unlock();
		} 
	}
} 

int main(void)
{
	ConcurrentLinkedStack<int> sta1("write-first");
	
	thread thr1(func, ref(sta1), 1);
	thread thr2(func, ref(sta1), 2);
	thread thr3(func, ref(sta1), 3);
	thread thr4(func, ref(sta1), 4);
	thread thr5(func, ref(sta1), 5);
	thread thr6(func, ref(sta1), 6);
	
	thr1.join();
	thr2.join();
	thr3.join();
	thr4.join();
	thr5.join();
	thr6.join();
	
	while(sta1.length())
	{
		int val;
		sta1.pop(val); 
		cout<<val<<" ";
	}
	cout<<endl;
}
