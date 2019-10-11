#include "ConcurrentLinkedStack.h"               
#include <iostream>          
#include <thread>
#include <mutex>

using namespace std;       
 
mutex mut;    // �����̻߳����� 
 
void func(ConcurrentLinkedStack<int> &sta, int id)   
{
	srand(id*12+5); 
	for(int i=0;i<10;i++)
	{
		int judge=rand()%4;
		// ����ֵΪ0����ջ+��ջ+��ջ���� 
		// note: �����ĵ���push����������Ҫ����������������Ϊ��Ҫ��֤������ɺ������������˼����� 
		if(judge==0)
		{
			mut.lock();
			if(sta.push(id))
			{
				string str1="�߳�"+to_string(id)+"��ջֵ"+to_string(id)+"\n";
				cout<<str1;
			}
			else
			{
				string str1="ջ�������߳�"+to_string(id)+"��ջʧ��"+"\n";
				cout<<str1;
			}
			mut.unlock();
			
			// 1.2
			mut.lock();
			int num2; 
			if(sta.pop(num2))  
			{
				string str2="�߳�"+to_string(id)+"��ջֵ"+to_string(num2)+"\n";
				cout<<str2;
			}
			else
			{
				string str2="ջ�ѿգ��߳�"+to_string(id)+"��ջʧ��"+"\n";
				cout<<str2;
			}
			mut.unlock();
			
			// 1.3
			mut.lock();
			if(sta.push(id))
			{
				string str3="�߳�"+to_string(id)+"��ջֵ"+to_string(id)+"\n";
				cout<<str3;
			}
			else
			{
				string str3="ջ�������߳�"+to_string(id)+"��ջʧ��"+"\n";
				cout<<str3;
			}
			mut.unlock();
		}
		
		// ����ֵΪ1������ջ��С 
		// note: ��ȡջ��С��Ӧ����ʹ�ã���һ��ʱ�������ȡ��ջ��С����ʧЧ�������Ҫ������length�����Ĵ����ʹ��ջ��С�Ĵ���д��һ��ԭ�Ӳ��� 
		else if(judge==1)
		{
			mut.lock();
			string str="�߳�"+to_string(id)+"��ȡ��ջ��СΪ"+to_string(sta.length())+"\n";
			cout<<str;
			mut.unlock();
		}
		
		// ����ֵΪ2����ջ+��ջ+��ջ���� 
		// note: �����ĵ���pop����������Ҫ����������������Ϊ��Ҫ��֤������ɺ������������˼����� 
		else if(judge==2)
		{
			// 3.1  
			mut.lock();
			int num1; 
			if(sta.pop(num1))    // ��һ�γ�ջ 
			{
				string str1="�߳�"+to_string(id)+"��ջֵ"+to_string(num1)+"\n";
				cout<<str1;
			}
			else
			{
				string str1="ջ�ѿգ��߳�"+to_string(id)+"��ջʧ��"+"\n";
				cout<<str1;
			}
			mut.unlock();
			
			// 3.2
			mut.lock();
			if(sta.push(id))
			{
				string str2="�߳�"+to_string(id)+"��ջֵ"+to_string(id)+"\n";
				cout<<str2;
			}
			else
			{
				string str2="ջ�������߳�"+to_string(id)+"��ջʧ��"+"\n";
				cout<<str2;
			}
			mut.unlock();
			
			// 3.3  
			mut.lock();
			int num3;
			if(sta.pop(num3))    // �ڶ��γ�ջ 
			{
				string str3="�߳�"+to_string(id)+"��ջֵ"+to_string(num3)+"\n";
				cout<<str3;
			}
			else
			{
				string str3="ջ�ѿգ��߳�"+to_string(id)+"��ջʧ��"+"\n";
				cout<<str3;
			}
			mut.unlock();
		}
		
		// 4. ��ȡջ��Ԫ�� 
		// note: ��ȡջ��Ԫ�غ�Ӧ����ʹ�ã���һ��ʱ�������ȡ��ջ��Ԫ�ؿ���ʧЧ�������Ҫ������top�����Ĵ����ʹ��ջ��Ԫ�صĴ���д��һ��ԭ�Ӳ��� 
		else
		{
			mut.lock();
			int val;
			if(sta.top(val))
			{
				string str1="�߳�"+to_string(id)+"��ȡ��ջ��Ԫ��Ϊ"+to_string(val)+"\n";
				cout<<str1;
			}
			else
			{
				string str1="ջ�ѿգ��߳�"+to_string(id)+"��ȡջ��Ԫ��ʧ��"+"\n";
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
