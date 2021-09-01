#include "Server.hpp"

int main(int ac, char **av)
{
    try
    {
        if(ac != 3)
			throw std::runtime_error("Wrong number of arguments.");
        Server server(std::atoi(av[1]), av[2]);
    }
    catch(const std::exception& exp)
    {
        std::cerr << "Error: " << exp.what() << '\n';
		exit(EXIT_FAILURE);

    }
    return 0;
}
