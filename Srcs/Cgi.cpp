#include "Cgi.hpp"

Cgi::Cgi()
{
}

Cgi::Cgi(Cgi const &ths)
{
	*this = ths;
	return ;
}

Cgi::~Cgi()
{
}

Cgi & Cgi::operator=(Cgi const &ths)
{
	if(this != &ths)
	{

	}
	return *this;
} 