#include "Cgi.hpp"

Cgi::Cgi() : _port(0), _root(""), _cgiPath(""), _cgiResult(""), _contentLength(0)
{
}

Cgi::Cgi(Request &request, Location &location, HttpServer &server, short &port) : _request(request), _server(server), _port(port), _root(""), _cgiPath(""), _cgiResult(""), _contentLength(0)
{
	_root = (location.getRoot().size()) ? location.getRoot().c_str() : _server.getRoot().c_str();
	_cgiPath = location.getFastCgiPass();
	if (access(_cgiPath.c_str(), F_OK)  == -1)
		throw std::runtime_error(": No such file or directory {" + _cgiPath +'}');
	if (access(_cgiPath.c_str(), X_OK) == -1)
		throw std::runtime_error(": " + _cgiPath + ": Permission denied.");
	_contentLength = std::atoi(_request.getHeaderVal("Content-Length").c_str());
	this->setEnvCgi();
	this->cgiExec();
}

Cgi::Cgi(Cgi const &ths)
{
	*this = ths;
	return;
}

Cgi::~Cgi()
{
}

Cgi &Cgi::operator=(Cgi const &ths)
{
	if (this != &ths)
	{
		this->_request = ths._request;
		this->_server = ths._server;
		this->_port = ths._port;
		this->_root = ths._root;
		this->_cgiResult = ths._cgiResult;
		this->_contentLength = ths._contentLength;
	}
	return *this;
}

void Cgi::setEnvCgi()
{
	setenv("REQUEST_METHOD", _request.getStartLineVal("method").c_str(), 1);
	setenv("SERVER_PROTOCOL", _request.getStartLineVal("protocol").c_str(), 1);
	setenv("CONTENT_TYPE", _request.getHeaderVal("Content-Type").c_str(), 1);
	setenv("CONTENT_LENGTH", _request.getHeaderVal("Content-Length").c_str(), 1);
	setenv("PATH_INFO", _request.getHeaderVal("url").c_str(), 1);
	setenv("QUERY_STRING", _request.getStartLineVal("query").c_str(), 1);
	setenv("SERVER_SOFTWARE", "webserv", 1);
	setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);
	setenv("DOCUMENT_ROOT", _root.c_str(), 1);
	setenv("SERVER_NAME", _server.getHost().c_str(), 1);
	setenv("SERVER_PORT", std::to_string(_port).c_str(), 1);
	setenv("SCRIPT_NAME", _request.getStartLineVal("script-name").c_str(), 1);
	setenv("SCRIPT_FILENAME", (_root + "/" + _request.getStartLineVal("script-name")).c_str(), 1);
	setenv("REDIRECT_STATUS", std::to_string(_request.getStatusCode()).c_str(), 1);
}

void Cgi::cgiExec()
{
	char const *args[3];
	int pipeFDs[2];
	int readBytes = 0;
	char buffer[BUFFER_SIZE + 1];
	pid_t childPID;

	args[0] = _cgiPath.c_str();
	args[1] = getenv("SCRIPT_FILENAME");
	args[2] = NULL;
	if (pipe(pipeFDs) < 0)
		throw std::runtime_error("pipe failed.");
	childPID = fork();
	if (childPID < 0)
		throw std::runtime_error("Unable to create child by fork.");
	if (childPID == 0)
	{
		close(pipeFDs[1]);
		dup2(pipeFDs[0], STDIN_FILENO);
		dup2(_fd, STDOUT_FILENO);
		chdir(_root.c_str());
		if (execve(_cgiPath.c_str(), (char *const *)args, environ) == -1)
			exit(1);
	}
	else
	{
		close(pipeFDs[0]);
		for (size_t i = 0; i < _request.getBody().size(); i++)
			if (_request.getBody()[i].body.size())
				_request.setReqBody(_request.getBody()[i].body);
		if (_contentLength)
			write(pipeFDs[1], _request.getReqBody().c_str(), _request.getReqBody().size());
		close(pipeFDs[1]);
		bzero(buffer, BUFFER_SIZE);
		while ((readBytes = read(_fd, buffer, BUFFER_SIZE)) > 0)
		{
			buffer[readBytes] = '\0';
			_cgiResult.append(buffer, readBytes);
		}
		wait(0);
	}
}

std::string &Cgi::getCgiResult()
{
	return _cgiResult;
}
