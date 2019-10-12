#include <iostream> 
#include <thread>
#include <mutex>

#include "wrmutex.h"                      

using namespace std;

wrmutex mut;

void write_func(int id)
{
	mut.write_lock();     // 写者锁加锁 
	string str="写者线程"+to_string(id)+"\n";
	cout<<str;
	mut.write_unlock();   // 写者锁解锁 
}
 
void read_func(int id)
{  
	mut.read_lock();      // 读者锁加锁 
	string str="读者线程"+to_string(id)+"\n";
	cout<<str;
	mut.read_unlock();    // 读者锁解锁 
}

int main(void)
{
	thread th1(write_func, 1);
	thread th2(write_func, 2);
	thread th3(read_func, 3);
	thread th4(read_func, 4);
	thread th5(write_func, 5);
	thread th6(read_func, 6);
	thread th7(read_func, 7);
	thread th8(write_func, 8);
	thread th9(read_func, 9);
	thread th10(read_func, 10);
	thread th11(read_func, 11);
	thread th12(write_func, 12);
	thread th13(read_func, 13);
	thread th14(write_func, 14);
	thread th15(read_func, 15); 
	
	th1.join();
	th2.join();
	th3.join();
	th4.join();
	th5.join();
	th6.join();
	th7.join();
	th8.join();
	th9.join();
	th10.join();
	th11.join();
	th12.join();
	th13.join();
	th14.join();
	th15.join();
}
