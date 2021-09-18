#include "Server.hpp"

int main(int ac, char **av)
{
	ConfigFileParser parser;

    try
    {
		parser.parseConfigFile(ac, av);
		// parser.printContentData();
		
		Server server(parser.getServers(), av[2]);

    }
    catch(const std::exception& exp)
    {
        std::cerr << "Error: " << exp.what() << '\n';
		return (1);
    }
    return (0);
}
