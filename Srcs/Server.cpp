#include "Server.hpp"

Server::Server(short port, char *fileName): _port(port)
{
	// Socket: creating && binding 
	createBindSocket();

    //Listen
    if (listen(_sockFd, 50) == -1)
        throw std::runtime_error("Unable to listen for connections.");

    while (true)
    {
    	//Accept
		std::cout << "<WAITING CONNECTION TO ESTABLISGH/>" << std::endl;
		std::memset(&_cliAddr, 0, sizeof(_cliAddr));
        if ((_newSockFd = accept(_sockFd, (struct sockaddr *)&_cliAddr, &_addrLen)) == -1)
            throw std::runtime_error("Unable to accept the connection.");
        // select work 
            FD_ZERO(&_fds);
			struct timeval      _tv;
            _tv.tv_sec = 1;
            _tv.tv_usec = 5;
            FD_SET(_newSockFd, &_fds);
            if(select(_newSockFd + 1, &_fds, &_fds, NULL, &_tv) == -1)
				throw std::runtime_error("Select unable to manipulexing I/O");
            if (!FD_ISSET(_newSockFd, &_fds))
                throw std::runtime_error("server timed out");
    
		std::cout << "################ RESQUEST ################" << std::endl;
		// recv
		char _buffReq[1024] = {0};
        std::memset(_buffReq, 0, sizeof(_buffReq));
        int valRead = recv(_newSockFd, _buffReq, sizeof(_buffReq), 0);


        std::cout << _buffReq << std::endl;
        if(valRead == -1)
			throw std::runtime_error("Unable to receive the request from client.");
		
		// send the request content
		/* Request req(_buffReq);*/

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

		std::string msgRes;// will hold the data that we will send
		//Header
		msgRes += "HTTP/1.1 200 OK\n";// HTTP version code msg
		msgRes += "Content-Type: text/html\n";
		msgRes += "Content-Length: " + std::to_string(st.st_size);
		msgRes += "\n\n"; //blank-line
		//Body
		msgRes += _buffRes;
		delete [] _buffRes;

        if (!FD_ISSET(_newSockFd, &_fds))
                throw std::runtime_error("server timed out");
        if(send(_newSockFd, msgRes.c_str(), msgRes.length(), 0) == -1)
            throw std::runtime_error("Unable to send the response from client.");

        std::cout << msgRes << std::endl << std::endl;
        close(_newSockFd);
    }
}

Server::Server(Server const & ths)
{
    *this = ths;
    return ;
}


Server::~Server()
{
    close(_sockFd);
}

Server & Server::operator=(Server const & ths)
{
    if (this != &ths)
    {
		this->_sockFd = ths._sockFd;
		this->_myAddr = ths._myAddr;
		this->_addrLen = ths._addrLen;
		this->_newSockFd = ths._newSockFd;
		this->_fds = ths._fds;
    }
    return *this;
}

void Server::createBindSocket()
{
	//Socket creating
    if ((_sockFd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        throw std::runtime_error("Unable to create a socket.");
    
	//Socket binding 
    std::memset(&_myAddr, 0, sizeof(_myAddr));
    _addrLen = sizeof(_myAddr);
    _myAddr.sin_family = AF_INET;
    _myAddr.sin_port = htons(_port);
    _myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(_sockFd, (struct sockaddr *)&_myAddr, sizeof(_myAddr))  == -1)
        throw std::runtime_error("Unable to bind the socket");
    
}
