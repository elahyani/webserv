#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <string>
# include <sys/socket.h>
# include <sys/types.h>
# include <limits.h>

class server
{
private:
    int _serverFd;
    int _accptFd;

public:
    server();
    ~server();
};

server::server()
{
}

server::~server()
{
}


#endif