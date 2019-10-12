#include <iostream>
#include "unique_ptr.h"  
#include "TestElement.h" 
  
using namespace std;

int main(void)
{
	unique_ptr<TestElement> pos1(new TestElement()), pos2(new TestElement()), pos3(new TestElement());
	(*pos1).print();
	pos1->print();
	pos1=move(pos3);   // �ͷ�obj1 
	cout<<pos3.empty()<<endl;
	pos2=move(pos1);   // �ͷ�obj2 
	
	unique_ptr<TestElement> pos4(new TestElement()), pos5(new TestElement()), pos6(new TestElement());
	pos2.reset(pos4.release());   // �ͷ�obj3 
	cout<<pos4.empty()<<endl;
	pos2.reset();                 // �ͷ�obj4 
	cout<<pos2.empty()<<endl;
	pos5.reset();                 // �ͷ�obj5 
	// �ͷ�obj6 
}
