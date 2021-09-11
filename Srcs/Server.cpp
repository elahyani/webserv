#include "Server.hpp"

Server::Server(short port, char *fileName) : _port(port), _fileName(fileName)
{
    // for(int i = 0; i < MAX_CLI
    // Socket creating
    this->createSocket();
    FD_ZERO(&_readFds);
    FD_ZERO(&_writeFds);

	// Bind socket
	this->bindSocket();
	FD_SET(_masterSockFd, &_readFds);
	_maxSockFd = (_masterSockFd > _maxSockFd) ? _masterSockFd : _maxSockFd;
	_sockFds.push_back(_masterSockFd);

    // Listen to client in socket 
	this->listenToClient();
	for(;;)
    {
    	FD_ZERO(&_tmpReadFds); // tmp of fd_set for select
		_tmpReadFds = _readFds;
    	std::cout << "\t\t<WAITING FOR INCOMING CONNECTION/>" << std::endl;
		std::cout << "SocketsFDs size: " << _sockFds.size() << std::endl;
		int timeout = 1;
		struct timeval _tv = {timeout, 0}; // set the time
		int selectRet = select(_maxSockFd + 1, &_tmpReadFds, NULL, NULL, &_tv);
		if (selectRet < 0)
			throw std::runtime_error("Unable to select work socket.");
		for(std::vector<int>::iterator it = _sockFds.begin(); it != _sockFds.end(); it++)
		{
			if(FD_ISSET(*it, &_tmpReadFds))
			{
				if(*it == _masterSockFd)
				{
					this->newConnectHandling();
					break ;
				}
				else
				{
					this->existConnectHandling(it);
					break ;
				}
			}
		}
    }
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
	close(_newSockFd);
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
	fcntl(_masterSockFd, F_SETFL, O_NONBLOCK);

	// set socket option
	int opt = 1;
	if (setsockopt(_masterSockFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
		throw std::runtime_error("Unable to set socket option.");
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
	if (listen(_masterSockFd, 5) == -1)
		throw std::runtime_error("Unable to listen for connections.");
}

void Server::exampleOfResponse(char *fileName)
{
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
	msgRes += "\r\n\r\n"; //blank-line
	//Body
	msgRes += _buffRes;
	delete [] _buffRes;
	if (FD_ISSET(_newSockFd, &_writeFds))
		if (send(_newSockFd, msgRes.c_str(), msgRes.length(), 0) != (ssize_t)msgRes.length())
			throw std::runtime_error("Unable to send the response from client.");
}

void Server::newConnectHandling()
{
	std::cout << "An master socket: ID: " << std::to_string(_masterSockFd) << std::endl;
	if ((_newSockFd = accept(_masterSockFd, (struct sockaddr *)&_clientAddr, &_addrLen)) == -1)
		throw std::runtime_error("Unable to accept the connection.");
	std::cout << "MyAdrr: " << _myAddr.sin_addr.s_addr << " ClientAddr: " << _clientAddr.sin_addr.s_addr << std::endl;
	FD_SET(_newSockFd, &_readFds);
	FD_SET(_newSockFd, &_writeFds);
	std::cout << "################ RESQUEST ################" << std::endl;
	// send the request content
	// Request req(_newSockFd);
	
	// req.parseRequest();
	// req.printRequest();
	// exit(1);
	_sockFds.push_back(_newSockFd);
	std::cout << "An new accept socket: ID: " << std::to_string(_newSockFd) << std::endl;
	if(_newSockFd > _maxSockFd)
		_maxSockFd = _newSockFd;
	std::cout << "################ RESPONSE ################" << std::endl;
	this->exampleOfResponse(_fileName);
}

void Server::existConnectHandling(std::vector<int>::iterator & _existSock)
{
	char _buffRes[1024];
	int valRead = recv(*_existSock, _buffRes, sizeof(_buffRes), 0);
	if (valRead == 0)
	{
		std::cout << "Removed socket: " << std::to_string(*_existSock) << std::endl;
		close(*_existSock);
		FD_CLR(*_existSock, &_readFds);
		FD_CLR(*_existSock, &_writeFds);
		std::cout << "New size before removing socket: " << _sockFds.size() << std::endl;
		_sockFds.erase(_existSock);
		std::cout << "New size after removing socket: " << _sockFds.size() << std::endl;

	}
	else
	{
		_buffRes[valRead] = '\0';
		if (FD_ISSET(*_existSock, &_writeFds))
			if (send(*_existSock, _buffRes, valRead , 0) != valRead)
				throw std::runtime_error("Unable to send Response in socket file descriptor " + std::to_string(*_existSock));
	}
}
