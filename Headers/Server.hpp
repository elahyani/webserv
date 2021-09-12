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
# include <fcntl.h>
# include <sys/stat.h>
# include <algorithm>
# include <list>
# include "Request.hpp"
class Request;

class Server
{
private:
	int					_masterSockFD;
	short				_port;
	struct sockaddr_in 	_myAddr;
	struct sockaddr_in 	_clientAddr;
	socklen_t			_addrLen;
	int					_newSockFD;
	fd_set				_masterFDs;
	fd_set				_readFDs;
	fd_set				_writeFDs;
	int					_maxSockFD;
	char*				_fileName;

public:
	Server();
    Server(short port, char *fileName);
    Server(Server const & ths);
    ~Server();
    Server & operator=(const Server & ths);
	
	void createSocket();
	void bindSocket();
	void listenToClient();

	void newConnectHandling(int &);
	void existConnectHandling(int &);


	void exampleOfResponse(char *);
	void createSockets();
};

#endif
