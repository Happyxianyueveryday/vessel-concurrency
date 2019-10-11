#include "ConcurrentArrayQueue.h"            
#include <iostream>   
#include <thread>

using namespace std;    
 
mutex mut;    // �����̶߳�д�� 
 
void func(ConcurrentArrayQueue<int> *que, int id)   
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
			if(que->push(id))
			{
				string str="�߳�"+to_string(id)+"���"+to_string(id)+"\n";
				cout<<str;
			}
			else
			{
				string str="�����������߳�"+to_string(id)+"���ʧ��"+"\n";  
				cout<<str;
			}
			mut.unlock();
		}
		
		// 2. ����ջ��С 
		// note: ��ȡջ��С��Ӧ����ʹ�ã���һ��ʱ�������ȡ��ջ��С����ʧЧ�������Ҫ������length�����Ĵ����ʹ��ջ��С�Ĵ���д��һ��ԭ�Ӳ��� 
		if(judge==1)
		{
			mut.lock();
			string str="�߳�"+to_string(id)+"��ȡ�Ķ��д�СΪ"+to_string(que->length())+"\n";
			cout<<str;
			mut.unlock();
		}
		
		// 3. ��ջ����
		// note: �����ĵ���pop����������Ҫ����������������Ϊ��Ҫ��֤������ɺ������������˼����� 
		if(judge==2)
		{
			mut.lock();
			int num; 
			if(que->pop(num))
			{
				string str="�߳�"+to_string(id)+"����ֵ"+to_string(num)+"\n";
				cout<<str;
			}
			else
			{
				string str="�����ѿգ��߳�"+to_string(id)+"����ʧ��"+"\n";
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
			if(que->frontval(val))
			{
				string str="�߳�"+to_string(id)+"��ȡ�Ķ���Ԫ��Ϊ"+to_string(val)+"\n";
				cout<<str;
			}
			else
			{
				string str="�����ѿգ��߳�"+to_string(id)+"��ȡ����Ԫ��ʧ��"+"\n";
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
