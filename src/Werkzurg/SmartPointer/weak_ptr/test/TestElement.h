#ifndef TESTELEMENT_H 
#define TESTELEMENT_H
#include <string>  
#include <iostream>  

using namespace std; 

class TestElement  
{
	public:
	TestElement();                           // 默认构造函数 
	void print();                            // 输出测试函数 
    ~TestElement();                          // 析构函数
    
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
	cout<<"对象"+to_string(val)+"的析构函数被调用."<<endl;
}                                 

#endif
