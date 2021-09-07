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
# include "Request.hpp"
# include "Response.hpp"

# define MAX_CLIENTS 1024

class Server
{
private:
    int                 _masterSockFd;
	short				_port;
    struct sockaddr_in  _myAddr;
    socklen_t           _addrLen;
    int                 _newSockFd;
    fd_set              _readFds;
    int                 _sockClients[1024];
    int                 _maxSockFd;
    int                 _sockFd;

public:
    Server(short port, char *fileName);
    Server(Server const & ths);
    ~Server();
    Server & operator=(const Server & ths);
	
	void createBindSocket();
};

#endif