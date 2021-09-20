#include "Cgi.hpp"

Cgi::Cgi()
{
}

Cgi::Cgi(struct CGIInfos & cgiInfos)
{
	setenv("REQUEST_METHOD", cgiInfos.request.getStartLineVal("method").c_str(), 0);
	std::cout << "REQUEST_METHOD=" << getenv("REQUEST_METHOD") << std::endl;
	setenv("SERVER_PROTOCOL", cgiInfos.request.getStartLineVal("protocol").c_str(), 1);
	std::cout << "SERVER_PROTOCOL=" << getenv("SERVER_PROTOCOL") << std::endl;
	setenv("CONTENT_TYPE", cgiInfos.request.getHeaderVal("Content-Type").c_str(), 1);
	std::cout << "CONTENT_TYPE=" << getenv("CONTENT_TYPE") << std::endl;
	setenv("CONTENT_LENGTH", cgiInfos.request.getHeaderVal("Content-Length").c_str(), 1);
	std::cout << "CONTENT_LENGTH=" << getenv("CONTENT_LENGTH") << std::endl;
	setenv("PATH_INFO", cgiInfos.request.getStartLineVal("url").c_str(), 1);
	std::cout << "PATH_INFO=" << getenv("PATH_INFO") << std::endl;
	setenv("QUERY_STRING", cgiInfos.request.getStartLineVal("query").c_str(), 1);
	std::cout << "QUERY_STRING=" << getenv("QUERY_STRING") << std::endl;
	setenv("SERVER_SOFTWARE", "webserv", 1);
	std::cout << "SERVER_SOFTWARE=" << getenv("SERVER_SOFTWARE") << std::endl;
	setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);
	std::cout << "GATEWAY_INTERFACE=" << getenv("GATEWAY_INTERFACE") << std::endl;
	setenv("DOCUMENT_ROOT", cgiInfos.server.getRoot().c_str(), 1);
	std::cout << "DOCUMENT_ROOT=" << getenv("DOCUMENT_ROOT") << std::endl;
	setenv("SERVER_NAME", cgiInfos.server.getHost().c_str(), 1);
	std::cout << "SERVER_NAME=" << getenv("SERVER_NAME") << std::endl;
	setenv("SERVER_PORT", std::to_string(cgiInfos.port).c_str(), 1);
	std::cout << "SERVER_PORT=" << getenv("SERVER_PORT") << std::endl;
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

