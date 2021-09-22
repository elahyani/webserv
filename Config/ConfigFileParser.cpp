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

ConfigFileParser::ConfigFileParser() : data(""),
									   locationsNumber(0),
									   inLocation(false),
									   countauto(0),
									   isEnabled(0),
									   _isCGI(false),
									   _isLoc(false),
									   _isServ(false)

{
}

ConfigFileParser::~ConfigFileParser()
{
	//////////////////////////
	this->servers.clear();
	this->mapTmp.clear();
	this->location.clearAll();
	this->server.clearAll();
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
		start = end;
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

	if (!isdigit(buffer[i]) || buffer[i] == '0')
		throw std::invalid_argument("Exception: Client max body size must be a non-zeropositive number\t");
	while (buffer[i])
	{
		if (buffer.find("m") == std::string::npos || (!std::isdigit(buffer[i]) && buffer[i] != 'm'))
		{
			throw std::invalid_argument("Exception:\tUnrecognized or Missing unit!");
		}
		i++;
	}
}

void ConfigFileParser::checkMissingAttrs()
{
	if (this->inServer)
		throw std::invalid_argument("Exception:\tSyntax Error1");
	else if (!this->serversNumber)
		throw std::invalid_argument("Exception:\tServer Not Found");
	else if (this->server.getPorts().empty())
		throw std::invalid_argument("Exception:\tListening port not found!");
	else if (!this->server.getHost().size())
		this->server.setHost("ANY");
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

void ConfigFileParser::checkLocAttr(void)
{
	if (this->location.getLocationName().compare("/") == 0)
	{
		if (this->countauto == 0 && this->location.getIndexes().empty()
			&& this->location.getAllowedMethods().empty() && this->location.getRoot().empty())
			throw std::invalid_argument("Exception\t this location `" + this->location.getLocationName() + "` has no attribute");
		if (this->countauto == 0)
			throw std::invalid_argument("Exception\tMissing autoindex in this location `" + this->location.getLocationName() + "`");
		if (this->location.getIndexes().empty())
			throw std::invalid_argument("Exception\tMissing index in this location `" + this->location.getLocationName() + "`");
		if (this->location.getAllowedMethods().empty())
			throw std::invalid_argument("Exception\tMissing allow_methods in this location `" + this->location.getLocationName() + "`");
	}
	if (this->location.getLocationName().compare("/return") == 0)
	{
		if (this->location.getReturn().empty())
			throw std::invalid_argument("Exception\tMissing redirection code and path in this location `" + this->location.getLocationName() + "`");
	}
	if (this->location.getLocationName().compare("*.php") == 0)
	{
		if (this->location.getAllowedMethods().empty())
			throw std::invalid_argument("Exception\tMissing allow_methods in this location `" + this->location.getLocationName() + "`");
	}
	if (this->location.getLocationName().compare("*.py") == 0)
	{
		if (this->location.getAllowedMethods().empty())
			throw std::invalid_argument("Exception\tMissing allow_methods in this location `" + this->location.getLocationName() + "`");
	}
	if (this->location.getLocationName().compare("/upload") == 0)
	{
		if (this->location.getAllowedMethods().empty() && this->location.getUploadStore().empty()
			&& this->isEnabled == 0)
			throw std::invalid_argument("Exception\t this location `" + this->location.getLocationName() + "` has no attribute");
		if (this->location.getAllowedMethods().empty())
			throw std::invalid_argument("Exception\tMissing allow_methods in this location `" + this->location.getLocationName() + "`");
		if (this->location.getUploadStore().empty())
			throw std::invalid_argument("Exception\tMissing upload_store in this location `" + this->location.getLocationName() + "`");
		if (this->isEnabled == 0)
			throw std::invalid_argument("Exception\tMissing upload_enable in this location `" + this->location.getLocationName() + "`");
	}
	if (_isCGI && this->location.getFastCgiPass().empty())
	{
		std::cout << "---> " << _isCGI << std::endl;
		std::cout << "---> " << this->location.getLocationName() << std::endl;
		throw std::invalid_argument("Exception:\tFastCGI Not Found in : " + this->location.getLocationName());
	}

}

void ConfigFileParser::chekDupServerName(void)
{
	for (int i = 0; i < serversNumber; i++)
	{
		for (int j = 0; j < this->servers[i].getServerName().size(); j++)
		{
			if (i + 1 < serversNumber)
			{
				for (int k = 0; k < this->servers[i + 1].getServerName().size(); k++)
				{
						std::cout << "this |" << this->servers[i].getServerName()[j] << "| and that |";
						std::cout << this->servers[i + 1].getServerName()[k] << "|\n";
					if (this->servers[i].getServerName()[j].compare(this->servers[i+1].getServerName()[k]) == 0)
						throw std::invalid_argument("Exception\tServers cannot have the same name");
				}
			}
		}
	}
}

void ConfigFileParser::parseLocation(std::string _data)
{
	std::string buffer;
	std::istringstream str(_data.substr(_data.find("location: "), _data.find("}") + 1));
	// std::istringstream str(_data.substr(_data.find("location: ")));


	this->countauto = 0;
	this->isEnabled = 0;
	while (std::getline(str, buffer))
	{
		// std::cout << "..." << buffer << std::endl;
		if (buffer.find("#") != std::string::npos)
		{
			buffer = buffer.substr(0, buffer.find_first_of('#'));
			buffer = trimContent(buffer);
		}
		if (buffer.find("{") != std::string::npos)
		{
			if (buffer.compare("{") == 0)
			{
				// this->inLocation = !this->inLocation;
				this->inLocation = true;
			}
			else
				throw std::invalid_argument("Exception:\tOnly a { is allowed in this line");
		}
		else if (buffer.find("}") != std::string::npos)
		{

			if (buffer.compare("}") == 0)
			{
				checkLocAttr();
				// std::cout << ">>>>>>>>>emptyyyyyyyyyyyyy |" << this->location.getFastCgiPass() << ">>>>>>>>>>>>" << this->location.get_isCGI() << "|\n\n";
				// if (this->location.get_isCGI() && this->location.getFastCgiPass().empty())
				// if (_isCGI && this->location.getFastCgiPass().empty())
				// {
				// 	std::cout << "---> " << _isCGI << std::endl;
				// 	std::cout << "---> " << this->location.getLocationName() << std::endl;
				// 	throw std::invalid_argument("Exception:\tFastCGI Not Found in : " + this->location.getLocationName());
				// }
				_isCGI = false;
				this->locationsNumber++;
				this->inLocation = !this->inLocation;
				this->server.setLocation(this->location);
				this->location.clearAll();
				return;
			}
			else
				throw std::invalid_argument("Exception:\tOnly a } is allowed in this line");
		}
		if (this->inLocation)
		{
			if (buffer.find("autoindex") != std::string::npos)
			{
				this->countauto++;
				syntaxChecker(buffer, 1);
				buffer.pop_back();
				std::string tmp = trimContent(buffer.substr(buffer.find("=") + 1));
				if (this->countauto > 1)
					throw std::invalid_argument("Exception:\tDuplicated autoindex");
				if (!tmp.size())
					throw std::invalid_argument("Exception:\tAutoindex value not found");
				this->location.setAutoIndex(tmp);
			}
			else if (buffer.find("root") != std::string::npos)
			{
				if (!this->location.getRoot().size())
				{
					syntaxChecker(buffer, 1);
					this->split(trimContent(buffer.substr(buffer.find("=") + 1)), ' ');
					if (mapTmp.size() != 1)
						throw std::invalid_argument("Exception:\tWrong number of arguments");
					this->location.setRoot(trimContent(buffer.substr(buffer.find("=") + 1)));
					this->server.setRoot(this->location.getRoot());
					if (!this->location.getRoot().size())
						throw std::invalid_argument("Exception:\tRoot path not found");
				}
				else
					throw std::invalid_argument("Exception:\tDuplicated root");
			}
			else if (buffer.find("index") != std::string::npos)
			{
				if (this->location.getIndexes().empty())
				{
					syntaxChecker(buffer, 1);
					std::string indexes = trimContent(buffer.substr(buffer.find("=") + 1));
					indexes.pop_back();
					if (!indexes.size())
						throw std::invalid_argument("Exception:\tindex not found");
					this->split(indexes, ' ');
					for (size_t i = 0; i < mapTmp.size(); i++)
						this->location.setIndex(trimContent(mapTmp[i]));
				}
				else
					throw std::invalid_argument("Exception:\tDuplicated index");
			}
			else if (buffer.find("allow_methods") != std::string::npos)
			{
				if (this->location.getAllowedMethods().empty())
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
					if (this->mapTmp.size() > 3)
						throw std::invalid_argument("Exception:\tExceeded max methods length");
					for (size_t i = 0; i < mapTmp.size(); i++)
					{
						if (trimContent(mapTmp[i]).compare("GET") != 0 && trimContent(mapTmp[i]).compare("POST") != 0 && trimContent(mapTmp[i]).compare("DELETE") != 0)
							throw std::invalid_argument("Exception:\tMethod is not allowed");
						this->location.setAllowedMethods(trimContent(mapTmp[i]));
					}
				}
				else
					throw std::invalid_argument("Exception:\tDuplicated Allow_methods attr");
			}
			else if (buffer.find("return") != std::string::npos)
			{
				if (this->location.getReturn().empty())
				{
					syntaxChecker(buffer, 1);
					buffer.pop_back();
					this->split(trimContent(buffer.substr(buffer.find("=") + 1)), ' ');
					if (mapTmp.size() != 2)
						throw std::invalid_argument("Exception:\tWrong number of arguments");
					this->location.setReturn(std::stoi(mapTmp[0]), trimContent(mapTmp[1]));
				}
				else
					throw std::invalid_argument("Exception:\tDuplicated return");
			}
			else if (buffer.find("fastcgi_pass") != std::string::npos)
			{
				if (!this->location.getFastCgiPass().size())
				{
					syntaxChecker(buffer, 1);
					this->split(trimContent(buffer.substr(buffer.find("=") + 1)), ' ');
					if (mapTmp.size() != 1)
						throw std::invalid_argument("Exception:\tWrong number of arguments");
					this->location.setFastCgiPass(trimContent(buffer.substr(buffer.find("=") + 1)));
					if (!this->location.getFastCgiPass().size())
						throw std::invalid_argument("Exception:\tfastcgi_pass path not found");
				}
				else
					throw std::invalid_argument("Exception:\tDuplicated fastcgi_pass");
			}
			else if (buffer.find("upload_enable") != std::string::npos)
			{
				this->isEnabled++;
				syntaxChecker(buffer, 1);
				buffer.pop_back();
				std::string tmp = trimContent(buffer.substr(buffer.find("=") + 1));
				if (isEnabled > 1)
				{
					std::cout << isEnabled << std::endl;
					throw std::invalid_argument("Exception:\tDuplicated upload_enable");
				}
				if (!tmp.size())
					throw std::invalid_argument("Exception:\tUpload_enable value not found");
				this->location.setUploadEnable(tmp);
			}
			else if (buffer.find("upload_store") != std::string::npos)
			{
				if (!this->location.getUploadStore().size())
				{
					syntaxChecker(buffer, 1);
					std::string upstore = trimContent(buffer.substr(buffer.find("=") + 1));
					upstore.pop_back();
					if (!upstore.size())
						throw std::invalid_argument("Execption:\tupload_store path not found");
					this->split(trimContent(buffer.substr(buffer.find("=") + 1)), ' ');
					if (mapTmp.size() != 1)
						throw std::invalid_argument("Exception:\tWrong number of arguments");
					this->location.setUploadStore(trimContent(buffer.substr(buffer.find("=") + 1)));
				}
				else
					throw std::invalid_argument("Exception:\tDuplicated upload_store");
			}
		}
	}
	if (inLocation)
		throw std::invalid_argument("Exception:\tSyntax Error 5");
}

