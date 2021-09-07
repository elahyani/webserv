#include "Server.hpp"

Server::Server(short port, char *fileName) : _port(port)
{
    // for(int i = 0; i < MAX_CLIENTS; i++)
    //     _sockClients[i] = 0;
    // Socket creating
    FD_ZERO(&_readFds);
    FD_ZERO(&_masterFds);
    this->createSocket();
	// setsockopt
	int opt = 1;
	if (setsockopt(_masterSockFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
		throw std::runtime_error("Unable to set socket option.");
	// Bind socket
	this->bindSocket();
    //Listen
	this->listenToClient();
    
	FD_SET(_masterSockFd, &_masterFds);
    _maxSockFd = _masterSockFd;
	// std::cout << "Socket: " + std::to_string(_masterSockFd) << "Max Socket: " + std::to_string(_maxSockFd) << std::endl;

    std::cout << "<WAITING FOR INCOMING CONNECTION/>" << std::endl;
    while (1)
    {
		// std::cout << "Socket: " + std::to_string(_masterSockFd) << " Max Socket: " + std::to_string(_maxSockFd) << std::endl;
        // select work
		int timeout = 15;
		struct timeval _tv = {0, timeout * 1000};
		_readFds = _masterFds;
        if (select(_maxSockFd + 1, &_readFds, NULL, NULL, &_tv) == -1)
            throw std::runtime_error("server timed out");
            // throw std::runtime_error("Select unable to manipulexing I/O");
		for (int i = 0; i <= _maxSockFd; i++)
		{
			if (FD_ISSET(i, &_readFds))
			{
				if (i == _masterSockFd)
				{
					//Accept
					if ((_newSockFd = accept(_masterSockFd, (struct sockaddr *)&_clientAddr, &_addrLen)) == -1)
						throw std::runtime_error("Unable to accept the connection.");
					std::cout << "MyAdrr: " << _myAddr.sin_addr.s_addr << " ClientAddr: " << _clientAddr.sin_addr.s_addr << std::endl;
					FD_SET(_newSockFd, &_masterFds);
					if (_newSockFd > _maxSockFd)
						_maxSockFd = _newSockFd;
					std::cout << "################ RESQUEST ################" << std::endl;
					// send the request content
					// Request req(_newSockFd);

					// req.parseRequest();
					// req.printRequest();
					// exit(1);
					std::cout << "################ RESPONSE ################" << std::endl;
					//Response file <html>
					struct stat st;
					stat(fileName, &st);
					std::fstream fdRes;
					fdRes.open(fileName, std::ios::in);
					if (!fdRes)
						throw std::runtime_error("Unable to open response file.");
					char *_buffRes = new char[st.st_size + 1];
					fdRes.read(_buffRes, st.st_size);
					fdRes.close();

					std::string msgRes; // will hold the data that we will send
					//Header
					msgRes += "HTTP/1.1 200 OK\n"; // HTTP-version code msg
					msgRes += "Content-Type: text/html\n";
					msgRes += "Content-Length: " + std::to_string(st.st_size);
					msgRes += "\n\n"; //blank-line
					//Body
					msgRes += _buffRes;
					delete [] _buffRes;
					if (send(_newSockFd, msgRes.c_str(), msgRes.length(), 0) == -1)
						throw std::runtime_error("Unable to send the response from client.");
					// close(_newSockFd);
				}
				else
				{
					char *_buffRes = new char[1024];
					int valRead = recv(i, _buffRes, sizeof(_buffRes), 0);
					if (valRead <= 0)
					{
					std::cout << "Socket: " + std::to_string(i) << std::endl;
						close(i);
						FD_CLR(i, &_masterFds);
					}
					else
					{
						_buffRes[valRead] = '\0';
						for(int j = 0; j  <= _maxSockFd; j++)
						{
							if(FD_ISSET(i, &_masterFds))
							{
								if (i != j && j != _masterSockFd)
								{
									if (send(i, _buffRes, valRead , 0) <= 0)
										throw std::runtime_error("Unable to send Response in socket file descriptor " + std::to_string(j));
								}
							}
						}
					}
					delete [] _buffRes;
				}
			}
		}
    }
	std::cout << "server down" << std::endl;
}

// Copy constructor
Server::Server(Server const &ths)
{
    *this = ths;
    return;
}

// Destructor
Server::~Server()
{
    close(_masterSockFd);
}

// Assignment operator=
Server &Server::operator=(Server const &ths)
{
    if (this != &ths)
    {
        this->_masterSockFd = ths._masterSockFd;
		this->_port = ths._port;
        this->_myAddr = ths._myAddr;
        this->_addrLen = ths._addrLen;
        this->_newSockFd = ths._newSockFd;
        this->_readFds = ths._readFds;
		this->_maxSockFd = ths._maxSockFd;
		this->_sockFd = ths._sockFd;
    }
    return *this;
}

// Socket creating
void Server::createSocket()
{
    if ((_masterSockFd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        throw std::runtime_error("Unable to create a socket.");

}

// Socket binding
void Server::bindSocket()
{
    std::memset(&_myAddr, 0, sizeof(_myAddr));
    _addrLen = sizeof(_myAddr);
    _myAddr.sin_family = AF_INET;
    _myAddr.sin_port = htons(_port);
    _myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(_masterSockFd, (struct sockaddr *)&_myAddr, sizeof(_myAddr)) == -1)
        throw std::runtime_error("Unable to bind the socket");
}

// Listen for incoming connections from client
void Server::listenToClient()
{
	if (listen(_masterSockFd, 50) == -1)
		throw std::runtime_error("Unable to listen for connections.");
}