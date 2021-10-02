#include "Server.hpp"

// Default constructor
Server::Server() :	_masterSockFD(0),
					_port(0),
					_host(""),
					_addrLen(0),
					_maxSockFD(0),
					_isChunked(false),
					_contentLength(0)
{}

Server::Server(ConfigFileParser & parser) :	_parser(parser),
											_masterSockFD(0),
											_port(0),
											_host(""),
											_addrLen(0),
											_maxSockFD(0),
											_isChunked(false),
											_contentLength(0),
											_portServer(0),
											_mbs(0)
{
	_servers.assign(parser.getServers().begin(), parser.getServers().end());
	this->makeSockets();
	this->waitingForConnections();
}

// Copy constructor
Server::Server(Server const &ths)
{
	*this = ths;
	return;
}

// Destructor
Server::~Server() {
	for (std::vector<int>::iterator it = _masterSockFDs.begin(); it != _masterSockFDs.end(); it++)
	{
		close(*it);
	}
	_servers.clear();
	_masterSockFDs.clear();
	_ports.clear();
	_clients.clear();
	_accptMaster.clear();
}

// Assignment operator=
Server &Server::operator=(Server const &ths)
{
	if (this != &ths)
	{
		this->_parser = ths._parser;
		this->_servers = ths._servers;
		this->_masterSockFDs = ths._masterSockFDs;
		this->_masterSockFD = ths._masterSockFD;
		this->_ports = ths._ports;
		this->_port = ths._port;
		this->_host = ths._host;
		this->_serverAddr = ths._serverAddr;
		this->_clientAddr = ths._clientAddr;
		this->_addrLen = ths._addrLen;
		this->_masterFDs = ths._masterFDs;
		this->_readFDs = ths._readFDs;
		this->_writeFDs = ths._writeFDs;
		this->_maxSockFD = ths._maxSockFD;
		this->_clients = ths._clients;
		this->_accptMaster = ths._accptMaster;
		this->_request = ths._request;
	}
	return *this;
}

void Server::makeSockets()
{
	// fd_set structures initializing
	FD_ZERO(&_masterFDs);
	FD_ZERO(&_writeFDs);
	// making master sockets for()
	for (std::vector<HttpServer>::iterator itServer = _servers.begin(); itServer != _servers.end(); itServer++)
	{
		_ports = itServer->getPorts();
		_host = itServer->getHost();
		for (std::vector<short>::iterator itPort = _ports.begin(); itPort != _ports.end(); ++itPort) {
			_port = *itPort;
			try
			{
				// Socket creating
				this->createSocket();
				// Bind a name to a socket
				this->bindSocket();
				// Listen for socket connections
				this->listenSocket();
				std::cout << "New socket " + std::to_string(_masterSockFD) + " bind to "<< _host << ':' << _port << std::endl;
			}
			catch (const std::exception &e)
			{
				close(_masterSockFD);
				std::cerr << e.what() << '\n';
			}
		}
	}
}

