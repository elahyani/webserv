#include "Server.hpp"

Server::Server(short port, char *fileName) : _port(port), _fileName(fileName)
{
    // Socket creating
    this->createSocket();
    FD_ZERO(&_readFDs);
    FD_ZERO(&_writeFDs);

	// Bind socket
	this->bindSocket();
	FD_SET(_masterSockFD, &_readFDs);
	FD_SET(_masterSockFD, &_writeFDs);
	_maxSockFD = (_masterSockFD > _maxSockFD) ? _masterSockFD : _maxSockFD;

    // Listen to client in socket 
	this->listenToClient();
	for(;;)
    {
    	FD_ZERO(&_tmpReadFDs); // tmp of fd_set for select
		_tmpReadFDs = _readFDs;
    	// std::cout << "\t\t<WAITING FOR INCOMING CONNECTION/>" << std::endl;
		int timeout = 1;
		struct timeval _tv = {timeout, 0}; // set the time
		int selectRet = select(_maxSockFD + 1, &_tmpReadFDs, &_writeFDs, NULL, &_tv);
		if (selectRet < 0)
			throw std::runtime_error("Unable to select work socket.");
		if (selectRet > 0)
			for (int i = 0; i < _maxSockFD + 1; i++)
			{
				if(FD_ISSET(i, &_tmpReadFDs))
					(i == _masterSockFD) ? this->newConnectHandling(i) : this->existConnectHandling(i);
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
    close(_masterSockFD);
	close(_newSockFD);
}

// Assignment operator=
Server &Server::operator=(Server const &ths)
{
    if (this != &ths)
    {
        this->_masterSockFD = ths._masterSockFD;
		this->_port = ths._port;
        this->_myAddr = ths._myAddr;
        this->_addrLen = ths._addrLen;
        this->_newSockFD = ths._newSockFD;
        this->_readFDs = ths._readFDs;
		this->_maxSockFD = ths._maxSockFD;
		this->_sockFD = ths._sockFD;
    }
    return *this;
}

// Socket creating
void Server::createSocket()
{
    if ((_masterSockFD = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        throw std::runtime_error("Unable to create a socket.");
	fcntl(_masterSockFD, F_SETFL, O_NONBLOCK);

	// set socket option
	int opt = 1;
	if (setsockopt(_masterSockFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
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
    if (bind(_masterSockFD, (struct sockaddr *)&_myAddr, sizeof(_myAddr)) == -1)
        throw std::runtime_error("Unable to bind the socket");
}

// Listen for incoming connections from client
void Server::listenToClient()
{
	if (listen(_masterSockFD, 5) == -1)
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
	std::string msgRes; // Will hold the data that we will send
	//Header
	msgRes += "HTTP/1.1 200 OK\n"; // HTTP-version code msg
	msgRes += "Content-Type: text/html\n";
	msgRes += "Content-Length: " + std::to_string(st.st_size);
	msgRes += "\r\n\r\n"; //blank-line
	//Body
	msgRes += _buffRes;
	delete [] _buffRes;
	if (FD_ISSET(_newSockFD, &_writeFDs))
		if (send(_newSockFD, msgRes.c_str(), msgRes.length(), 0) != (ssize_t)msgRes.length())
			throw std::runtime_error("Unable to send the response from client.");
}

void Server::newConnectHandling(int &sockFD)
{
	// char _buffRes[1024] = {0};
	std::cout << "An master socket: ID: " << std::to_string(_masterSockFD) << std::endl;
	if ((_newSockFD = accept(sockFD, (struct sockaddr *)&_clientAddr, &_addrLen)) == -1)
		throw std::runtime_error("Unable to accept the connection.");
	std::cout << "New connection , socket fd is " << std::to_string(_newSockFD) << " , ip is : " << inet_ntoa(_clientAddr.sin_addr) << " , port : " << std::to_string(_clientAddr.sin_port) << std::endl;
	fcntl(_newSockFD, F_SETFL, O_NONBLOCK);
	FD_SET(_newSockFD, &_readFDs);
	std::cout << "################ RESQUEST ################" << std::endl;
	FD_SET(_newSockFD, &_writeFDs);
	// recv(_newSockFD, _buffRes, strlen(_buffRes), 0);
	// std::cout << "======== 1 ========> " << _buffRes << std::endl;
	// send the request content
	Request req(_newSockFD);
	
	req.parseRequest();
	req.printRequest();
	if(_newSockFD > _maxSockFD)
		_maxSockFD = _newSockFD;
	std::cout << "################ RESPONSE ################" << std::endl;
	this->exampleOfResponse(_fileName);
}

void Server::existConnectHandling(int &existSockFD)
{
	char _buffRes[1024] = {0};
	int valRead = recv(existSockFD, _buffRes, sizeof(_buffRes), 0);
	std::cout << "======== 2 ========> " << _buffRes << std::endl;
	if (valRead == 0)
	{
		std::cout << "Removed socket: " << std::to_string(existSockFD) << std::endl;
		close(existSockFD);
		FD_CLR(existSockFD, &_readFDs);
		FD_CLR(existSockFD, &_writeFDs);
	}
	else
	{
		_buffRes[valRead] = '\0';
		if (FD_ISSET(existSockFD, &_writeFDs))
			if (send(existSockFD, _buffRes, valRead , 0) != valRead)
				throw std::runtime_error("Unable to send Response in socket file descriptor " + std::to_string(existSockFD));
	}
}
