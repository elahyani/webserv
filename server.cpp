#include "Server.hpp"

Server::Server(){}

Server::Server(short port)
{
    (void)_cliFd;

    //Create a socket
    if ((_sockFd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        throw SocketFailedToCreate();
    

    std::memset(&_myAddr, 0, sizeof(_myAddr));

    _addrLen = sizeof(_myAddr);
    _myAddr.sin_family = AF_INET;
    _myAddr.sin_port = htons(port); // >= 5000
    _myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(_sockFd, (struct sockaddr *)&_myAddr, sizeof(_myAddr))  == -1)
        throw SocketFailedToBind();
    //Listen
    if (listen(_sockFd, 50) == -1)
        throw SocketFailedToListen();

    //Accept
    while (true)
    {
        std::cout << "Waiting for a request from the client" << std::endl;
        std::memset(&_cliAddr, 0, sizeof(_cliAddr));
        if ((_newSockFd = accept(_sockFd, (struct sockaddr *)&_cliAddr, &_addrLen)) == -1)
            throw SocketFailedToAccept();
        

        // recv
        char buffer[1024] = {0};
        if (recv(_newSockFd, buffer, sizeof(buffer), 0) == -1)
            throw SocketFailedToRecv();
        char hello[13] = "Hello World!";
        if(send(_newSockFd, hello, strlen(hello), 0) == -1)
            throw SocketFailedToSend();
        std::cout << "Hello msg has been send" << std::endl;
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
        // need to add all the attributes: any allocation means deepcopy
    }
    return *this;
}

// Exceptions

const char * Server::SocketFailedToCreate::what() const throw()
{
    return "Socket hasn't been created";
}

const char * Server::SocketFailedToBind::what() const throw()
{
    return "Socket hasn't been bind";
}

const char * Server::SocketFailedToListen::what() const throw()
{
    return "Socket can not listen";
}

const char * Server::SocketFailedToAccept::what() const throw()
{
    return "Socket hasn't been created";
}

const char * Server::SocketFailedToRecv::what() const throw()
{
    return "Server can not receive message from client";
}

const char * Server::SocketFailedToSend::what() const throw()
{
    return "Server can not message to client";
}