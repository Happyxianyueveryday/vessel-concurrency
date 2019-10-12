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
	pos6=move(pos1);   // 析构obj6
	pos6=move(pos2);   // 析构obj5
	
	pos6=pos1;         // 析构obj4 
	
	pos1=pos2;         // 测试空指针情况 
	pos1=pos6;         
	pos1->print();   
	
	// 析构obj2
}
