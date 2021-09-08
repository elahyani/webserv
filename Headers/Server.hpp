#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <cstring>
# include <string>
# include <sys/socket.h>
# include <sys/types.h>
# include <sys/select.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <iostream>
# include <fstream>
# include <unistd.h>
# include <sys/stat.h>
# include <algorithm>
# include <list>
# include "Request.hpp"
class Request;

class Server
{
private:
    int                 _masterSockFd;
	short				_port;
    struct sockaddr_in  _myAddr;
    struct sockaddr_in  _clientAddr;
    socklen_t           _addrLen;
    int                 _newSockFd;
    fd_set              _readFds;
    // fd_set              _masterFds;
    // fd_set              _writeFds;
    // fd_set              _expFds;
    int                 _maxSockFd;
    int                 _sockFd;
	std::vector<int>	_accptSockFds;

public:
    Server(short port, char *fileName);
    Server(Server const & ths);
    ~Server();
    Server & operator=(const Server & ths);
	
	void createSocket();
	void bindSocket();
	void listenToClient();
	void exampleOfResponse(char *);
};

#endif