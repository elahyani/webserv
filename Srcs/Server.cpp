#include "Server.hpp"

Server::Server(short port, char *fileName) : _port(port)
{
    for(int i = 0; i < MAX_CLIENTS; i++)
        _sockClients[i] = 0;
    // Socket: creating && binding
    createBindSocket();

    //Listen
    if (listen(_masterSockFd, 3) == -1)
        throw std::runtime_error("Unable to listen for connections.");

    std::cout << "<WAITING FOR INCOMING CONNECTION/>" << std::endl;
    while (true)
    {
        // select work
        FD_ZERO(&_readFds);
        FD_SET(_masterSockFd, &_readFds);
        _maxSockFd = _masterSockFd;
        for(int i = 0; i < MAX_CLIENTS; i++)
        {
            _sockFd = _sockClients[i];
            if (_sockFd > 0)
                FD_SET(_sockFd, &_readFds);
            if (_sockFd > _maxSockFd)
                _maxSockFd = _sockFd;
        }
        if (select(_maxSockFd + 1, &_readFds, NULL, NULL, NULL) == -1)
            throw std::runtime_error("Select unable to manipulexing I/O");
        if (FD_ISSET(_masterSockFd, &_readFds))
        {
                // throw std::runtime_error("server timed out");
            //Accept
            // std::memset(&_cliAddr, 0, sizeof(_cliAddr));
            if ((_newSockFd = accept(_masterSockFd, (struct sockaddr *)&_myAddr, &_addrLen)) == -1)
                throw std::runtime_error("Unable to accept the connection.");

            std::cout << "################ RESQUEST ################" << std::endl;
            // recv
            // char _buffReq[1024] = {0};
            // std::memset(_buffReq, 0, sizeof(_buffReq));
            // while
            // read
            // check if end request
            // if yes break
            // int valRead = recv(_newSockFd, _buffReq, sizeof(_buffReq), 0);

            /*
                
                /r/n
                /r/n
                Content-Length 
                Transfer-Encoding: --boundary
                /r/n/r/n
                BODY
                /r/n/r/n
                
            */

            // std::cout << _buffReq << std::endl;
            // if (valRead == -1)
            // throw std::runtime_error("Unable to receive the request from client.");

            // send the request content
            Request req(_newSockFd);

            req.parseRequest();
            req.printRequest();
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
            msgRes += "HTTP/1.1 200 OK\n"; // HTTP version code msg
            msgRes += "Content-Type: text/html\n";
            msgRes += "Content-Length: " + std::to_string(st.st_size);
            msgRes += "\n\n"; //blank-line
            //Body
            msgRes += _buffRes;
            delete[] _buffRes;
            if (send(_newSockFd, msgRes.c_str(), msgRes.length(), 0) == -1)
                throw std::runtime_error("Unable to send the response from client.");

            // std::cout << msgRes << std::endl << std::endl;
            for(int i = 0; i < MAX_CLIENTS; i++)
            {
                if(_sockClients[i] == 0)
                {
                    _sockClients[i] = _newSockFd;
                    break ;
                }
            }
            // close(_newSockFd);
        }
        for(int i = 0; i < MAX_CLIENTS; i++)
        {
            _sockFd = _sockClients[i];
            if (FD_ISSET(_sockFd, &_readFds))
            {
                char *_buffRes = new char[1024];
                int valRead = recv(_sockFd, _buffRes, 1024, 0);
                if (!valRead)
                {
                    close(_sockFd);
                    _sockClients[i] = 0;   
                }
                else
                {
                    std::cout << "its some IO operation on some other socket." << std::endl;
                    _buffRes[valRead] = '\0';
                    send(_sockFd, _buffRes , 1024 , 0); 
                }
            }
        }
    }
}

Server::Server(Server const &ths)
{
    *this = ths;
    return;
}

Server::~Server()
{
    close(_masterSockFd);
}

Server &Server::operator=(Server const &ths)
{
    if (this != &ths)
    {
        this->_masterSockFd = ths._masterSockFd;
        this->_myAddr = ths._myAddr;
        this->_addrLen = ths._addrLen;
        this->_newSockFd = ths._newSockFd;
        this->_readFds = ths._readFds;
    }
    return *this;
}

void Server::createBindSocket()
{
    //Socket creating
    if ((_masterSockFd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        throw std::runtime_error("Unable to create a socket.");

    //Socket binding
    std::memset(&_myAddr, 0, sizeof(_myAddr));
    _addrLen = sizeof(_myAddr);
    _myAddr.sin_family = AF_INET;
    _myAddr.sin_port = htons(_port);
    _myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(_masterSockFd, (struct sockaddr *)&_myAddr, sizeof(_myAddr)) == -1)
        throw std::runtime_error("Unable to bind the socket");
}