#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <string>
# include <sys/socket.h>
# include <sys/types.h>
# include <sys/select.h>
# include <limits.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <fcntl.h>
# include <unistd.h>

class Server
{
private:
    int                 _sockFd;
    int                 _cliFd;
    struct sockaddr_in  _myAddr;
    struct sockaddr_in  _cliAddr;
    socklen_t           _addrLen;
    int                 _newSockFd;
    fd_set              _fds;
    struct timeval      _tv;

public:
    Server();
    Server(short port);
    Server(Server const & ths);
    ~Server();
    Server & operator=(const Server & ths);
    class SocketFailedToCreate : public std::exception
    {
    public:
        const char * what() const throw();
    };
    class SocketFailedToBind : public std::exception
    {
    public:
        const char * what() const throw();
    };
    class SocketFailedToListen : public std::exception
    {
    public:
        const char * what() const throw();
    };
    class SocketFailedToAccept : public std::exception
    {
    public:
        const char * what() const throw();
    };
    class SocketFailedToRecv : public std::exception
    {
    public:
        const char * what() const throw();
    };
    class SocketFailedToSend : public std::exception
    {
    public:
        const char * what() const throw();
    };

    class TimedOut : public std::exception
    {
    public:
        const char *what() const throw();
    };
};

#endif