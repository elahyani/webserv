#include "Cgi.hpp"

Cgi::Cgi()
{
}

Cgi::Cgi(Request & request)
{
	// Filling the metavariables structure
	std::memset(&_envCgi, '\0', sizeof(_envCgi));
	_envCgi.requestMethod = request.getStartLineVal("method");
	_envCgi.serverProtocol = request.getStartLineVal("protocol");
	_envCgi.contentType = request.getHeaderVal("Content-Type");
	_envCgi.contentLength = request.getHeaderVal("Content-Length");
	_envCgi.pathInfo = request.getStartLineVal("url");
	_envCgi.queryString = request.getStartLineVal("query");
	// _envCgi.scriptFileName =
	this->printCgiEnv();
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

void Cgi::printCgiEnv()
{
	std::cout << "REREQUEST_METHOD=" << '\"'<< _envCgi.requestMethod << '\"' << std::endl;
	std::cout << "SERVER_PROTOCOL=" << '\"'<< _envCgi.serverProtocol << '\"' << std::endl;
	std::cout << "CONTENT_TYPE=" << '\"'<< _envCgi.contentType << '\"' << std::endl;
	std::cout << "CONTENT_LENGTH=" << '\"'<< _envCgi.contentLength << '\"' << std::endl;
	std::cout << "PATH_INFO=" << '\"'<< _envCgi.pathInfo << '\"' << std::endl;
	std::cout << "QUERY_STRING=" << '\"'<< _envCgi.queryString << '\"' << std::endl;
	std::cout << "SCRIPT_NAME=" << '\"'<< _envCgi.scriptName << '\"' << std::endl;
	std::cout << "SERVER_NAME=" << '\"'<< _envCgi.serverName << '\"' << std::endl;
}
