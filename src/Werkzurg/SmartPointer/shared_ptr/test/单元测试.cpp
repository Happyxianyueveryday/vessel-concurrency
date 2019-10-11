#include "shared_ptr.h"
#include "TestElement.h" 

using namespace std;
 
int main(void)
{
	shared_ptr<TestElement> pos1, pos2, pos3;
	pos1->print(); 
	(*pos1).print();
	
	pos3=pos2;   // 析构obj3 
	pos2=pos1;    
	{
		shared_ptr<TestElement> pos4, pos5;
		pos2=pos4;     
		pos1=pos5;   // 析构obj1 
	}
	
	shared_ptr<TestElement> pos6=shared_ptr<TestElement>(); 
	pos6=move(pos1);   // 析构obj6，同时移动后pos1指向新建对象obj7
	pos6=move(pos2);   // 析构obj5，同时移动后pos1指向新建对象obj8 
	
	// 析构obj2, obj4, obj7, obj8，析构顺序不定 
}