// Socket creating
void Server::createSocket() {
	if ((_masterSockFD = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		throw std::runtime_error("Unable to create a socket.");
	// set socket to non-blocking
	if (fcntl(_masterSockFD, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("Unable to set the socket " + std::to_string(_masterSockFD) + " to non-blocking.");
	// set socket option of reusing address 
	int opt = 1;
	if (setsockopt(_masterSockFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1)
		throw std::runtime_error("Unable to set socket option to the socket " + std::to_string(_masterSockFD));
}

// Socket binding
void Server::bindSocket() {
	std::memset(&_serverAddr, 0, sizeof(_serverAddr));
	_addrLen = sizeof(_serverAddr);
	_serverAddr.sin_family = AF_INET;
	_serverAddr.sin_port = htons(_port);
	_serverAddr.sin_addr.s_addr = (_host == "ANY") ? htonl(INADDR_ANY) : inet_addr(_host.c_str());
	if (bind(_masterSockFD, (struct sockaddr *)&_serverAddr, sizeof(_serverAddr)) == -1)
		throw std::runtime_error("Unable to bind the socket " + std::to_string(_masterSockFD) + " with " + _host + ":" + std::to_string(_port) + ": Already used");
}

// Listen for incoming connections from clients
void Server::listenSocket() {
	if (listen(_masterSockFD, 2000) == -1)
		throw std::runtime_error("Unable to listen for connections in the socket " + std::to_string(_masterSockFD));
	// set socket to fd_set struct	
	FD_SET(_masterSockFD, &_masterFDs);
	_maxSockFD = (_masterSockFD > _maxSockFD) ? _masterSockFD : _maxSockFD;
	// Add the socket to the sockets vector
	_masterSockFDs.push_back(_masterSockFD);
}

void Server::waitingForConnections() {
	std::cout << "\t<Server running... waiting for connections./>" << std::endl;
	for(;;) {
		FD_ZERO(&_readFDs);
		_readFDs = _masterFDs;
		usleep(2000);
		// std::cout << "==> " <<  _maxSockFD << std::endl;
		struct timeval _tv = {0, 0};
		int activity = select(_maxSockFD + 1, &_readFDs, &_writeFDs, NULL, &_tv);
		if (activity == -1)
			throw std::runtime_error("Select failed to multiplexing Input/Output.");
		if (activity > 0)
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
					(newConnect) ? this->newConnectHandling(sockFD) : this->accptedConnectHandling(sockFD);
				}
			}
	}
}

void Server::newConnectHandling(int &sockFD)
{
	int accptSockFD = accept(sockFD, (struct sockaddr *)&_clientAddr, &_addrLen);
	if (accptSockFD == -1)
		throw std::runtime_error("Unable to accept the connection from client by the socket " + std::to_string(accptSockFD));
	std::cout << "New connection: Master socket " << std::to_string(sockFD) << ". Accept socket " + std::to_string(accptSockFD) << ", address " << inet_ntoa(_clientAddr.sin_addr) << ":" << std::to_string(ntohs(_clientAddr.sin_port)) << std::endl;
	if (fcntl(accptSockFD, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("Unable to set the socket " + std::to_string(accptSockFD) + " to non-blocking.");
	FD_SET(accptSockFD, &_masterFDs);
	FD_SET(accptSockFD, &_writeFDs);
	if (accptSockFD > _maxSockFD)
		_maxSockFD = accptSockFD;
	_clients.insert(std::pair<int, std::string>(accptSockFD, ""));
	std::map<int, int>::iterator it = _accptMaster.find(accptSockFD);
	if (it != _accptMaster.end())
		it->second = sockFD;
	else
		_accptMaster.insert(std::pair<int, int>(accptSockFD, sockFD));
}

bool Server::detectEndRequest(std::string &buffReq, int &accptSockFD)
{
	getServerBySocket(accptSockFD, &_server, &_portServer);
	_mbs = _server.getClientMaxBodySize() * 1024 * 1024;
	if (!(buffReq.find("\r\n\r\n") == std::string::npos))
	{
		std::string headers = buffReq.substr(0, buffReq.find("\r\n\r\n") + 4);
		if (headers.find("Content-Length") != std::string::npos)
		{
			try
			{
				size_t length = std::stoi(headers.substr(headers.find("Content-Length: ") + 16));
				std::string body = buffReq.substr(buffReq.find("\r\n\r\n") + 4);

				if (length > (size_t)_mbs)
					return true;
				else if (body.length() < length)
					return false;
			}
			catch(const std::exception& e)
			{
				std::cerr << e.what() << '\n';
			}
			
		}
		else if (headers.find("Transfer-Encoding: chunked") != std::string::npos)
		{
			_isChunked = true;
			if (buffReq.find("0\r\n\r\n") == std::string::npos)
				return false;
		}
		return true;
	}
	return false;
}
// body.len > max * 1024 * 1024;
void Server::accptedConnectHandling(int &accptSockFD)
{
	// HttpServer server;
	// short port = 0;
	char _buffRes[BUFFER_SIZE + 1] = { 0 };
	bzero(_buffRes, sizeof(_buffRes));
	int valRead = recv(accptSockFD, _buffRes, BUFFER_SIZE, 0);
	std::cout << "Activity in socket " << std::to_string(accptSockFD) << ", address: " << inet_ntoa(_clientAddr.sin_addr) << ':' << std::to_string(ntohs(_clientAddr.sin_port)) << std::endl;
	
	if (valRead > 0)
	{
		_buffRes[valRead] = '\0';
		std::map<int, std::string>::iterator it = _clients.find(accptSockFD);
		if (it != _clients.end())
			it->second += _buffRes;
		if (detectEndRequest(it->second, accptSockFD))
		{
			std::cout << "*********************" << std::endl;
			std::cout << it->second << std::endl;
			std::cout << "*********************" << std::endl;
			if (_isChunked)
				it->second = unchunkingRequest(it->second);
			_request.setRequestData(it->second, _mbs);
			_request.parseRequest();
			_request.printRequest();
			if (FD_ISSET(accptSockFD, &_writeFDs))
			{
				this->responseHandling(accptSockFD);
			}
			it->second = "";
		}
	}
	else if (valRead == 0)
	{
		std::cout << "Disconnected socket " << std::to_string(accptSockFD) << std::endl;
		close(accptSockFD);
		FD_CLR(accptSockFD, &_masterFDs);
		FD_CLR(accptSockFD, &_writeFDs);
		_clients.erase(accptSockFD);
	}
	else
		return ; // Socket is connected but doesn't send request.
}

size_t getChunkedDataSize(std::string & chunkSize) {
 	size_t size = 0;
	std::stringstream stream(chunkSize);
	stream >> std::hex >> size;
	return size;
}

std::string Server::unchunkingRequest(std::string &request)
{
	std::string body = request.substr(request.find("\r\n\r\n") + 4);
	std::string unchunkedData = request.substr(0, request.find("\r\n\r\n") + 4);
	std::string line("");
	size_t chunkSize;
	
	_contentLength = 0;
	std::stringstream bodyStream(body);
	for(;;) {
		int end = 0;
		std::getline(bodyStream, line);
		chunkSize = getChunkedDataSize(line);
		if (chunkSize == 0) end++;
		std::getline(bodyStream, line);
		if (std::strcmp(line.c_str(), "\r\n")) end++;
		unchunkedData.append(line.c_str(), chunkSize);
		if (end == 2) break ;
		_contentLength += chunkSize;
	}
	_isChunked = false;
	return unchunkedData;
}

void Server::getServerBySocket(int &accptSockFD, HttpServer *server, short *port)
{
	std::map<int, int>::iterator it = _accptMaster.find(accptSockFD);
	struct sockaddr_in serverAddr;
	std::memset(&serverAddr, 0, _addrLen);
	if (getsockname(it->second, (struct sockaddr *)&serverAddr, &_addrLen) == -1)
		throw std::runtime_error("Unable to get server informations from socket " + std::to_string(it->second));
	for (std::vector<HttpServer>::iterator it = _servers.begin(); it != _servers.end(); it++)
	{
		if (it->getHost() == inet_ntoa(serverAddr.sin_addr))
		{
			_ports = it->getPorts();
			for (std::vector<short>::iterator itPort = _ports.begin(); itPort != _ports.end(); itPort++)
			{
				if (*itPort == ntohs(serverAddr.sin_port))
				{
					*server = *it;
					*port = *itPort;
					return ;
				}
			}
		}
	}
}

void Server::responseHandling(int &accptSockFD)
{
	Response response(this->_request, _server, _portServer);
	std::string msgRes(""); // Will hold the data that we will send
	response.buildResponse();
	msgRes.append(response.getHeaders());
	if (FD_ISSET(accptSockFD, &_writeFDs))
	{
		if (send(accptSockFD, msgRes.c_str(), msgRes.length(), 0) != (ssize_t)msgRes.length())
		{
			throw std::runtime_error("Unable to send the response to client in socket " + std::to_string(accptSockFD));
		}
		if (_request.getHeaderVal("Connection").compare("close") == 0)
		{
			std::cout << "Disconnected socket " << std::to_string(accptSockFD) << std::endl;
			close(accptSockFD);
			FD_CLR(accptSockFD, &_masterFDs);
			FD_CLR(accptSockFD, &_writeFDs);
		}
		_request.clearRequest();
		response.clearAll();
	}
}
