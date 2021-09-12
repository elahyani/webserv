#include "Server.hpp"

Server::Server()
{}

Server::Server(short port, char *fileName) : _port(port), _fileName(fileName)
{
    // Socket creating
    this->createSocket();

	// Bind socket
	this->bindSocket();
    FD_ZERO(&_masterFDs);
    FD_ZERO(&_writeFDs);
	FD_SET(_masterSockFD, &_masterFDs);
	_writeFDs = _masterFDs;
	_maxSockFD = (_masterSockFD > _maxSockFD) ? _masterSockFD : _maxSockFD;

    // Listen to client in socket 
	this->listenToClient();
    std::cout << "\t<Server running... waiting for connections./>" << std::endl;
	for(;;)
    {
    	FD_ZERO(&_readFDs); // tmp of fd_set for select
		_readFDs = _masterFDs;
		int timeout = 1;
		struct timeval _tv = {timeout, 0}; // set the time
		int selectRet = select(_maxSockFD + 1, &_readFDs, &_writeFDs, NULL, &_tv);
		if (selectRet < 0)
			throw std::runtime_error("Unable to select work socket.");
		if (selectRet > 0)
			for (int i = 0; i < _maxSockFD + 1; i++)
			{
				if(FD_ISSET(i, &_readFDs))
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
    }
    return *this;
}

// Socket creating
void Server::createSocket()
{
    if ((_masterSockFD = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        throw std::runtime_error("Unable to create a socket.");
	if (fcntl(_masterSockFD, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error("Unable to set socket ID: " + std::to_string(_masterSockFD) + " to non-blocking.");
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
	std::cout << "An master socket: ID: " << std::to_string(_masterSockFD) << std::endl;
	if ((_newSockFD = accept(sockFD, (struct sockaddr *)&_clientAddr, &_addrLen)) == -1)
		throw std::runtime_error("Unable to accept the connection.");
	std::cout << "New connection , socket fd is " << std::to_string(_newSockFD) << " , ip is : " << inet_ntoa(_myAddr.sin_addr) << " , port : " << std::to_string(ntohs(_myAddr.sin_port)) << std::endl;
	if (fcntl(_masterSockFD, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error("Unable to set socket ID: " + std::to_string(_masterSockFD) + " to non-blocking.");
	FD_SET(_newSockFD, &_masterFDs);
	FD_SET(_newSockFD, &_writeFDs);
	std::cout << "################ RESQUEST ################" << std::endl;
	/*	
	 char _buffRes[1024] = {0};
	 int ret = 0;
	 for (; (ret = recv(_newSockFD, _buffRes, 1024, 0)) > 0;)
	 {
	 	std::cout << "recv return == " + std::to_string(ret) << " _buffRes == "<< _buffRes << std::endl;
	 }
	 if (ret < 0)
	 	std::cout << "recv return == " + std::to_string(ret) << " _buffRes == "<< _buffRes << std::endl;
	*/
	// send the request content
	// std::cout << "here" << std::endl;
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
	// std::cout << "======== 2 ========> " << _buffRes << std::endl;
	if (valRead == 0)
	{
		std::cout << "Removed socket: " << std::to_string(existSockFD) << std::endl;
		close(existSockFD);
		FD_CLR(existSockFD, &_masterFDs);
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

void Server::createSockets()
{

}