#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <cstring>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <algorithm>
#include <vector>
#include <map>
#include "ConfigFileParser.hpp"
#include "HttpServer.hpp"
#include "Cgi.hpp"
#include "Request.hpp"
class Request;
#include "Response.hpp"

#define BUFFER_SIZE 1024
#define BACKLOG 2048

class Server
{
private:
	ConfigFileParser	_parser;
	std::vector<HttpServer> _servers;
	int _masterSockFD;
	std::vector<int> _masterSockFDs;
	std::vector<short> _ports;
	short _port;
	std::string _host;
	struct sockaddr_in _serverAddr;
	struct sockaddr_in _clientAddr;
	socklen_t _addrLen;
	fd_set _masterFDs;
	fd_set _readFDs;
	fd_set _writeFDs;
	int _maxSockFD;
	std::map<int, std::string> _clients;
	std::map<int, int> _accptMaster;
	Request _request;
<<<<<<< HEAD
<<<<<<< HEAD
	// std::string _dirPath;
	// size_t _locPos;
=======
	char *_fileName;
>>>>>>> 0902391 (feat(CGI): Handling php script)
=======
>>>>>>> 10cf81e (refactor(webserv): Accept the new changin)

public:
	Server();
	Server(ConfigFileParser &);
	Server(Server const &);
	~Server();
	Server &operator=(const Server &);

	void createSocket();
	void bindSocket();
	void listenToClient();

	void newConnectHandling(int &);
	void existConnectHandling(int &);

	void responseHandling(int &);
	void createMasterSockets();

	void findTheTargetServer(int &, HttpServer *, short *);
};

#endif
