#include <iostream>

int add(int a, int b)
{
	return a+b;
}

int sub(int a, int b)
{
	return a - b;
}

int main()
{
	
	int x = 5;
	int y = 2;
	int z = 0;
	int w = 0;
	z = add(x,y);
	w= sub(x,y);

	std::cout << "Address of main(): 0x" << std::hex << &main << "\n";
	std::cout << "Address of add(): 0x" << std::hex << &add << "\n";
	std::cout << "Address of sub(): 0x" << std::hex << &sub << "\n";
	std::cout << "Address of x: 0x" << std::hex << &x << "\n";
	std::cout << "Address of y: 0x" << std::hex << &y << "\n";
	std::cout << "Address of z: 0x" << std::hex << &z << "\n";
	std::cout << "Address of w: 0x" << std::hex << &w << "\n";

return 0;
}

