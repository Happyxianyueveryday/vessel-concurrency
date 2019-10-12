#ifndef TEST_ELEMENT_H       
#define TEST_ELEMENT_H 
#include <iostream>
#include <string>  

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
	std::cout<<"Hello, World!"<<std::endl;
}

TestElement::~TestElement() 
{
	std::cout<<"对象"+std::to_string(val)+"的析构函数被调用."<<std::endl;
}                                 

#endif
