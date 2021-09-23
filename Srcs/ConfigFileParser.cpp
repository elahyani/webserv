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

ConfigFileParser::ConfigFileParser() : _data(""),
									   _locationsNumber(0),
									   _inLocation(false),
									   _countauto(0),
									   _isEnabled(0)
{
}

ConfigFileParser::~ConfigFileParser()
{
	//////////////////////////
	this->_servers.clear();
	this->_mapTmp.clear();
	this->_location.clearAll();
	this->_server.clearAll();
	//////////////////////////
}

ConfigFileParser::ConfigFileParser(ConfigFileParser const &src)
{
	*this = src;
}

ConfigFileParser &ConfigFileParser::operator=(ConfigFileParser const &src)
{
	//! use assign
	if (this != &src)
	{
	}
	return *this;
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

void ConfigFileParser::syntaxChecker(std::string &buffer, int inLoc)
{
	//check semicolumn in every line that should have one
	size_t semiColon = std::count(buffer.begin(), buffer.end(), ';');
	size_t fWColon = std::count(buffer.begin(), buffer.end(), ':');
	size_t equal = std::count(buffer.begin(), buffer.end(), '=');

	if (buffer.back() != ';')
		throw std::invalid_argument("Exception:\tMissing Semicolon in the line ==> " + trimContent(buffer));
	else if (semiColon > 1)
		throw std::invalid_argument("Exception:\tMultiple semicolons in the line ==> " + trimContent(buffer));
	else if (fWColon > 1 || equal > 1 || (inLoc && !equal) || (!inLoc && !fWColon))
		throw std::invalid_argument("Exception:\tSyntax Error at line ==> " + trimContent(buffer));
}

void ConfigFileParser::split(std::string line, char splitter)
{
	int i = 0;
	int k = 0;
	int start = 0;
	int end = line.find(splitter);

	this->_mapTmp.clear();
	while (end != -1)
	{
		k = end;
		this->_mapTmp.insert(std::pair<int, std::string>(i, line.substr(start, end - start)));
		while (line[k] == splitter)
		{
			end++;
			k++;
		}
		// std::cout << "Start: " << start << std::endl;
		// std::cout << "end: " << end << std::endl;
		start = end;
		end = line.find(splitter, start);
		i++;
	}
	this->_mapTmp.insert(std::pair<int, std::string>(i, line.substr(start, end - start)));
}

void ConfigFileParser::checkFile(int ac, char **av)
{
	// if (ac > 2 || ac < 1)
	// 	throw std::invalid_argument("Invalid Arguments");
	if (ac == 2)
		this->_filename = av[1];
	else
		this->_filename = "ConfigFiles/config.conf";
	std::string ext = this->_filename.substr(this->_filename.find(".") + 1);
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
	if (this->_inServer)
		throw std::invalid_argument("Exception:\tSyntax Error");
	else if (!this->_serversNumber)
		throw std::invalid_argument("Exception:\tServer Not Found");
	else if (this->_server.getPorts().empty())
		throw std::invalid_argument("Exception:\tListening port not found!");
	else if (!this->_server.getHost().size())
		this->_server.setHost("ANY");
	else if (this->_server.getServerName().empty())
		throw std::invalid_argument("Exception:\tServer_name not found!");
	else if (!this->_server.getClientMaxBodySize())
		throw std::invalid_argument("Exception:\tClient_max_body_size not found!");
	else if (this->_server.getErrorsPages().empty())
		throw std::invalid_argument("Exception:\tError page not found!");
	else if (!this->_server.getRoot().size())
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
	// bool isRootSeted = false;
	std::string buffer;
	std::istringstream str(_data.substr(_data.find("location: ")));

	this->_countauto = 0;
	this->_isEnabled = 0;
	// std::string locBlock = _data.substr(0, _data.find("}") + 1);

	// std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
	// std::cout << locBlock << std::endl;
	// std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
	// if (locBlock.find("root") == std::string::npos)
	// {
	// 	isRootSeted = true;
	// 	_location.setRoot(_server.getRoot());
	// }
	while (std::getline(str, buffer))
	{
		if (buffer.find("#") != std::string::npos)
		{
			buffer = buffer.substr(0, buffer.find_first_of('#'));
			buffer = trimContent(buffer);
		}
		if (buffer.find("{") != std::string::npos)
			this->_inLocation = true;
		else if (buffer.find("}") != std::string::npos)
		{
			this->_locationsNumber++;
			this->_inLocation = false;
			this->_server.setLocation(this->_location);
			this->_location.clearAll();
			return;
		}
		if (this->_inLocation)
		{
			if (buffer.find("autoindex") != std::string::npos)
			{
				this->_countauto++;
				syntaxChecker(buffer, 1);
				buffer.pop_back();
				std::string tmp = trimContent(buffer.substr(buffer.find("=") + 1));
				if (this->_countauto > 1)
					throw std::invalid_argument("Exception:\tDuplicated autoindex");
				if (!tmp.size())
					throw std::invalid_argument("Exception:\tAutoindex value not found");
				this->_location.setAutoIndex(tmp);
			}
			else if (buffer.find("root") != std::string::npos)
			{
				if (!this->_location.getRoot().size())
				{
					syntaxChecker(buffer, 1);
					buffer.pop_back();
					this->split(trimContent(buffer.substr(buffer.find("=") + 1)), ' ');
					if (_mapTmp.size() != 1)
						throw std::invalid_argument("Exception:\tWrong number of arguments");
					this->_location.setRoot(trimContent(buffer.substr(buffer.find("=") + 1)));
					// this->_server.setRoot(this->_location.getRoot());
					if (!this->_location.getRoot().size())
						throw std::invalid_argument("Exception:\tRoot path not found");
				}
				else
					throw std::invalid_argument("Exception:\tDuplicated root");
			}
			else if (buffer.find("index") != std::string::npos)
			{
				if (!this->_location.getIndex().size())
				{
					syntaxChecker(buffer, 1);
					std::string index = trimContent(buffer.substr(buffer.find("=") + 1));
					index.pop_back();
					if (!index.size())
						throw std::invalid_argument("Exception:\tindex not found");
					this->_location.setIndex(trimContent(index));
				}
				else
					throw std::invalid_argument("Exception:\tDuplicated index");
			}
			else if (buffer.find("allow_methods") != std::string::npos)
			{
				if (this->_location.getAllowedMethods().empty())
				{
					syntaxChecker(buffer, 1);
					std::string allow_mtd = trimContent(buffer.substr(buffer.find("=") + 1));
					allow_mtd.pop_back();
					if (!allow_mtd.size())
						throw std::invalid_argument("Exception:\tAllow methods not found");
					buffer.pop_back();
					std::string allow_methods = buffer.substr(buffer.find("[") + 1, buffer.find("]"));
					allow_methods.pop_back();
					this->split(allow_methods, ',');
					if (this->_mapTmp.size() > 3)
						throw std::invalid_argument("Exception:\tExceeded max methods length");
					for (size_t i = 0; i < _mapTmp.size(); i++)
					{
						if (trimContent(_mapTmp[i]).compare("GET") != 0 && trimContent(_mapTmp[i]).compare("POST") != 0 && trimContent(_mapTmp[i]).compare("DELETE") != 0)
							throw std::invalid_argument("Exception:\tMethod is not allowed");
						this->_location.setAllowedMethods(trimContent(_mapTmp[i]));
					}
				}
				else
					throw std::invalid_argument("Exception:\tDuplicated Allow_methods attr");
			}
			else if (buffer.find("return") != std::string::npos)
			{
				if (this->_location.getReturn().empty())
				{
					syntaxChecker(buffer, 1);
					buffer.pop_back();
					this->split(trimContent(buffer.substr(buffer.find("=") + 1)), ' ');
					if (_mapTmp.size() != 2)
						throw std::invalid_argument("Exception:\tWrong number of arguments");
					this->_location.setReturn(std::stoi(_mapTmp[0]), trimContent(_mapTmp[1]));
				}
				else
					throw std::invalid_argument("Exception:\tDuplicated return");
			}
			else if (buffer.find("fastcgi_pass") != std::string::npos)
			{
				if (!this->_location.getFastCgiPass().size())
				{
					syntaxChecker(buffer, 1);
					buffer.pop_back();
					this->split(trimContent(buffer.substr(buffer.find("=") + 1)), ' ');
					if (_mapTmp.size() != 1)
						throw std::invalid_argument("Exception:\tWrong number of arguments");
					this->_location.setFastCgiPass(trimContent(buffer.substr(buffer.find("=") + 1)));
					// std::cout << "+_*/------>>>>................" << std::endl;
					// std::cout << "+_*/------>>>> " << _location.getIsCGI() << std::endl;
					if (!this->_location.getFastCgiPass().size())
						throw std::invalid_argument("Exception:\tfastcgi_pass path not found");
					this->_location.setIsCGI(true);
				}
				else
					throw std::invalid_argument("Exception:\tDuplicated fastcgi_pass");
			}
			else if (buffer.find("upload_enable") != std::string::npos)
			{
				this->_isEnabled++;
				syntaxChecker(buffer, 1);
				buffer.pop_back();
				std::string tmp = trimContent(buffer.substr(buffer.find("=") + 1));
				if (_isEnabled > 1)
				{
					std::cout << _isEnabled << std::endl;
					throw std::invalid_argument("Exception:\tDuplicated upload_enable");
				}
				if (!tmp.size())
					throw std::invalid_argument("Exception:\tUpload_enable value not found");
				this->_location.setUploadEnable(tmp);
			}
			else if (buffer.find("upload_store") != std::string::npos)
			{
				if (!this->_location.getUploadStore().size())
				{
					syntaxChecker(buffer, 1);
					std::string upstore = trimContent(buffer.substr(buffer.find("=") + 1));
					upstore.pop_back();
					if (!upstore.size())
						throw std::invalid_argument("Execption:\tupload_store path not found");
					this->split(trimContent(buffer.substr(buffer.find("=") + 1)), ' ');
					if (_mapTmp.size() != 1)
						throw std::invalid_argument("Exception:\tWrong number of arguments");
					this->_location.setUploadStore(trimContent(buffer.substr(buffer.find("=") + 1)));
				}
				else
					throw std::invalid_argument("Exception:\tDuplicated upload_store");
			}
		}
	}
}

void ConfigFileParser::parseConfigFile(int ac, char **av)
{
	checkFile(ac, av);
	std::ifstream file(this->_filename);

	if (!file.is_open())
		throw std::invalid_argument("Exception:\tFile Not Found!");
	std::string buffer;

	while (std::getline(file, buffer))
	{
		this->_data.append(trimContent(buffer));
		this->_data.append("\n");
	}

	std::istringstream str(this->_data);
	while (std::getline(str, buffer))
	{
		// trim buffer
		buffer = trimContent(buffer);

		if (buffer.find("#") != std::string::npos)
		{
			buffer = buffer.substr(0, buffer.find_first_of('#'));
			buffer = trimContent(buffer);
		}
		// buffer.erase(std::remove_if(buffer.begin(), buffer.end(), ::isspace), buffer.end());
		// std::cout << ">>>>>>>>>>>> " << buffer << std::endl;
		if (buffer.compare("[") == 0)
			this->_inServer = !this->_inServer;
		else if (buffer.compare("]") == 0)
		{
			this->_serversNumber++;
			this->_inServer = !this->_inServer;
			this->checkMissingAttrs();
			this->_servers.push_back(this->_server);
			this->_server.clearAll();
			this->_data = this->_data.substr(_data.find("]\n"));
		}
		if (this->_inServer)
		{
			// std::cout << "IN SERVER" << std::endl;
			if (buffer.find("listen") != std::string::npos)
			{
				syntaxChecker(buffer, 0);
				size_t j = 0;
				std::string s = buffer.substr(buffer.find(":") + 1);
				s.pop_back();
				s = trimContent(s);
				while (std::isdigit(s[j]))
					j++;
				if (s[j] != '\0')
					throw std::invalid_argument("Exception:\tInvalid port!2");

				int port = std::stoi(buffer.substr(buffer.find(":") + 1));

				if (this->_server.getPorts().size())
				{
					for (size_t i = 0; i < this->_server.getPorts().size(); i++)
					{
						if (this->_server.getPorts()[i] == port)
							throw std::invalid_argument("Exception:\tDuplicated port!");
					}
				}
				this->_server.setPorts(port);
			}
			else if (buffer.find("host") != std::string::npos)
			{
				if (!this->_server.getHost().size())
				{
					syntaxChecker(buffer, 0);
					buffer.pop_back();
					std::string host = buffer.substr(buffer.find(":") + 1);
					this->_server.setHost(trimContent(host));
					checkHost(this->_server.getHost());
					if (!this->_server.getHost().size())
						this->_server.setHost("ANY");
				}
				else
					throw std::invalid_argument("Exception:\tDuplicated " + trimContent(buffer.substr(0, buffer.find(":"))));
			}
			else if (buffer.find("server_name") != std::string::npos)
			{
				if (!this->_server.getServerName().size())
				{
					syntaxChecker(buffer, 0);
					buffer.pop_back();
					this->split(trimContent(buffer.substr(buffer.find(":") + 1)), ' ');
					for (size_t i = 0; i < _mapTmp.size(); i++)
						this->_server.setServerName(_mapTmp[i]);
					if (!this->_server.getServerName().size())
						throw std::invalid_argument("Exception:\tServer name not found");
				}
				else
					throw std::invalid_argument("Exception:\tDuplicated " + trimContent(buffer.substr(0, buffer.find(":"))));
			}
			else if (buffer.find("client_max_body_size") != std::string::npos)
			{
				if (!this->_server.getClientMaxBodySize())
				{
					syntaxChecker(buffer, 0);
					buffer.pop_back();
					this->checkUnit(this->trimContent(buffer.substr(buffer.find(":") + 1)));
					this->_server.setClientMaxBodySize(std::stoi(buffer.substr(buffer.find(":") + 1)));
				}
				else
					throw std::invalid_argument("Exception:\tDuplicated " + trimContent(buffer.substr(0, buffer.find(":"))));
			}
			else if (buffer.find("root") != std::string::npos)
			{
				if (buffer.find("root:") != std::string::npos)
				{
					syntaxChecker(buffer, 0);
					buffer.pop_back();
					if (!this->_server.getRoot().size())
					{
						this->_server.setRoot(trimContent(buffer.substr(buffer.find(":") + 1)));
						std::string s = this->_server.getRoot();
						s = trimContent(s);
						this->split(s, ' ');
						if (_mapTmp.size() != 1)
							throw std::invalid_argument("Exception:\tWrong number of arguments!");
						if (!this->_server.getRoot().size())
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
				syntaxChecker(buffer, 0);
				buffer.pop_back();

				std::string s = buffer.substr(buffer.find(":") + 1);
				size_t k = 0;

				s = trimContent(s);
				this->split(s, ' ');
				if (_mapTmp.size() != 2)
					throw std::invalid_argument("Exception:\tWrong number of arguments!");
				if (isdigit(s[k]))
				{
					while (isdigit(s[k]))
						k++;
				}
				else
					throw std::invalid_argument("Exception:\tInvalid error page!1");
				this->_server.setErrorCode(std::stoi(buffer.substr(buffer.find(":") + 1, buffer.find(" /"))));
				if (buffer.find(" /") != std::string::npos)
					this->_server.setErrorPagePath(buffer.substr(buffer.find(" /") + 1));
				else
					throw std::invalid_argument("Exception:\tInvalid error page!");
				if (this->_server.getErrorPagePath().find(".html") != std::string::npos)
				{
					if (this->_server.getErrorPagePath().substr(this->_server.getErrorPagePath().find(".html")).compare(".html") != 0)
						throw std::invalid_argument("Exception:\tInvalid extention!");
				}
				else
					throw std::invalid_argument("Exception:\tInvalid extention!");

				// std::cout << "error page path ---> " << errorPagePath << std::endl;
				this->_server.setErrorsPages(this->_server.getErrorCode(), this->_server.getErrorPagePath());
			}
			else if (buffer.find("location") != std::string::npos)
			{
				this->_location.setLocationName(trimContent(buffer.substr(buffer.find(":") + 1)));
				this->split(this->_location.getLocationName(), ' ');
				if (_mapTmp.size() != 1)
					throw std::invalid_argument("Exception:\tWrong number of arguments");
				this->parseLocation(_data.substr(_data.find("location: " + this->_location.getLocationName())));
			}
		}
		// std::cout << "|" << buffer << "|" << std::endl;
	}
}

void ConfigFileParser::printContentData()
{
	std::cout << "-------------------[SERVER]-------------------" << std::endl;
	for (std::vector<HttpServer>::iterator it = this->_servers.begin(); it != this->_servers.end(); ++it)
	{
		std::cout << "servers number       .... " << _serversNumber << std::endl;
		for (size_t i = 0; i < it->getPorts().size(); i++)
			std::cout << "listen on            .... |" << it->getPorts()[i] << "|" << std::endl;
		std::cout << "Host                 .... |" << it->getHost() << "|" << std::endl;
		for (size_t i = 0; i < it->getServerName().size(); i++)
			std::cout << "Server name          .... |" << it->getServerName()[i] << "|" << std::endl;
		std::cout << "Client Max Body Size .... |" << it->getClientMaxBodySize() << "|" << std::endl;
		std::cout << "Root                 .... |" << it->getRoot() << "|" << std::endl;
		for (std::map<int, std::string>::iterator err = it->getErrorsPages().begin(); err != it->getErrorsPages().end(); ++err)
			std::cout << "Error Page           .... |" << err->first << "| | |" << err->second << "|" << std::endl;
		std::cout << "••••••••••••••••••" << std::endl;
		std::cout << "\n-------------------[LOCATIONS]-------------------" << std::endl;
		for (size_t i = 0; i < it->getLocations().size(); i++)
		{
			std::cout << "LocationName         .... |" << it->getLocations()[i].getLocationName() << "|" << std::endl;
			std::cout << "autoindex            .... |" << it->getLocations()[i].getAutoIndex() << "|" << std::endl;
			std::cout << "root                 .... |" << it->getLocations()[i].getRoot() << "|" << std::endl;
			std::cout << "index                .... |" << it->getLocations()[i].getIndex() << "|" << std::endl;
			for (size_t j = 0; j < it->getLocations()[i].getAllowedMethods().size(); j++)
				std::cout << "allow_methods        .... |" << it->getLocations()[i].getAllowedMethods()[j] << "|" << std::endl;
			for (std::map<int, std::string>::iterator ret = it->getLocations()[i].getReturn().begin(); ret != it->getLocations()[i].getReturn().end(); ++ret)
				std::cout << "return               .... |" << ret->first << "| |" << ret->second << "|" << std::endl;
			std::cout << "fastCgiPass          .... |" << it->getLocations()[i].getFastCgiPass() << "|" << std::endl;
			std::cout << "isCGI                .... |" << it->getLocations()[i].isCGI() << "|" << std::endl;
			std::cout << "uploadEnable         .... |" << it->getLocations()[i].getUploadEnable() << "|" << std::endl;
			std::cout << "uploadStore          .... |" << it->getLocations()[i].getUploadStore() << "|" << std::endl;
			std::cout << "••••••••••••••••••" << std::endl;
		}
		std::cout << "\n››››››››››››››››››››››››››››››››››››››››››››››››" << std::endl;
	}
}

std::vector<HttpServer> &ConfigFileParser::getServers()
{
	return this->_servers;
}