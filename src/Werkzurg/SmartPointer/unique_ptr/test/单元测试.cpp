#include <iostream>
#include "unique_ptr.h"  
#include "TestElement.h" 
  
using namespace std;

int main(void)
{
	unique_ptr<TestElement> pos1(new TestElement()), pos2(new TestElement()), pos3(new TestElement());
	(*pos1).print();
	pos1->print();
	pos1=move(pos3);   // 释放obj1 
	cout<<pos3.empty()<<endl;
	pos2=move(pos1);   // 释放obj2 
	
	unique_ptr<TestElement> pos4(new TestElement()), pos5(new TestElement()), pos6(new TestElement());
	pos2.reset(pos4.release());   // 释放obj3 
	cout<<pos4.empty()<<endl;
	pos2.reset();                 // 释放obj4 
	cout<<pos2.empty()<<endl;
	pos5.reset();                 // 释放obj5 
	// 释放obj6 
}
