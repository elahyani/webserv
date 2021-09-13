#include <iostream>
#include <fstream>


bool is_empty(std::ifstream& pFile)
{
    return pFile.peek() == std::ifstream::traits_type::eof();
}
int main()
{
	std::ifstream file("Makefile");
	std::cout << is_empty(file) << "\n";
	return (0);
}
