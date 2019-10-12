#include "shared_ptr.h" 
#include "weak_ptr.h"
#include "TestElement.h"

using namespace std; 
 
int main(void)
{
	shared_ptr<TestElement> pos1, pos2, pos3;    // 创建obj1, obj2, obj3的shared_ptr 
	weak_ptr<TestElement> wk1(pos1), wk2(pos2), wk3(pos3), wk3_temp(pos3);   // 创建weak_ptr
	pos3=pos2;   // 析构obj3
	cout<<wk3.expired()<<" "<<wk3_temp.expired()<<endl;   // 指向obj3的weak_ptr全部失效 
	shared_ptr<TestElement> temp=wk2.lock();  // temp指向obj2  
	temp->print();
	wk3=pos3;   // 重新将weak_ptr wk3指向obj2
	cout<<wk3.use_count()<<endl;
	cout<<wk3.expired()<<endl;    // wk3重新生效 
	{
		shared_ptr<TestElement> pos4;
		wk1=pos4;   // 重新将wk1指向obj4 
		cout<<wk1.expired()<<endl;   // wk1有效 
	}
	// 析构obj4 
	cout<<wk1.expired()<<endl;    // obj4超出作用域被析构，wk1失效
	cout<<wk3.use_count()<<endl;
	// 现在temp, pos3, pos2共3个shared_ptr指向obj2 
	pos3=move(pos1);  // pos3指向obj1   
	pos2=move(pos1);  // pos2指向nullptr 
	temp=pos1;        // 析构obj2 
	cout<<wk3.expired()<<endl;    // 指向obj2的wk3失效
	cout<<weak_ptr<TestElement>(pos2).expired()<<endl;    // 从pos2新建的weak_ptr失效，因为pos2指向nullptr 
	// 析构obj1 
}
