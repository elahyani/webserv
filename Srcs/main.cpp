#include "Server.hpp"

int main(int ac, char **av)
{
  try
  {
    ConfigFileParser parser;
    parser.parseConfigFile(ac, av);
    Server server(parser);
  }
  catch (const std::exception &exp)
  {
    std::cerr << "Error: " << exp.what() << '\n';
    return (1);
  }
  return (0);
}