void ConfigFileParser::checkLocationName(std::string &buffer)
{
	this->split(this->location.getLocationName(), ' ');
	if (mapTmp.size() != 1)
		throw std::invalid_argument("Exception:\tWrong number of arguments");
	if (buffer.find("/") != std::string::npos)
	{
		this->split(buffer, '/');
		if (mapTmp[1].compare("\0") != 0)
		{
			int i = 0;
			while (mapTmp[1][i])
			{
				if (!isalpha(mapTmp[1][i]))
					throw std::invalid_argument("Exception\tLocation name is invalid");
				i++;
			}
		}
	}
	if (buffer.find(".") != std::string::npos)
	{
		this->split(buffer, '.');
		if (mapTmp.size() != 2 || std::count(buffer.begin(), buffer.end(), '.') > 1)
			throw std::invalid_argument("Exception:\tLocation name is invalid");
		if (mapTmp[1] != "py" && mapTmp[1] != "php")
			throw std::invalid_argument("Exception:\t CGI extention is not supported");
	}
}

void ConfigFileParser::parseConfigFile(int ac, char **av)
{
	int isDef = -1;
	checkFile(ac, av);
	std::ifstream file(this->filename);

	if (!file.is_open())
		throw std::invalid_argument("Exception:\tFile Not Found!");
	std::string buffer;

	while (std::getline(file, buffer))
	{
		this->data.append(trimContent(buffer));
		this->data.append("\n");
	}
	if (data.find("server") == std::string::npos || data.find("[") == std::string::npos)
		throw std::invalid_argument("Exception:\tNO SERVER CONFIGURATION FOUND!");
	std::istringstream str(this->data);
	while (std::getline(str, buffer))
	{
		// trim buffer
		buffer = trimContent(buffer);
		if (buffer.compare("server") == 0)
			_isServ = true;
		if (buffer.find("}") != std::string::npos && _isLoc)
		{
			_isLoc = false;
			continue ;
		}
		if (buffer.find("#") != std::string::npos)
		{
			buffer = buffer.substr(0, buffer.find_first_of('#'));
			buffer = trimContent(buffer);
		}
		// buffer.erase(std::remove_if(buffer.begin(), buffer.end(), ::isspace), buffer.end());
		// std::cout << ">>>>>>>>>>>> " << buffer << std::endl;
		if (buffer.find("[") != std::string::npos)
		{
			this->split(trimContent(buffer), '=');
			if (buffer.compare("[") == 0)
			{
				if (!_isServ)
					throw std::invalid_argument("Exception:\tOpening brackets without server REALLY! :/");
				if (this->inServer)
					throw std::invalid_argument("Exception:\tCannot define server inside server");
				
				this->inServer = !this->inServer;
				isDef = -1;
			}
			else if (mapTmp[0] != "allow_methods")
				throw std::invalid_argument("Exception:\tOnly a [ is allowed in this line");
		}
		else if (buffer.find("]") != std::string::npos)
		{
			if (buffer.compare("]") == 0)
			{
				this->serversNumber++;
				this->inServer = !this->inServer;
				this->checkMissingAttrs();
				this->servers.push_back(this->server);
				this->server.clearAll();
				_isServ = false;
				this->data = this->data.substr(data.find("]\n"));
				if (isDef == -1)
					throw std::invalid_argument("Exception:\tmakainx");
			}
			else
				throw std::invalid_argument("Exception:\tOnly a ] is allowed in this line");
		}
		if (this->inServer)
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
					syntaxChecker(buffer, 0);
					buffer.pop_back();
					std::string host = buffer.substr(buffer.find(":") + 1);
					this->server.setHost(trimContent(host));
					checkHost(this->server.getHost());
					if (!this->server.getHost().size())
						this->server.setHost("ANY");
				}
				else
					throw std::invalid_argument("Exception:\tDuplicated " + trimContent(buffer.substr(0, buffer.find(":"))));
			}
			else if (buffer.find("server_name") != std::string::npos)
			{
				if (!this->server.getServerName().size())
				{
					syntaxChecker(buffer, 0);
					buffer.pop_back();
					this->split(trimContent(buffer.substr(buffer.find(":") + 1)), ' ');
					for (int i = 0; i < mapTmp.size(); i++)
						this->server.setServerName(mapTmp[i]);
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
					syntaxChecker(buffer, 0);
					buffer.pop_back();
					this->checkUnit(this->trimContent(buffer.substr(buffer.find(":") + 1)));
					this->server.setClientMaxBodySize(std::stoi(buffer.substr(buffer.find(":") + 1)));
				}
				else
					throw std::invalid_argument("Exception:\tDuplicated " + trimContent(buffer.substr(0, buffer.find(":"))));
			}
			else if (buffer.find("root") != std::string::npos)
			{
				if (buffer.find("root:") != std::string::npos)
				{
					isDef = 1;
					syntaxChecker(buffer, 0);
					if (!this->server.getRoot().size())
					{
						this->server.setRoot(trimContent(buffer.substr(buffer.find(":") + 1)));
						std::string s = this->server.getRoot();
						s = trimContent(s);
						this->split(s, ' ');
						if (mapTmp.size() != 1)
							throw std::invalid_argument("Exception:\tWrong number of arguments!");
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
				syntaxChecker(buffer, 0);
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
			else if (buffer.find("location") != std::string::npos)
			{
				if (buffer.back() == ';')
					throw std::invalid_argument("Exception:\tLocation name must not end with ';'");
				if (buffer.find(".php") != std::string::npos || buffer.find(".py") != std::string::npos)
				{
					_isCGI = true;
				}
				this->location.setLocationName(trimContent(buffer.substr(buffer.find(":") + 1)));
				checkLocationName(buffer);
				_isLoc = true;
				// this->split(this->location.getLocationName(), ' ');
				// if (mapTmp.size() != 1)
				// 	throw std::invalid_argument("Exception:\tWrong number of arguments");
				this->parseLocation(data.substr(data.find("location: " + this->location.getLocationName())));
				// std::cout << "HANAAA >>>>>>>>>>>>>>|" << buffer << "|\n";
				// buffer = data.substr(data.find("}") + 1);
				// std::cout << "HANAAA >>>>>>>>>>>>>>|" << this->location.getLocationName() << "|\n";
				// std::cout << "HANAAA >>>>>>>>>>>>>>|" << buffer << "|\n";
			}
			else if (buffer.find("{") == std::string::npos && buffer.find("}") == std::string::npos &&
					 buffer.find("[") == std::string::npos && buffer.find("]") == std::string::npos &&
					 buffer.find("autoindex") == std::string::npos && buffer.find("index") == std::string::npos &&
					 buffer.find("return") == std::string::npos && buffer.find("allow_methods") == std::string::npos &&
					 buffer.find("fastcgi_pass") == std::string::npos && buffer.find("upload_enable") == std::string::npos &&
					 buffer.find("upload_store") == std::string::npos &&
					 buffer != "\n" && buffer != "\0")
			{
				std::cout << ">>>>>>>>>>>>>>|" << buffer << "|\n";
				throw std::invalid_argument("Exception:\tAttribute not recognized");
			}
			if ((buffer.find("{") != std::string::npos || buffer.find("}") != std::string::npos) && !_isLoc)
				throw std::invalid_argument("Exception:\tMisplaced curly braces");
		}
		// else if (!serversNumber && buffer.find("server") == std::string::npos)
		// 	throw std::invalid_argument("Exception:\tEmpty Configuration File");
		// std::cout << ">>>>>>>>>>>>>>SERVER NUM|" << serversNumber << " " << buffer<< "|" << std::endl;

		std::cout << "|" << buffer << "|" << std::endl;
	}
	if (inServer)
		throw std::invalid_argument("Exception:\tSyntax Error 5");
	chekDupServerName();
	std::cout << ">>>>>>>>>>>>>>|" << isDef << "|\n";
}

