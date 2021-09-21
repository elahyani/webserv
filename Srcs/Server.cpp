#include "Server.hpp"

Server::Server() {}

Server::Server(ConfigFileParser &parser, char *fileName) : _parser(parser), _maxSockFD(0), _fileName(fileName)
{
	_servers.assign(parser.getServers().begin(), parser.getServers().end());
	this->createMasterSockets();

	std::cout << "\t<Server running... waiting for connections./>" << std::endl;
	for (;;)
	{
		FD_ZERO(&_readFDs);
		_readFDs = _masterFDs;
		struct timeval _tv = {1, 0};
		int selectRet = select(_maxSockFD + 1, &_readFDs, &_writeFDs, NULL, &_tv);
		if (selectRet < 0)
			throw std::runtime_error("Unable to select work socket.");
		if (selectRet > 0)
			for (int sockFD = 0; sockFD < _maxSockFD + 1; sockFD++)
			{
				if (FD_ISSET(sockFD, &_readFDs))
				{
					int newConnect = 0;
					for (std::vector<int>::iterator it = _masterSockFDs.begin(); it != _masterSockFDs.end(); it++)
					{
						if (sockFD == *it)
						{
							newConnect = 1;
							break;
						}
					}
					(newConnect) ? this->newConnectHandling(sockFD) : this->existConnectHandling(sockFD);
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
		this->_serverAddr = ths._serverAddr;
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
	std::memset(&_serverAddr, 0, sizeof(_serverAddr));
	_addrLen = sizeof(_serverAddr);
	_serverAddr.sin_family = AF_INET;
	_serverAddr.sin_port = htons(_port);
	if (_host == "ANY")
		_serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	else
		_serverAddr.sin_addr.s_addr = inet_addr(_host.c_str());
	if (bind(_masterSockFD, (struct sockaddr *)&_serverAddr, sizeof(_serverAddr)) == -1)
		throw std::runtime_error("Unable to bind the socket");
}

// Listen for incoming connections from client
void Server::listenToClient()
{
	if (listen(_masterSockFD, 2048) == -1)
		throw std::runtime_error("Unable to listen for connections.");
}

HttpServer Server::findTheTargetServer(int &accptSockFD)
{
	// Find the target server
	HttpServer targetServer;
	std::map<int, int>::iterator it = _serverAddrS.find(accptSockFD);
	struct sockaddr_in serverAddr;
	std::memset(&serverAddr, 0, _addrLen);
	if (getsockname(it->second, (struct sockaddr *)&serverAddr, &_addrLen) < 0)
		throw std::runtime_error("Unable to get server informations from socket " + std::to_string(it->second));
	std::cout << "Master socket fd is " << std::to_string(it->second) << " , ip is : " << inet_ntoa(serverAddr.sin_addr) << " , port : " << std::to_string(ntohs(serverAddr.sin_port)) << std::endl;
	for (std::vector<HttpServer>::iterator it = _servers.begin(); it != _servers.end(); it++)
	{
		if (it->getHost() == inet_ntoa(serverAddr.sin_addr))
		{
			_ports = it->getPorts();
			for (std::vector<short>::iterator itPort = _ports.begin(); itPort != _ports.end(); itPort++)
			{
				if (*itPort == ntohs(serverAddr.sin_port))
				{
					targetServer = *it;
					break;
				}
			}
		}
	}
	return targetServer;
}

void Server::exampleOfResponse(char *fileName, int &accptSockFD)
{
	(void)fileName;

	HttpServer server = findTheTargetServer(accptSockFD);
	// if (_dirPath.size())
	// {
	// 	std::cout << "000000000000000========>>>>>>>>>>>------->>>>>> |" << _dirPath << "|" << std::endl;
	// 	server.getLocations()[_locPos].setRoot(_dirPath);
	// }
	// _dirPath.clear();
	Response response(this->_request, server /*, this */);

	std::string msgRes(""); // Will hold the data that we will send
	// //Header
	// msgRes += "HTTP/1.1 200 OK\n"; // HTTP-version code msg
	// msgRes += "Content-Type: text/html\n";
	// msgRes += "Content-Length: " + std::to_string(st.st_size);
	// msgRes += "\r\n\r\n"; //blank-line
	// //Body
	response.buildResponse();
	msgRes.append(response.getHeaders());
	// msgRes += _buffRes;
	// delete[] _buffRes;
	if (FD_ISSET(accptSockFD, &_writeFDs))
	{
		if (send(accptSockFD, msgRes.c_str(), msgRes.length(), 0) != (ssize_t)msgRes.length())
		{
			throw std::runtime_error("Unable to send the response from client.");
		}
	}
}

void Server::newConnectHandling(int &sockFD)
{
	std::cout << "Master socket is " << std::to_string(sockFD) << std::endl;
	int newSockFD = accept(sockFD, (struct sockaddr *)&_clientAddr, &_addrLen);
	if (newSockFD < -1)
		throw std::runtime_error("Unable to accept the connection.");
	std::cout << "New connection , socket fd is " << std::to_string(newSockFD) << " , ip is : " << inet_ntoa(_clientAddr.sin_addr) << " , port : " << std::to_string(ntohs(_clientAddr.sin_port)) << std::endl;
	struct sockaddr_in addr;
	getsockname(sockFD, (struct sockaddr *)&addr, &_addrLen);
	std::cout << "New connection , Master socket fd is " << std::to_string(sockFD) << " , ip is : " << inet_ntoa(addr.sin_addr) << " , port : " << std::to_string(ntohs(addr.sin_port)) << std::endl;
	if (fcntl(_masterSockFD, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error("Unable to set socket ID: " + std::to_string(newSockFD) + " to non-blocking.");
	FD_SET(newSockFD, &_masterFDs);
	FD_SET(newSockFD, &_writeFDs);
	if (newSockFD > _maxSockFD)
		_maxSockFD = newSockFD;
	_clients.insert(std::pair<int, std::string>(newSockFD, ""));
	std::map<int, int>::iterator it = _serverAddrS.find(newSockFD);
	if (it != _serverAddrS.end())
		it->second = sockFD;
	else
		_serverAddrS.insert(std::pair<int, int>(newSockFD, sockFD));
}

// void Server::setCurrentDir(std::string &dirpath, size_t locPos)
// {
// 	_dirPath = dirpath;
// 	_locPos = locPos;
// }

bool checkRequest(std::string &buffReq)
{
	if (!(buffReq.find("\r\n\r\n") == std::string::npos))
	{
		std::string headers = buffReq.substr(0, buffReq.find("\r\n\r\n") + 4);
		if (headers.find("Content-Length") != std::string::npos)
		{
			size_t length = std::atoi(headers.substr(headers.find("Content-Length: ")).c_str() + 16);
			std::string body = buffReq.substr(buffReq.find("\r\n\r\n") + 4);
			if (body.length() < length)
				return false;
		}
		return true;
	}
	return false;
}

void Server::existConnectHandling(int &accptSockFD)
{
	char _buffRes[1024] = {0};
	int valRead = recv(accptSockFD, _buffRes, sizeof(_buffRes), 0);
	std::cout << "Exist connection , socket fd is " << std::to_string(accptSockFD) << " , ip is : " << inet_ntoa(_clientAddr.sin_addr) << " , port : " << std::to_string(ntohs(_clientAddr.sin_port)) << std::endl;
	if (valRead > 0)
	{
		_buffRes[valRead] = '\0';
		std::map<int, std::string>::iterator it = _clients.find(accptSockFD);
		if (it != _clients.end())
		{
			it->second += _buffRes;
		}
		if (checkRequest(it->second))
		{
			// std::cout << "--------------------------" << std::endl;
			// std::cout << "buffReq... " << it->second << std::endl;
			// std::cout << "--------------------------" << std::endl;

			_request.setRequestData(it->second);
			_request.parseRequest();
			_request.printRequest();
			// Cgi cgi(req);
			// if (it->second.find("Content-Length"))
			// {
			// 	std::cout << "@@@@#####@@@@###@@@@" << it->second.substr(it->second.find("Content-Length")) << std::endl;
			// 	return ;
			// }
			if (FD_ISSET(accptSockFD, &_writeFDs))
			{
				//RESPONSE
				this->exampleOfResponse(_fileName, accptSockFD);
			}
			it->second = "";
		}
	}
	else if (valRead == 0)
	{

		std::cout << "Disconnect socket: " << std::to_string(accptSockFD) << " valRead == " << valRead << std::endl;
		close(accptSockFD);
		FD_CLR(accptSockFD, &_masterFDs);
		FD_CLR(accptSockFD, &_writeFDs);
	}
	else
		return; // Socket is connected but doesn't send request.
}

void Server::createMasterSockets()
{
	FD_ZERO(&_masterFDs);
	FD_ZERO(&_writeFDs);
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
}