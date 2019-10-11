#include "ConcurrentArrayStack.h"      
       
#include <iostream>    
#include <thread>

using namespace std;    
 
mutex mut;    // �����̻߳�����  
 
void func(ConcurrentArrayStack<int> &sta, int id)   
{
	srand(id*12+5); 
	for(int i=0;i<10;i++)
	{
		int judge=rand()%4;
		
		// 1. ��ջ���� 
		// note: �����ĵ���push����������Ҫ����������������Ϊ��Ҫ��֤������ɺ������������˼����� 
		if(judge==0)
		{
			mut.lock();
			if(sta.push(id))
			{
				string str="�߳�"+to_string(id)+"��ջֵ"+to_string(id)+"\n";
				cout<<str;
			}
			else
			{
				string str="ջ�������߳�"+to_string(id)+"��ջʧ��"+"\n";
				cout<<str;
			}
			mut.unlock();
		}
		
		// 2. ����ջ��С 
		// note: ��ȡջ��С��Ӧ����ʹ�ã���һ��ʱ�������ȡ��ջ��С����ʧЧ�������Ҫ������length�����Ĵ����ʹ��ջ��С�Ĵ���д��һ��ԭ�Ӳ��� 
		if(judge==1)
		{
			mut.lock();
			string str="�߳�"+to_string(id)+"��ȡ��ջ��СΪ"+to_string(sta.length())+"\n";
			cout<<str;
			mut.unlock();
		}
		
		// 3. ��ջ����
		// note: �����ĵ���pop����������Ҫ����������������Ϊ��Ҫ��֤������ɺ������������˼����� 
		if(judge==2)
		{
			mut.lock();
			int num; 
			if(sta.pop(num))
			{
				string str="�߳�"+to_string(id)+"��ջֵ"+to_string(num)+"\n";
				cout<<str;
			}
			else
			{
				string str="ջ�ѿգ��߳�"+to_string(id)+"��ջʧ��"+"\n";
				cout<<str;
			}
			mut.unlock();
		}
		
		// 4. ��ȡջ��Ԫ�� 
		// note: ��ȡջ��Ԫ�غ�Ӧ����ʹ�ã���һ��ʱ�������ȡ��ջ��Ԫ�ؿ���ʧЧ�������Ҫ������top�����Ĵ����ʹ��ջ��Ԫ�صĴ���д��һ��ԭ�Ӳ��� 
		if(judge==3)
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
	ConcurrentArrayStack<int> sta1(3);
	
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
