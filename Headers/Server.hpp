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
# include <vector>
# include <map>
// # include "ConfigFileParser.hpp"
# include "Request.hpp"
class Request;

class Server
{
private:
	int							_masterSockFD;
	std::vector<int>			_masterSockFDs;
	short						_port;
	std::vector<short>			_ports;
	struct sockaddr_in 			_myAddr;
	struct sockaddr_in 			_clientAddr;
	socklen_t					_addrLen;
	fd_set						_masterFDs;
	fd_set						_readFDs;
	fd_set						_writeFDs;
	int							_maxSockFD;
	char*						_fileName;
	std::map<int, std::string>	_clients;

public:
	Server();
    Server(std::vector<short> &, char *);
    // Server(std::vector<HttpsServer> &, char *);
    Server(Server const &);
    ~Server();
    Server & operator=(const Server &);
	
	void createSocket();
	void bindSocket();
	void listenToClient();

	void newConnectHandling(int &);
	void existConnectHandling(int &);

	void exampleOfResponse(char *, int &);
	void createMasterSockets();
};

#endif


