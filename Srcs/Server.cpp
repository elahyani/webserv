#include "Server.hpp"

Server::Server(short port, char *fileName) : _port(port)
{
    // for(int i = 0; i < MAX_CLI
    // Socket creating
    this->createSocket();

    // Bind socket
    this->bindSocket();
    // Listen to client in socket
    this->listenToClient();

    _maxSockFd = _masterSockFd;
    std::cout << "<WAITING FOR INCOMING CONNECTION/>" << std::endl;
    while (1)
    {
        FD_ZERO(&_readFds);
        FD_SET(_masterSockFd, &_readFds);
        if (_masterSockFd > _maxSockFd)
            _maxSockFd = _masterSockFd;
        // select work
        int timeout = 1;
        struct timeval _tv = {timeout, 0}; // set the time to chek
        int iSelect = select(_maxSockFd + 1, &_readFds, NULL, NULL, &_tv);
        if (iSelect > 0)
        {
            // _newSockFd = accept(_ma)
            if ((_newSockFd = accept(_masterSockFd, (struct sockaddr *)&_clientAddr, &_addrLen)) == -1)
                throw std::runtime_error("Unable to accept the connection.");
            if (FD_ISSET(_masterSockFd, &_readFds))
            {
                std::cout << "MyAdrr: " << _myAddr.sin_addr.s_addr << " ClientAddr: " << _clientAddr.sin_addr.s_addr << std::endl;
                FD_SET(_newSockFd, &_readFds);
                std::cout << "################ RESQUEST ################" << std::endl;
                // send the request content
                Request req(_newSockFd);

                req.readRequest();
                req.parseRequest();
                req.printRequest();

                // exit(1);
                std::cout << "################ RESPONSE ################" << std::endl;
                Response resp(req);

                resp.buildHeaders();
                resp.buildResponse();
                exit(1);
                // (void)fileName;
                this->exampleOfResponse(fileName);
                _listSocketFds.push_back(_newSockFd);
            }
        }
        for (std::list<int>::iterator it = _listSocketFds.begin(); it != _listSocketFds.end(); it++)
        {
            FD_SET(*it, &_readFds);
            if (*it > 0 && *it > _maxSockFd)
                _maxSockFd = *it;
        }
        iSelect = select(_maxSockFd + 1, &_readFds, NULL, NULL, &_tv);
        if (iSelect > 0)
        {
            // std::cout << "list_size: "<< _listSocketFds.size() << std::endl;
            for (std::list<int>::iterator it = _listSocketFds.begin(); it != _listSocketFds.end(); it++)
            {
                if (FD_ISSET(*it, &_readFds))
                {
                    char *_buffRes[1024];
                    int valRead = recv(*it, _buffRes, sizeof(_buffRes), 0);
                    if (valRead == 0)
                    {
                        close(*it);
                        _listSocketFds.remove(*it);
                    }
                    else
                    {
                        if (send(*it, _buffRes, valRead, 0) != valRead)
                            throw std::runtime_error("Unable to send Response in socket file descriptor " + std::to_string(*it));
                    }
                    break;
                }
                // std::cout << "list_size: "<< _listSocketFds.size() << std::endl;
            }
        }
    }
    std::cout << "server down" << std::endl;
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
    // setsockopt
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
    msgRes += "\n\n"; //blank-line
    //Body
    msgRes += _buffRes;
    delete[] _buffRes;
    if (send(_newSockFd, msgRes.c_str(), msgRes.length(), 0) != (ssize_t)msgRes.length())
        throw std::runtime_error("Unable to send the response from client.");
}