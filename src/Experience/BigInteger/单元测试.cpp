#include "BigInteger.h" 
#include <iostream> 

using namespace std;

int main(void)
{
	BigInteger a(-5900);
	BigInteger b(5421000);
	BigInteger c(-1);
	
	BigInteger d("5421000");
	BigInteger e("-5900");
	BigInteger f("-1");
	
	BigInteger g("7");
	BigInteger h("-2");
	BigInteger i("-1");
	
	cout<<a+b+c<<endl;
	cout<<a-b-c<<endl;
	cout<<a*b*c<<endl;
	cout<<a+b-(-c)<<endl;
	cout<<a*(b+c)<<endl;
	
	cout<<d+e+f<<endl;
	cout<<d-e-f<<endl;
	cout<<d*e*f<<endl;
	
	cout<<g+h+i<<endl;
	cout<<g-h-i<<endl;
	cout<<g*h*i<<endl;
	cout<<g+h-(-i)<<endl;
	cout<<g*(h+i)<<endl;
}
