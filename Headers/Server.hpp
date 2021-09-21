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

class Server
{
private:
	ConfigFileParser _parser;
	std::vector<HttpServer> _servers;
	int _masterSockFD;
	std::vector<int> _masterSockFDs;
	short _port;
	std::vector<short> _ports;
	std::string _host;
	struct sockaddr_in _serverAddr;
	struct sockaddr_in _clientAddr;
	socklen_t _addrLen;
	fd_set _masterFDs;
	fd_set _readFDs;
	fd_set _writeFDs;
	int _maxSockFD;
	char *_fileName;
	std::map<int, std::string> _clients;
	std::map<int, int> _serverAddrS;
	Request _request;
	// std::string _dirPath;
	// size_t _locPos;

public:
	Server();
	Server(ConfigFileParser &, char *);
	Server(Server const &);
	~Server();
	Server &operator=(const Server &);

	void createSocket();
	void bindSocket();
	void listenToClient();

	void newConnectHandling(int &);
	void existConnectHandling(int &);

	void exampleOfResponse(char *, int &);
	void createMasterSockets();

	HttpServer findTheTargetServer(int &);

	// void setCurrentDir(std::string &, size_t);
};

#endif
