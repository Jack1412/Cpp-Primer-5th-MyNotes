#include <iostream>
using namespace std;

class Base
{
private:
	int base;
public:
	Base() {}
	virtual ~Base() {}
	// virtual void func() {}
};
class Dervied : public Base
{
private:
	int dervied;
    // int xx;
public:
	Dervied() {}
	~Dervied() {}
};

int main()
{
    // cout << sizeof(Base::~Base()) << endl;
	cout << sizeof(Base) << endl;
	cout << sizeof(Dervied) << endl;
	return 0;
}