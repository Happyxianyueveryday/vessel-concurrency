#include "shared_ptr.h"
#include "TestElement.h" 

using namespace std;
 
int main(void)
{
	shared_ptr<TestElement> pos1, pos2, pos3;
	pos1->print(); 
	(*pos1).print();
	
	pos3=pos2;   // ����obj3 
	pos2=pos1;    
	{
		shared_ptr<TestElement> pos4, pos5;
		pos2=pos4;     
		pos1=pos5;   // ����obj1 
	}
	
	shared_ptr<TestElement> pos6=shared_ptr<TestElement>(); 
	pos6=move(pos1);   // ����obj6��ͬʱ�ƶ���pos1ָ���½�����obj7
	pos6=move(pos2);   // ����obj5��ͬʱ�ƶ���pos1ָ���½�����obj8 
	
	// ����obj2, obj4, obj7, obj8������˳�򲻶� 
}
