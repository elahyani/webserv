#ifndef CGI_HPP
# define CGI_HPP

# include "Request.hpp"
# include "HttpServer.hpp"
# include "Server.hpp"
# include <cstdlib>
# include <string>
# include <unistd.h>

// Environement variables change during the program
extern char** environ;

class Cgi
{
private:
	Request _request;
	HttpServer _server;
	short _port;
	std::string _root;
	std::string _cgiPath;
	std::string _cgiResult;
	int _contentLength;
	
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