void ConfigFileParser::printContentData()
{
	std::cout << "-------------------[SERVER]-------------------" << std::endl;
	for (std::vector<HttpServer>::iterator it = this->servers.begin(); it != this->servers.end(); ++it)
	{
		std::cout << "servers number       .... " << serversNumber << std::endl;
		for (int i = 0; i < it->getPorts().size(); i++)
			std::cout << "listen on            .... |" << it->getPorts()[i] << "|" << std::endl;
		std::cout << "Host                 .... |" << it->getHost() << "|" << std::endl;
		for (int i = 0; i < it->getServerName().size(); i++)
			std::cout << "Server name          .... |" << it->getServerName()[i] << "|" << std::endl;
		std::cout << "Client Max Body Size .... |" << it->getClientMaxBodySize() << "|" << std::endl;
		std::cout << "Root                 .... |" << it->getRoot() << "|" << std::endl;
		for (std::map<int, std::string>::iterator err = it->getErrorsPages().begin(); err != it->getErrorsPages().end(); ++err)
			std::cout << "Error Page           .... |" << err->first << "| | |" << err->second << "|" << std::endl;
		std::cout << "••••••••••••••••••" << std::endl;
		std::cout << "\n-------------------[LOCATIONS]-------------------" << std::endl;
		for (size_t i = 0; i < it->getLoactions().size(); i++)
		{
			std::cout << "LocationName         .... |" << it->getLoactions()[i].getLocationName() << "|" << std::endl;
			std::cout << "autoindex            .... |" << it->getLoactions()[i].getAutoIndex() << "|" << std::endl;
			std::cout << "root                 .... |" << it->getLoactions()[i].getRoot() << "|" << std::endl;
			for (int j = 0; j < it->getLoactions()[i].getIndexes().size(); j++)
				std::cout << "index                .... |" << it->getLoactions()[i].getIndexes()[j] << "|" << std::endl;
			for (int j = 0; j < it->getLoactions()[i].getAllowedMethods().size(); j++)
				std::cout << "allow_methods        .... |" << it->getLoactions()[i].getAllowedMethods()[j] << "|" << std::endl;
			for (std::map<int, std::string>::iterator ret = it->getLoactions()[i].getReturn().begin(); ret != it->getLoactions()[i].getReturn().end(); ++ret)
				std::cout << "return               .... |" << ret->first << "| |" << ret->second << "|" << std::endl;
			std::cout << "fastCgiPass          .... |" << it->getLoactions()[i].getFastCgiPass() << "|" << std::endl;
			std::cout << "uploadEnable         .... |" << it->getLoactions()[i].getUploadEnable() << "|" << std::endl;
			std::cout << "uploadStore          .... |" << it->getLoactions()[i].getUploadStore() << "|" << std::endl;
			std::cout << "••••••••••••••••••" << std::endl;
		}
		std::cout << "\n››››››››››››››››››››››››››››››››››››››››››››››››" << std::endl;
	}
}