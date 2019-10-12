#ifndef TESTELEMENT_H 
#define TESTELEMENT_H
#include <string>  
#include <iostream>  

using namespace std; 

class TestElement  
{
	public:
	TestElement();                           // Ĭ�Ϲ��캯�� 
	void print();                            // ������Ժ��� 
    ~TestElement();                          // ��������
    
    private: 
    int val; 
	static int count;    
};

int TestElement::count=1; 

TestElement::TestElement()
{
	val=count;
	count+=1;
}

void TestElement::print()
{
	cout<<"Hello, World!"<<endl;
}

TestElement::~TestElement() 
{
	cout<<"����"+to_string(val)+"����������������."<<endl;
}                                 

#endif
