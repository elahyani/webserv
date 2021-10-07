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
									   _serversNumber(0),
									   _locationsNumber(0),
									   _inServer(0),
									   _inLocation(false),
									   _filename(""),
									   _countauto(0),
									   _isEnabled(0),
									   _isLoc(false),
									   _isServ(false)

{
}

ConfigFileParser::~ConfigFileParser()
{
	this->_servers.clear();
	this->_mapTmp.clear();
	this->_location.clearAll();
	this->_server.clearAll();
}

ConfigFileParser::ConfigFileParser(ConfigFileParser const &src)
{
	*this = src;
}

ConfigFileParser &ConfigFileParser::operator=(ConfigFileParser const &src)
{
	if (this != &src)
	{
		this->_data = src._data;
		this->_serversNumber = src._serversNumber;
		this->_locationsNumber = src._locationsNumber;
		this->_inServer = src._inServer;
		this->_inLocation = src._inLocation;
		this->_filename = src._filename;
		this->_servers.assign(src._servers.begin(), src._servers.end());
		this->_server = src._server;
		this->_location = src._location;
		this->_mapTmp = src._mapTmp;
		this->_countauto = src._countauto;
		this->_isEnabled = src._isEnabled;
		this->_isLoc = src._isLoc;
		this->_isServ = src._isServ;
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
		start = end;
		end = line.find(splitter, start);
		i++;
	}
	this->_mapTmp.insert(std::pair<int, std::string>(i, line.substr(start, end - start)));
}

void ConfigFileParser::checkFile(int ac, char **av)
{
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

	if (!isdigit(buffer[i]) || buffer[i] == '0')
		throw std::invalid_argument("Exception: Client max body size must be a non-zeropositive number\t");
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
		throw std::invalid_argument("Exception:\tSyntax Error1");
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
			throw std::invalid_argument("Exception:\tinvalid format of Host");
	}
	if (dots != 3)
		throw std::invalid_argument("Exception:\tinvalid format of Host");
}

void ConfigFileParser::checkLocAttr(void)
{
	if (this->_countauto == 0 && this->_location.getIndex().empty() && this->_location.getAllowedMethods().empty() && this->_location.getRoot().empty() && this->_location.getReturn().empty() &&
		this->_location.getFastCgiPass().empty() && this->_location.getUploadStore().empty() && this->_isEnabled == 0)
		throw std::invalid_argument("Exception\tthis location `" + this->_location.getLocationName() + "` has no attribute");
	else if (this->_location.getUploadEnable() && this->_location.getUploadStore().empty())
		throw std::invalid_argument("Exception\tUpload is enabled yet upload store not found in `" + this->_location.getLocationName() + "`");
	else if (_location.isCGI() && this->_location.getFastCgiPass().empty())
		throw std::invalid_argument("Exception:\tFastCGI Not Found in : " + this->_location.getLocationName());
}

