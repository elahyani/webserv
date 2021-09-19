#include "Cgi.hpp"

Cgi::Cgi()
{
}

Cgi::Cgi(Request & request)
{
	// Filling the metavariables structure
	bzero(&_envVarsCgi, sizeof(_envVarsCgi));
	_envVarsCgi.requestMethod = request.getStartLineVal("method");
	_envVarsCgi.serverProtocol = request.getStartLineVal("protocol");
	_envVarsCgi.contentType = request.getHeaderVal("Content-Type");
	_envVarsCgi.contentLength = request.getHeaderVal("Content-Length");
	_envVarsCgi.pathInfo = request.getStartLineVal("url");
	_envVarsCgi.queryString = request.getStartLineVal("query");
	// _envVarsCgi.scriptFileName =

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
	const std::string ENV[ 24 ] = {
   "COMSPEC", "DOCUMENT_ROOT", "GATEWAY_INTERFACE",   
   "HTTP_ACCEPT", "HTTP_ACCEPT_ENCODING",             
   "HTTP_ACCEPT_LANGUAGE", "HTTP_CONNECTION",         
   "HTTP_HOST", "HTTP_USER_AGENT", "PATH",            
   "QUERY_STRING", "REMOTE_ADDR", "REMOTE_PORT",      
   "REQUEST_METHOD", "REQUEST_URI", "SCRIPT_FILENAME",
   "SCRIPT_NAME", "SERVER_ADDR", "SERVER_ADMIN",      
   "SERVER_NAME","SERVER_PORT","SERVER_PROTOCOL",     
   "SERVER_SIGNATURE","SERVER_SOFTWARE" };   

	std::cout << "REREQUEST_METHOD=" << '\"'<< _envVarsCgi.requestMethod << '\"' << std::endl;
	std::cout << "SERVER_PROTOCOL=" << '\"'<< _envVarsCgi.serverProtocol << '\"' << std::endl;
	std::cout << "CONTENT_TYPE=" << '\"'<< _envVarsCgi.contentType << '\"' << std::endl;
	std::cout << "CONTENT_LENGTH=" << '\"'<< _envVarsCgi.contentLength << '\"' << std::endl;
	std::cout << "PATH_INFO=" << '\"'<< _envVarsCgi.pathInfo << '\"' << std::endl;
	std::cout << "QUERY_STRING=" << '\"'<< _envVarsCgi.queryString << '\"' << std::endl;
	std::cout << "SCRIPT_NAME=" << '\"'<< _envVarsCgi.scriptName << '\"' << std::endl;
	std::cout << "SERVER_NAME=" << '\"'<< _envVarsCgi.serverName << '\"' << std::endl;
}
