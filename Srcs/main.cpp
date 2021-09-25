#include "Server.hpp"

int main(int ac, char **av)
{
  ConfigFileParser parser;

<<<<<<< HEAD
  try
  {
    parser.parseConfigFile(ac, av);
    // parser.printContentData();
    // exit(1);
    Server server(parser, av[2]);
  }
  catch (const std::exception &exp)
  {
    std::cerr << "Error: " << exp.what() << '\n';
    return (1);
  }
  return (0);
=======
    try
    {
		parser.parseConfigFile(ac, av);
		// parser.printContentData();
		Server server(parser);

    }
    catch(const std::exception& exp)
    {
        std::cerr << "Error: " << exp.what() << '\n';
		  return (1);
    }
    return (0);
>>>>>>> 10cf81e (refactor(webserv): Accept the new changin)
}