void ConfigFileParser::chekDupServerName()
{
	for (size_t i = 0; i < _serversNumber; i++)
	{
		for (size_t j = 0; j < this->_servers[i].getServerName().size(); j++)
		{
			if (i + 1 < _serversNumber)
			{
				for (size_t k = 0; k < this->_servers[i + 1].getServerName().size(); k++)
				{
					if (this->_servers[i].getServerName()[j].compare(this->_servers[i + 1].getServerName()[k]) == 0)
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

	this->_countauto = 0;
	this->_isEnabled = 0;
	while (std::getline(str, buffer))
	{
		if (buffer.find("#") != std::string::npos)
		{
			buffer = buffer.substr(0, buffer.find_first_of('#'));
			buffer = trimContent(buffer);
		}
		if (buffer.find("{") != std::string::npos)
		{
			if (buffer.compare("{") == 0)
				this->_inLocation = true;
			else
				throw std::invalid_argument("Exception:\tOnly a { is allowed in this line");
		}
		else if (buffer.find("}") != std::string::npos)
		{
			if (buffer.compare("}") == 0)
			{
				checkLocAttr();
				_location.setIsCGI(false);
				this->_locationsNumber++;
				this->_inLocation = !this->_inLocation;
				this->_server.setLocation(this->_location);
				this->_location.clearAll();
				return;
			}
			else
				throw std::invalid_argument("Exception:\tOnly a } is allowed in this line");
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
					throw std::invalid_argument("Exception:\tDuplicated Allow_methods attribute in `" + this->_location.getLocationName() + "`");
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
					if (!this->_location.getFastCgiPass().size())
						throw std::invalid_argument("Exception:\tfastcgi_pass path not found");
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
					throw std::invalid_argument("Exception:\tDuplicated upload_enable");
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
	if (_inLocation)
		throw std::invalid_argument("Exception:\tSyntax Error 5");
}

void ConfigFileParser::checkLocationName(std::string &buffer)
{
	this->split(this->_location.getLocationName(), ' ');
	if (_mapTmp.size() != 1)
		throw std::invalid_argument("Exception:\tWrong number of arguments");
	if (buffer.find("/") != std::string::npos)
	{
		this->split(buffer, '/');
		if (_mapTmp[1].compare("\0") != 0)
		{
			int i = 0;
			while (_mapTmp[1][i])
			{
				if (!isalpha(_mapTmp[1][i]) && _mapTmp[1][i] != '_')
					throw std::invalid_argument("Exception\tLocation name is invalid");
				i++;
			}
		}
	}
	if (buffer.find(".") != std::string::npos)
	{
		this->split(buffer, '.');
		if (_mapTmp.size() != 2 || std::count(buffer.begin(), buffer.end(), '.') > 1)
			throw std::invalid_argument("Exception:\tLocation name is invalid");
		if (_mapTmp[1] != "py" && _mapTmp[1] != "php")
			throw std::invalid_argument("Exception:\t CGI extention is not supported");
	}
}

void ConfigFileParser::parseConfigFile(int ac, char **av)
{
	int isDef = -1;
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
	if (_data.find("server") == std::string::npos || _data.find("[") == std::string::npos)
		throw std::invalid_argument("Exception:\tNO SERVER CONFIGURATION FOUND!");
	std::istringstream str(this->_data);
	while (std::getline(str, buffer))
	{
		buffer = trimContent(buffer);
		if (buffer.compare("server") == 0)
			_isServ = true;
		if (buffer.find("}") != std::string::npos && _isLoc)
		{
			_isLoc = false;
			continue;
		}
		if (buffer.find("#") != std::string::npos)
		{
			buffer = buffer.substr(0, buffer.find_first_of('#'));
			buffer = trimContent(buffer);
		}
		if (buffer.find("[") != std::string::npos)
		{
			this->split(trimContent(buffer), '=');
			if (buffer.compare("[") == 0)
			{
				if (!_isServ)
					throw std::invalid_argument("Exception:\tOpening brackets without server REALLY! :/");
				else if (this->_inServer)
					throw std::invalid_argument("Exception:\tCannot define server inside server");

				this->_inServer = !this->_inServer;
				isDef = -1;
			}
			else if (_mapTmp[0] != "allow_methods")
				throw std::invalid_argument("Exception:\tOnly a [ is allowed in this line");
		}
		else if (buffer.find("]") != std::string::npos)
		{
			if (buffer.compare("]") == 0)
			{
				this->_serversNumber++;
				this->_inServer = !this->_inServer;
				this->checkMissingAttrs();
				this->_servers.push_back(this->_server);
				this->_server.clearAll();
				_isServ = false;
				this->_data = this->_data.substr(_data.find("]\n"));
				if (isDef == -1)
					throw std::invalid_argument("Exception:\tDefault root not found!");
			}
			else
				throw std::invalid_argument("Exception:\tOnly a ] is allowed in this line");
		}
		if (this->_inServer)
		{
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
					isDef = 1;
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
						else if (!this->_server.getRoot().size())
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
				this->_server.setErrorsPages(this->_server.getErrorCode(), this->_server.getErrorPagePath());
			}
			else if (buffer.find("location") != std::string::npos)
			{
				if (buffer.back() == ';')
					throw std::invalid_argument("Exception:\tLocation name must not end with ';'");
				if (buffer.find(".php") != std::string::npos || buffer.find(".py") != std::string::npos)
					_location.setIsCGI(true);
				this->_location.setLocationName(trimContent(buffer.substr(buffer.find(":") + 1)));
				checkLocationName(buffer);
				_isLoc = true;
				this->parseLocation(_data.substr(_data.find("location: " + this->_location.getLocationName() + "\n")));
			}
			else if (buffer.find("{") == std::string::npos && buffer.find("}") == std::string::npos &&
					 buffer.find("[") == std::string::npos && buffer.find("]") == std::string::npos &&
					 buffer.find("autoindex") == std::string::npos && buffer.find("index") == std::string::npos &&
					 buffer.find("return") == std::string::npos && buffer.find("allow_methods") == std::string::npos &&
					 buffer.find("fastcgi_pass") == std::string::npos && buffer.find("upload_enable") == std::string::npos &&
					 buffer.find("upload_store") == std::string::npos &&
					 buffer != "\n" && buffer != "\0")
			{
				throw std::invalid_argument("Exception:\tAttribute not recognized `" + buffer + "`");
			}
			if ((buffer.find("{") != std::string::npos || buffer.find("}") != std::string::npos) && !_isLoc)
				throw std::invalid_argument("Exception:\tMisplaced curly braces");
		}
		else if (buffer.find("[") == std::string::npos &&
				 buffer.find("]") == std::string::npos &&
				 buffer.find("server") == std::string::npos &&
				 buffer != "\n" && buffer != "\0")
		{
			throw std::invalid_argument("Exception:\tInvaid Identifier");
		}
	}
	if (_inServer)
		throw std::invalid_argument("Exception:\tSyntax Error 5");
	chekDupServerName();
}

std::vector<HttpServer> &ConfigFileParser::getServers()
{
	return this->_servers;
}