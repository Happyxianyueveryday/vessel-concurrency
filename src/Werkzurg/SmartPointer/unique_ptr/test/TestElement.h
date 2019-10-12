#ifndef TEST_ELEMENT_H       
#define TEST_ELEMENT_H 
#include <iostream>
#include <string>  

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
	std::cout<<"Hello, World!"<<std::endl;
}

TestElement::~TestElement() 
{
	std::cout<<"����"+std::to_string(val)+"����������������."<<std::endl;
}                                 

#endif
