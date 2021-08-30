#include "Server.hpp"

Server::Server(short port)
{
    (void)_cliFd;

    //Create a socket
    if ((_sockFd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        throw std::runtime_error("Unable to create a socket.");
    
	//Socket binding 
    std::memset(&_myAddr, 0, sizeof(_myAddr));
    _addrLen = sizeof(_myAddr);
    _myAddr.sin_family = AF_INET;
    _myAddr.sin_port = htons(port); // >= 5000
    _myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(_sockFd, (struct sockaddr *)&_myAddr, sizeof(_myAddr))  == -1)
        throw std::runtime_error("Unable to bind the socket");
    
    //Listen
    if (listen(_sockFd, 50) == -1)
        throw std::runtime_error("Unable to listen for connections.");

    //Accept
    while (true)
    {
        std::cout << "<WAITING CONNECTION TO ESTABLISGH/>" << std::endl;
        std::memset(&_cliAddr, 0, sizeof(_cliAddr));
        if ((_newSockFd = accept(_sockFd, (struct sockaddr *)&_cliAddr, &_addrLen)) == -1)
            throw std::runtime_error("Unable to accept the connection.");
        // select work 
            FD_ZERO(&_fds);
            _tv.tv_sec = 2;
            _tv.tv_usec = 0;
            FD_SET(_newSockFd, &_fds);
            if(select(_newSockFd + 1, &_fds, &_fds, NULL, &_tv) == -1)
				throw std::runtime_error("Select unable to manipulexing I/O");
            if (!FD_ISSET(_newSockFd, &_fds))
                throw std::runtime_error("server timed out");
    
        // recv
        char buffer[1024];
        // std::memset(buffer, 0, sizeof(buffer));
        
        int valRead = recv(_newSockFd, buffer, sizeof(buffer), 0);
		std::cout << "################ RESQUEST ################" << std::endl;
        std::cout << buffer << std::endl;
        if(valRead == -1)
            throw std::runtime_error("Unable to receive the request from client.");
            
        if (!FD_ISSET(_newSockFd, &_fds))
                throw std::runtime_error("server timed out");
		
		std::string msg = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 47\n\n<html><body><H1>Hello world</H1></body></html>";
		const char *msgToSend = msg.c_str();

        if(send(_newSockFd, msgToSend, strlen(msgToSend), 0) == -1)
            throw std::runtime_error("Unable to send the response from client.");
		std::cout << "################ RESPONSE ################" << std::endl;
        std::cout << msgToSend << std::endl << std::endl;
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
        // need to add all the attributes
        //any dynamic allocation means deepcopy
    }
    return *this;
}
