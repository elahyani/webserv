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
// # include "Request.hpp"
// class Request;

class Server
{
private:
    int                 _sockFd;
    struct sockaddr_in  _myAddr;
    struct sockaddr_in  _cliAddr;
    socklen_t           _addrLen;
    int                 _newSockFd;
    fd_set              _fds;

public:
    Server();
    Server(short port, char *fileName);
    Server(Server const & ths);
    ~Server();
    Server & operator=(const Server & ths);
};

#endif