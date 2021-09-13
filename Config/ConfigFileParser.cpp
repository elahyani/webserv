/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFileParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ichejra <ichejra@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/09 15:24:18 by ichejra           #+#    #+#             */
/*   Updated: 2021/09/13 12:34:15 by ichejra          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigFileParser.hpp"

ConfigFileParser::ConfigFileParser() : data(""), locationsNumber(0)
{
}

ConfigFileParser::~ConfigFileParser()
{
}

std::string ConfigFileParser::trimContent(std::string str)
{
	size_t end;
	size_t start = str.find_first_not_of(" \r\v\f\t\n");

	if (start != std::string::npos)
	{
		str = str.substr(start);
		end = str.find_last_not_of(" \r\v\f\t\n");
		if (end != std::string::npos)
			str = str.substr(0, end + 1);
	}
	return str;
}

void ConfigFileParser::semiColonChecker(std::string &buffer)
{
	//check semicolumn in every line that should have one
	if (buffer.back() != ';')
		throw std::invalid_argument("Exception:\tMissing Semicolon in the line ==> " + trimContent(buffer));
	size_t i = 0;
	size_t count = 0;
	while (buffer[i])
	{
		if (buffer[i] == ';')
			count++;
		if (count > 1)
			throw std::invalid_argument("Exception:\tMultiple semicolons in the line ==> " + trimContent(buffer));
		i++;
	}
}

void ConfigFileParser::split(std::string line, char splitter)
{
	int i = 0;
	int k = 0;
	int start = 0;
	int end = line.find(splitter);
	if (!this->mapTmp.empty())
		this->mapTmp.clear();
	while (end != -1)
	{
		k = end;
		this->mapTmp.insert(std::pair<int, std::string>(i, line.substr(start, end - start)));
		while (line[k] == splitter)
		{
			end++;
			k++;
		}
		// std::cout << "Start: " << start << std::endl;
		// std::cout << "end: " << end << std::endl;
		start = end + 1;
		end = line.find(splitter, start);
		i++;
	}
	this->mapTmp.insert(std::pair<int, std::string>(i, line.substr(start, end - start)));
}

void ConfigFileParser::checkFile(int ac, char **av)
{
	if (ac > 2 || ac < 1)
		throw std::invalid_argument("Invalid Arguments");
	else if (ac == 2)
		this->filename = av[1];
	else
		this->filename = "Config/config.conf";
	std::string ext = this->filename.substr(this->filename.find(".") + 1);
	if (ext != "conf")
		throw std::invalid_argument("Exception:\tFile Extention is Invalid");
}

void ConfigFileParser::checkUnit(std::string buffer)
{
	int i = 0;

	while (buffer[i])
	{
		if (buffer.find("m") == std::string::npos || (!std::isdigit(buffer[i]) && buffer[i] != 'm'))
			throw std::invalid_argument("Exception:\tUnrecognized or Missing unit!");
		i++;
	}
}

void ConfigFileParser::checkMissingAttrs()
{
	if (this->inServer)
		throw std::invalid_argument("Exception:\tSyntax Error");
	else if (!this->serversNumber)
		throw std::invalid_argument("Exception:\tServer Not Found");
	else if (this->server.getPorts().empty())
		throw std::invalid_argument("Exception:\tListening port not found!");
	else if (!this->server.getHost().size())
		throw std::invalid_argument("Exception:\tHost not found!");
	else if (this->server.getServerName().empty())
		throw std::invalid_argument("Exception:\tServer_name not found!");
	else if (!this->server.getClientMaxBodySize())
		throw std::invalid_argument("Exception:\tClient_max_body_size not found!");
	else if (this->server.getErrorsPages().empty())
		throw std::invalid_argument("Exception:\tError page not found!");
	else if (!this->server.getRoot().size())
		throw std::invalid_argument("Exception:\tRoot not found!");
}

void ConfigFileParser::checkHost(std::string hostBuffer)
{
	size_t i = -1;
	int dots = 0;

	while (hostBuffer[++i])
	{
		if (hostBuffer[i] == '.')
			dots++;
		if (!std::isdigit(hostBuffer[i]) && hostBuffer[i] != '.')
			throw std::invalid_argument("Exception:\tinvalid format of Host1");
	}
	if (dots != 3)
		throw std::invalid_argument("Exception:\tinvalid format of Host2");
}

void ConfigFileParser::parseLocation(std::string _data)
{
	std::string buffer;
	std::istringstream str(_data.substr(_data.find("location: ")));

	while (std::getline(str, buffer))
	{
		if (buffer.find("{") != std::string::npos)
			this->inLocation = !this->inLocation;
		else if (buffer.find("}") != std::string::npos)
		{
			this->locationsNumber++;
			this->inLocation = !this->inLocation;
			this->locations.push_back(this->location);
			this->location.clearAll();
			return;
		}
		if (this->inLocation)
		{
			if (buffer.find("autoindex=") != std::string::npos)
			{
				semiColonChecker(buffer);
				this->location.setAutoIndex(trimContent(buffer.substr(buffer.find("=") + 1)));
			}
			else if (buffer.find("root=") != std::string::npos)
			{
				semiColonChecker(buffer);
				this->location.setRoot(trimContent(buffer.substr(buffer.find("=") + 1)));
			}
			else if (buffer.find("index=") != std::string::npos)
			{
				semiColonChecker(buffer);
				this->location.setIndex(trimContent(buffer.substr(buffer.find("=") + 1)));
			}
			else if (buffer.find("allow_methods=") != std::string::npos)
			{
				semiColonChecker(buffer);
				this->location.setAllowedMethods(trimContent(buffer.substr(buffer.find("=") + 1)));
			}
			else if (buffer.find("return=") != std::string::npos)
			{
				semiColonChecker(buffer);
				this->location.setReturn(0, trimContent(buffer.substr(buffer.find("=") + 1)));
			}
			else if (buffer.find("fastcgi_pass=") != std::string::npos)
			{
				semiColonChecker(buffer);
				this->location.setFastCgiPass(trimContent(buffer.substr(buffer.find("=") + 1)));
			}
			else if (buffer.find("upload_enable=") != std::string::npos)
			{
				semiColonChecker(buffer);
				this->location.setUploadEnable(trimContent(buffer.substr(buffer.find("=") + 1)));
			}
			else if (buffer.find("upload_store=") != std::string::npos)
			{
				semiColonChecker(buffer);
				this->location.setUploadStore(trimContent(buffer.substr(buffer.find("=") + 1)));
			}
		}
	}
}

void ConfigFileParser::parseConfigFile(int ac, char **av)
{
	checkFile(ac, av);
	std::ifstream file(this->filename);

	int i = 0;

	if (!file.is_open())
		throw std::invalid_argument("Exception:\tFile Not Found!");
	std::string buffer;

	while (std::getline(file, buffer))
	{
		this->data.append(buffer);
		this->data.append("\n");
	}

	std::istringstream str(this->data);
	while (std::getline(str, buffer))
	{
		// trim buffer
		buffer = trimContent(buffer);

		if (buffer.find("#") != std::string::npos)
		{
			buffer = buffer.substr(0, buffer.find_first_of('#'));
			buffer = trimContent(buffer);
			std::cout << "BUFFER AFTER: ********************>>>> |" << buffer << "|" << std::endl;
		}
		// buffer.erase(std::remove_if(buffer.begin(), buffer.end(), ::isspace), buffer.end());
		// std::cout << ">>>>>>>>>>>> " << buffer << std::endl;
		if (buffer.compare("[") == 0)
			this->inServer = !this->inServer;
		else if (buffer.compare("]") == 0)
		{
			this->serversNumber++;
			this->inServer = !this->inServer;
		}
		if (this->inServer)
		{
			// std::cout << "IN SERVER" << std::endl;
			if (buffer.find("listen") != std::string::npos)
			{
				semiColonChecker(buffer);
				size_t j = 0;
				std::string s = buffer.substr(buffer.find(":") + 1);
				s.pop_back();
				s = trimContent(s);
				while (std::isdigit(s[j]))
					j++;
				if (s[j] != '\0')
					throw std::invalid_argument("Exception:\tInvalid port!2");

				int port = std::stoi(buffer.substr(buffer.find(":") + 1));

				// if (this->ports.size())
				if (this->server.getPorts().size())
				{
					for (int i = 0; i < this->server.getPorts().size(); i++)
					{
						if (this->server.getPorts()[i] == port)
							throw std::invalid_argument("Exception:\tDuplicated port!");
					}
				}
				this->server.setPorts(port);
			}
			else if (buffer.find("host") != std::string::npos)
			{
				if (!this->server.getHost().size())
				{
					semiColonChecker(buffer);
					std::string host = buffer.substr(buffer.find(":") + 1);
					this->server.setHost(trimContent(host));
					checkHost(this->server.getHost());
					if (!this->server.getHost().size())
						throw std::invalid_argument("Exception:\tHost not found");
				}
				else
					throw std::invalid_argument("Exception:\tDuplicated " + trimContent(buffer.substr(0, buffer.find(":"))));
			}
			else if (buffer.find("server_name") != std::string::npos)
			{
				if (!this->server.getServerName().size())
				{
					semiColonChecker(buffer);
					buffer.pop_back();
					std::string s = trimContent(buffer.substr(buffer.find(":") + 1));
					this->split(s, ' ');
					while (i < mapTmp.size())
					{
						this->server.setServerName(mapTmp[i]);
						i++;
					}
					if (!this->server.getServerName().size())
						throw std::invalid_argument("Exception:\tServer name not found");
				}
				else
					throw std::invalid_argument("Exception:\tDuplicated " + trimContent(buffer.substr(0, buffer.find(":"))));
			}
			else if (buffer.find("client_max_body_size") != std::string::npos)
			{
				if (!this->server.getClientMaxBodySize())
				{
					semiColonChecker(buffer);
					buffer.pop_back();
					this->checkUnit(this->trimContent(buffer.substr(buffer.find(":") + 1)));
					this->server.setClientMaxBodySize(std::stoi(buffer.substr(buffer.find(":") + 1)));
				}
				else
					throw std::invalid_argument("Exception:\tDuplicated " + trimContent(buffer.substr(0, buffer.find(":"))));
			}
			else if (buffer.find("root") != std::string::npos)
			{
				semiColonChecker(buffer);
				if (buffer.find("root:") != std::string::npos)
				{
					if (!this->server.getRoot().size())
					{
						this->server.setRoot(buffer.substr(buffer.find(":") + 1));
						//////////////////
						std::string s = this->server.getRoot();
						// std::string s = this->root.substr(this->root.find(":") + 1);
						s = trimContent(s);
						this->split(s, ' ');
						std::cout << ">>>>>>>>>;;;; s|" << s << "|\n";
						for (std::map<int, std::string>::iterator it = mapTmp.begin(); it != mapTmp.end(); ++it)
						{
							std::cout << ">>>>>>>>>>>>>>>>>>>>>>|" << it->second << "|\n";
						}
						if (mapTmp.size() != 1)
							throw std::invalid_argument("Exception:\tWrong number of arguments!");
						//////////////////
						if (!this->server.getRoot().size())
							throw std::invalid_argument("Exception:\tRoot path not found");
					}
					else
						throw std::invalid_argument("Exception:\tDuplicated " + trimContent(buffer.substr(0, buffer.find(":"))));
				}
				else if (buffer.find("root=") == std::string::npos)
					throw std::invalid_argument("Exception:\tDuplicated " + trimContent(buffer.substr(0, buffer.find(":"))));
			}
			else if (buffer.find("error_page") != std::string::npos)
			{
				std::cout << "BUFFER =========================> |" << buffer << "|" << std::endl;
				semiColonChecker(buffer);
				buffer.pop_back();

				std::string s = buffer.substr(buffer.find(":") + 1);
				size_t k = 0;

				s = trimContent(s);
				this->split(s, ' ');
				if (mapTmp.size() != 2)
					throw std::invalid_argument("Exception:\tWrong number of arguments!");
				if (isdigit(s[k]))
				{
					while (isdigit(s[k]))
						k++;
				}
				else
					throw std::invalid_argument("Exception:\tInvalid error page!1");
				this->server.setErrorCode(std::stoi(buffer.substr(buffer.find(":") + 1, buffer.find(" /"))));
				if (buffer.find(" /") != std::string::npos)
					this->server.setErrorPagePath(buffer.substr(buffer.find(" /") + 1));
				else
					throw std::invalid_argument("Exception:\tInvalid error page!");
				if (this->server.getErrorPagePath().find(".html") != std::string::npos)
				{
					if (this->server.getErrorPagePath().substr(this->server.getErrorPagePath().find(".html")).compare(".html") != 0)
						throw std::invalid_argument("Exception:\tInvalid extention!");
				}
				else
					throw std::invalid_argument("Exception:\tInvalid extention!");

				// std::cout << "error page path ---> " << errorPagePath << std::endl;
				this->server.setErrorsPages(this->server.getErrorCode(), this->server.getErrorPagePath());
			}
			else if (buffer.find("location:") != std::string::npos)
			{
				this->location.setLocationName(trimContent(buffer.substr(buffer.find(":") + 1)));
				this->parseLocation(data.substr(data.find("location: " + this->location.getLocationName())));
				// this->locationsNumber++;
				//LocationName = buffer.substr(buffer.find(":") + 1);
				//if Location name == "/"
				//{
				// }
				// else if (location name == "\/return")
				//{}
				//else if (location name == *.php)
				//{}
				// else if(Location name == *.py)
				//{}
				// else if (location name == "/upload")
				//{}
				// locations->addLocation();
			}
		}
		std::cout << ">>>>>>>>>>>> " << buffer << std::endl;
	}
	//* CHECK IF AN ATTR IS MISSING HERE
	checkMissingAttrs();
	this->servers.push_back(this->server);
}

void ConfigFileParser::printContentData()
{
	HttpServer s = this->servers[0];
	std::map<int, std::string>::iterator it = s.getErrorsPages().begin();

	std::cout << "----------------------------------------" << std::endl;
	std::cout << "servers number       >>>> " << serversNumber << std::endl;
	for (int i = 0; i < this->server.getPorts().size(); i++)
		std::cout << "listen on            >>>> |" << this->server.getPorts()[i] << "|" << std::endl;
	std::cout << "Host                 >>>> |" << this->server.getHost() << "|" << std::endl;
	for (int i = 0; i < this->server.getServerName().size(); i++)
		std::cout << "Server name          >>>> |" << this->server.getServerName()[i] << "|" << std::endl;
	std::cout << "Client Max Body Size >>>> |" << this->server.getClientMaxBodySize() << "|" << std::endl;
	std::cout << "Root                 >>>> |" << this->server.getRoot() << "|" << std::endl;
	std::cout << "" << std::endl;
	std::cout << "ep size              >>>> |" << s.getErrorsPages().size() << "|" << std::endl;
	for (std::map<int, std::string>::iterator it = s.getErrorsPages().begin(); it != s.getErrorsPages().end(); ++it)
		std::cout << "Error Page           >>>> |" << it->first << "| | |" << it->second << "|" << std::endl;
	// std::cout << "locationsNumber          >>>> |" << this->locationsNumber << "|" << std::endl;
	std::cout << "----------------------------------------" << std::endl;
	for (std::vector<Location>::iterator it = this->locations.begin(); it != locations.end(); ++it)
	{
		std::cout << "LocationName         >>>> |" << it->getLocationName() << "|" << std::endl;
		std::cout << "autoindex            >>>> |" << it->getAutoIndex() << "|" << std::endl;
		std::cout << "root                 >>>> |" << it->getRoot() << "|" << std::endl;
		std::cout << "index                >>>> |" << it->getIndex() << "|" << std::endl;
		std::cout << "allow_methods        >>>> |" << it->getAllowedMethods() << "|" << std::endl;
		std::cout << "return               >>>> |" << it->getReturn() << "|" << std::endl;
		std::cout << "fastCgiPass          >>>> |" << it->getFastCgiPass() << "|" << std::endl;
		std::cout << "uploadEnable         >>>> |" << it->getUploadEnable() << "|" << std::endl;
		std::cout << "uploadStore          >>>> |" << it->getUploadStore() << "|" << std::endl;
		std::cout << "••••••••••••••••••" << std::endl;
	}

	std::cout << "----------------------------------------" << std::endl;
}