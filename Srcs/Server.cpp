#include "Server.hpp"

Server::Server() {}

Server::Server(std::vector<HttpServer> &servers, char *fileName) : _servers(servers), _maxSockFD(0), _fileName(fileName)
{
	this->createMasterSockets();

	std::cout << "\t<Server running... waiting for connections./>" << std::endl;
	for (;;)
	{
		FD_ZERO(&_readFDs);
		_readFDs = _masterFDs;
		int timeout = 1;
		struct timeval _tv = {timeout, 0};
		int selectRet = select(_maxSockFD + 1, &_readFDs, &_writeFDs, NULL, &_tv);
		if (selectRet < 0)
			throw std::runtime_error("Unable to select work socket.");
		if (selectRet > 0)
			for (int sockFD = 0; sockFD < _maxSockFD + 1; sockFD++)
			{
				if (FD_ISSET(sockFD, &_readFDs))
				{
					int _newConnect = 0;
					for (std::vector<int>::iterator it = _masterSockFDs.begin(); it != _masterSockFDs.end(); it++)
					{
						if (sockFD == *it)
						{
							_newConnect = 1;
							break;
						}
					}
					(_newConnect) ? this->newConnectHandling(sockFD) : this->existConnectHandling(sockFD);
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
	for (std::vector<int>::iterator it = _masterSockFDs.begin(); it != _masterSockFDs.end(); it++)
	{
		close(*it);
	}
	_masterSockFDs.clear();
	_clients.clear();
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
	if (_host == "ANY")
		_myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	else
		_myAddr.sin_addr.s_addr = inet_addr(_host.c_str());
	if (bind(_masterSockFD, (struct sockaddr *)&_myAddr, sizeof(_myAddr)) == -1)
		throw std::runtime_error("Unable to bind the socket");
}

// Listen for incoming connections from client
void Server::listenToClient()
{
	if (listen(_masterSockFD, 2048) == -1)
		throw std::runtime_error("Unable to listen for connections.");
}

void Server::exampleOfResponse(char *fileName, int &existSockFD)
{
	(void)fileName;
	

	Response response(this->_request);
	
	std::string msgRes(""); // Will hold the data that we will send
	// //Header
	// msgRes += "HTTP/1.1 200 OK\n"; // HTTP-version code msg
	// msgRes += "Content-Type: text/html\n";
	// msgRes += "Content-Length: " + std::to_string(st.st_size);
	// msgRes += "\r\n\r\n"; //blank-line
	// //Body
	response.buildHeaders();
	msgRes.append(response.getHeaders());
	// msgRes += _buffRes;
	// delete[] _buffRes;
	if (FD_ISSET(existSockFD, &_writeFDs))
	{
		if (send(existSockFD, msgRes.c_str(), msgRes.length(), 0) != (ssize_t)msgRes.length())
		{
			throw std::runtime_error("Unable to send the response from client.");
		}
	}
}

void Server::newConnectHandling(int &sockFD)
{
	std::cout << "Master socket is " << std::to_string(_masterSockFD) << std::endl;
	int newSockFD = accept(sockFD, (struct sockaddr *)&_clientAddr, &_addrLen);
	if (newSockFD < -1)
		throw std::runtime_error("Unable to accept the connection.");
	std::cout << "New connection , socket fd is " << std::to_string(newSockFD) << " , ip is : " << inet_ntoa(_clientAddr.sin_addr) << " , port : " << std::to_string(ntohs(_clientAddr.sin_port)) << std::endl;
	if (fcntl(_masterSockFD, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error("Unable to set socket ID: " + std::to_string(newSockFD) + " to non-blocking.");
	FD_SET(newSockFD, &_masterFDs);
	FD_SET(newSockFD, &_writeFDs);
	if (newSockFD > _maxSockFD)
		_maxSockFD = newSockFD;
	_clients.insert(std::pair<int, std::string>(newSockFD, ""));
}

bool checkRequest(std::string &req)
{
	std::string data;
	size_t i;

	i = req.find("\r\n\r\n");
	if (i == std::string::npos)
		return false;
	if (req.find("Content-Length") != std::string::npos)
	{

		data = req.substr(i + 4);
		if (data.find("\r\n\r\n") == std::string::npos)
			return false;
	}
	return true;
}

void Server::existConnectHandling(int &existSockFD)
{
	char _buffRes[1024] = {0};
	int valRead = recv(existSockFD, _buffRes, sizeof(_buffRes), 0);
	std::cout << "Exist connection , socket fd is " << std::to_string(existSockFD) << " , ip is : " << inet_ntoa(_clientAddr.sin_addr) << " , port : " << std::to_string(ntohs(_clientAddr.sin_port)) << std::endl;
	if (valRead > 0)
	{
		_buffRes[valRead] = '\0';
		std::map<int, std::string>::iterator it = _clients.find(existSockFD);
		if (it != _clients.end())
		{
			it->second += _buffRes;
		}
		if (checkRequest(it->second))
		{
			std::cout << "Request ====> "<< it->second << std::endl;
			_request.setRequestData(it->second);
			_request.parseRequest();
			_request.printRequest();
			// (void)_fileName;
			// Cgi cgi(req);
			// if (it->second.find("Content-Length"))
			// {
			// 	std::cout << "@@@@#####@@@@###@@@@" << it->second.substr(it->second.find("Content-Length")) << std::endl;
			// 	return ;
			// }
			if (FD_ISSET(existSockFD, &_writeFDs))
			{
				//RESPONSE
				this->exampleOfResponse(_fileName, existSockFD);
			}
		}
	}
	else if (valRead == 0)
	{

		std::cout << "Disconnect socket: " << std::to_string(existSockFD) << " valRead == " << valRead << std::endl;
		close(existSockFD);
		FD_CLR(existSockFD, &_masterFDs);
		FD_CLR(existSockFD, &_writeFDs);
	}
	else
		return; // Socket is connected but doesn't send request.
}

void Server::createMasterSockets()
{
	FD_ZERO(&_masterFDs);
	// foreach server
	// {
	for (std::vector<HttpServer>::iterator itServer = _servers.begin(); itServer != _servers.end(); itServer++)
	{
		_ports = itServer->getPorts();
		_host = itServer->getHost();
		for (std::vector<short>::iterator itPort = _ports.begin(); itPort != _ports.end(); ++itPort)
		{
			_port = *itPort;
			std::cout << _host << ':' << _port << std::endl;
			// Socket creating
			try
			{
				this->createSocket();
				// Bind socket
				this->bindSocket();
				FD_SET(_masterSockFD, &_masterFDs);
				_maxSockFD = (_masterSockFD > _maxSockFD) ? _masterSockFD : _maxSockFD;
				// Listen to client in socket
				this->listenToClient();
				_masterSockFDs.push_back(_masterSockFD);
			}
			catch (const std::exception &e)
			{
				close(_masterSockFD);
				std::cerr << e.what() << '\n';
			}
		}
	}
	FD_ZERO(&_writeFDs);
	_writeFDs = _masterFDs;
}