#ifndef CGI_HPP
# define CGI_HPP

# include "Request.hpp"
# include "HttpServer.hpp"
# include "Server.hpp"
# include <cstdlib>
# include <string>
# include <unistd.h>

extern char** environ;

class Cgi
{
private:
	Request _request;
	Location _location;
	HttpServer _server;
	short _port;
	std::string _cgiResult;
public:
	Cgi();
	Cgi(Request &, Location &, HttpServer &, short &);
	Cgi(Cgi const &);
	~Cgi();
	Cgi & operator=(Cgi const &);

	void setEnvCgi();
	void cgiExec();
	std::string & getCgiResult();
};

#endif

// REQUEST_METHOD = get method from request √
// SERVER_PROTOCOL = HTTP/1.1 √
// CONTENT_TYPE = get from request √
// CONTENT_LENGTH = get from request √
// PATH_INFO = get from request "url" √ /Users/asaadi/.Work/webserv
// QUERY_STRING =  ?...... √
// SERVER_SOFTWARE = webserv √
// GATEWAY_INTERFACE = CGI/1.1 √
// DOCUMENT_ROOT = getRoot from server √
// SERVER_ADDR = IP address or hostName √
// SCRIPT_NAME = *.php 
// SCRIPT_FILENAME = $DOCUMENT_ROOT$SCRIPT_NAME 
// SCRIPT_NAME = scriptName from server location 
// SERVER_NAME = get from the server 
// SERVER_PORT = get from server ports
// REMOTE_ADD = 

// REMOTE_ADDR
// PATH_TRANSLATED
// REDIRECT_STATUS

// /usr/local/bin/php-cgi