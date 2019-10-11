#include "ConcurrentLinkedStack.h"              
#include <iostream>   
#include <thread>
#include <mutex> 

using namespace std;       
 
mutex mut;    // �����̻߳����� 
 
void func(ConcurrentArrayStack<int> &sta, int id)   
{
	srand(id*12+5); 
	for(int i=0;i<10;i++)
	{
		int judge=rand()%4;
		// ����ֵΪ1����ջ+��ջ+��ջ���� 
		// note: �����ĵ���push����������Ҫ����������������Ϊ��Ҫ��֤������ɺ������������˼����� 
		if(judge==0)
		{
			// 1.1
			mut.lock();
			if(sta.push(id))
			{
				string str1="�߳�"+to_string(id)+"��ջֵ"+to_string(id)+"\n";
				cout<<str1; 
			}
			else
			{
				string str1="�ڴ治�㣬�߳�"+to_string(id)+"��ջʧ��\n";
				cout<<str1;
			}
			mut.unlock();
			
			// 1.2
			mut.lock();
			int num; 
			if(sta.pop(num))    // ��һ�γ�ջ 
			{
				string str2="�߳�"+to_string(id)+"��ջֵ"+to_string(num)+"\n";
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
			sta.push(id);
			string str3="�߳�"+to_string(id)+"��ջֵ"+to_string(id)+"\n";
			cout<<str3;
			mut.unlock();
		}
		
		// ����ֵΪ2������ջ��С 
		// note: ��ȡջ��С��Ӧ����ʹ�ã���һ��ʱ�������ȡ��ջ��С����ʧЧ�������Ҫ������length�����Ĵ����ʹ��ջ��С�Ĵ���д��һ��ԭ�Ӳ��� 
		else if(judge==1)
		{
			mut.lock();
			string str="�߳�"+to_string(id)+"��ȡ��ջ��СΪ"+to_string(sta.length())+"\n";
			cout<<str;
			mut.unlock();
		}
		
		// 3. ��ջ+��ջ+��ջ���� 
		// note: �����ĵ���pop����������Ҫ����������������Ϊ��Ҫ��֤������ɺ������������˼����� 
		else if(judge==2)
		{
			// 3.1  
			mut.lock();
			int num; 
			if(sta.pop(num))    // ��һ�γ�ջ 
			{
				string str1="�߳�"+to_string(id)+"��ջֵ"+to_string(num)+"\n";
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
				string str2="�ڴ治�㣬�߳�"+to_string(id)+"��ջʧ��\n";
				cout<<str2;
			}
			mut.unlock();
			
			// 3.3  
			mut.lock();
			if(sta.pop(num))    // �ڶ��γ�ջ 
			{
				string str2="�߳�"+to_string(id)+"��ջֵ"+to_string(num)+"\n";
				cout<<str2; 
			}
			else
			{
				string str2="ջ�ѿգ��߳�"+to_string(id)+"��ջʧ��"+"\n";
				cout<<str2;
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
				string str="�߳�"+to_string(id)+"��ȡ��ջ��Ԫ��Ϊ"+to_string(val)+"\n";
				cout<<str;
			}
			else
			{
				string str="ջ�ѿգ��߳�"+to_string(id)+"��ȡջ��Ԫ��ʧ��"+"\n";
				cout<<str;
			}
			mut.unlock();
		}
	}
} 

int main(void)
{
	ConcurrentArrayStack<int> sta1;
	
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
