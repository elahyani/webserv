#include "Server.hpp"

int main(int ac, char **av)
{
	ConfigFileParser parser;

    try
    {
		parser.parseConfigFile(ac, av);
		Server server(parser.getServers(), av[2]);
		// parser.printContentData();

        // if(ac != 2)
		// 	throw std::runtime_error("Wrong number of arguments.");
		// std::vector<short> ports;
		// ports.push_back(5000);
		// ports.push_back(7000);
		// ports.push_back(9000);
        // Server server(ports, av[1]);
    }
    catch(const std::exception& exp)
    {
        std::cerr << "Error: " << exp.what() << '\n';
		return (1);
    }
    return (0);
}
