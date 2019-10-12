#include "shared_ptr.h" 
#include "weak_ptr.h"
#include "TestElement.h"

using namespace std; 
 
int main(void)
{
	shared_ptr<TestElement> pos1, pos2, pos3;    // ����obj1, obj2, obj3��shared_ptr 
	weak_ptr<TestElement> wk1(pos1), wk2(pos2), wk3(pos3), wk3_temp(pos3);   // ����weak_ptr
	pos3=pos2;   // ����obj3
	cout<<wk3.expired()<<" "<<wk3_temp.expired()<<endl;   // ָ��obj3��weak_ptrȫ��ʧЧ 
	shared_ptr<TestElement> temp=wk2.lock();  // tempָ��obj2  
	temp->print();
	wk3=pos3;   // ���½�weak_ptr wk3ָ��obj2
	cout<<wk3.use_count()<<endl;
	cout<<wk3.expired()<<endl;    // wk3������Ч 
	{
		shared_ptr<TestElement> pos4;
		wk1=pos4;   // ���½�wk1ָ��obj4 
		cout<<wk1.expired()<<endl;   // wk1��Ч 
	}
	// ����obj4 
	cout<<wk1.expired()<<endl;    // obj4����������������wk1ʧЧ
	cout<<wk3.use_count()<<endl;
	// ����temp, pos3, pos2��3��shared_ptrָ��obj2 
	pos3=move(pos1);  // pos3ָ��obj1   
	pos2=move(pos1);  // pos2ָ��nullptr 
	temp=pos1;        // ����obj2 
	cout<<wk3.expired()<<endl;    // ָ��obj2��wk3ʧЧ
	cout<<weak_ptr<TestElement>(pos2).expired()<<endl;    // ��pos2�½���weak_ptrʧЧ����Ϊpos2ָ��nullptr 
	// ����obj1 
}
