/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpServer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ichejra <ichejra@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/09 15:24:18 by ichejra           #+#    #+#             */
/*   Updated: 2021/09/11 18:15:41 by ichejra          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpServer.hpp"

HttpServer::HttpServer() : host(""), clientMaxBodySize(), root(""), inServer(false), serversNumber(0), filename("")
{
}

HttpServer::~HttpServer()
{
}

std::string HttpServer::trimContent(std::string str)
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

void HttpServer::semiColonChecker(std::string &buffer)
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

void HttpServer::split(std::string line, char splitter)
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

void HttpServer::checkFile(int ac, char **av)
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

void HttpServer::checkUnit(std::string buffer)
{
	int i = 0;

	while (buffer[i])
	{
		if (buffer.find("m") == std::string::npos || (!std::isdigit(buffer[i]) && buffer[i] != 'm'))
			throw std::invalid_argument("Exception:\tUnrecognized or Missing unit!");
		i++;
	}
}

void HttpServer::parseConfigFile(int ac, char **av)
{
	checkFile(ac, av);
	std::ifstream file(this->filename);
	int i = 0;

	if (!file.is_open())
		throw std::invalid_argument("Exception:\tFile Not Found!");
	std::cout << "filename: " << this->filename << std::endl;
	std::string buffer;

	while (std::getline(file, buffer))
	{
		// trim buffer
		std::cout << ">>>>>>>>>>>> " << trimContent(buffer) << std::endl;

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

				if (this->ports.size())
				{
					for (int i = 0; i < ports.size(); i++)
					{
						if (ports[i] == port)
							throw std::invalid_argument("Exception:\tDuplicated port!");
					}
				}
				this->ports.push_back(port);
			}
			else if (buffer.find("host") != std::string::npos)
			{
				if (!this->host.size())
				{
					semiColonChecker(buffer);
					this->host = (buffer.substr(buffer.find(":") + 1));
					this->host.pop_back();
					if (!this->host.size())
						throw std::invalid_argument("Exception:\tHost not found");
				}
				else
					throw std::invalid_argument("Exception:\tDuplicated " + trimContent(buffer.substr(0, buffer.find(":"))));
			}
			else if (buffer.find("server_name") != std::string::npos)
			{
				if (!this->serverName.size())
				{
					semiColonChecker(buffer);
					buffer.pop_back();
					std::string s = trimContent(buffer.substr(buffer.find(":") + 1));
					this->split(s, ' ');
					while (i < mapTmp.size())
					{
						this->serverName.push_back(mapTmp[i]);
						i++;
					}
					if (!serverName.size())
						throw std::invalid_argument("Exception:\tServer name not found");
				}
				else
					throw std::invalid_argument("Exception:\tDuplicated " + trimContent(buffer.substr(0, buffer.find(":"))));
			}
			else if (buffer.find("client_max_body_size") != std::string::npos)
			{
				if (!this->clientMaxBodySize)
				{
					semiColonChecker(buffer);
					buffer.pop_back();
					this->checkUnit(this->trimContent(buffer.substr(buffer.find(":") + 1)));
					this->clientMaxBodySize = std::stoi(buffer.substr(buffer.find(":") + 1));
				}
				else
					throw std::invalid_argument("Exception:\tDuplicated " + trimContent(buffer.substr(0, buffer.find(":"))));
			}
			else if (buffer.find("root") != std::string::npos)
			{
				semiColonChecker(buffer);
				if (buffer.find("root:") != std::string::npos)
				{
					if (!this->root.size())
					{
						this->root = buffer.substr(buffer.find(":") + 1);
						this->root.pop_back();
						//////////////////
						std::string s = this->root.substr(this->root.find(":") + 1);
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
						if (!root.size())
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
				this->errorCode = std::stoi(buffer.substr(buffer.find(":") + 1, buffer.find(" /")));
				if (buffer.find(" /") != std::string::npos)
					this->errorPagePath = buffer.substr(buffer.find(" /") + 1);
				else
					throw std::invalid_argument("Exception:\tInvalid error page!");
				if (errorPagePath.find(".html") != std::string::npos)
				{
					if (errorPagePath.substr(errorPagePath.find(".html")).compare(".html") != 0)
						throw std::invalid_argument("Exception:\tInvalid extention!");
				}
				else
					throw std::invalid_argument("Exception:\tInvalid extention!");

				// std::cout << "error page path ---> " << errorPagePath << std::endl;
				this->errorsPages.insert(std::pair<int, std::string>(this->errorCode, this->errorPagePath));
			}
		}
	}
	if (this->inServer)
		throw std::invalid_argument("Syntax Error");
	if (this->ports.empty())
		throw std::invalid_argument("Exception:\tListening port not found!");
}

void HttpServer::printContentData()
{
	std::cout << "----------------------------------------" << std::endl;
	std::cout << "servers number       >>>> " << serversNumber << std::endl;
	for (int i = 0; i < this->ports.size(); i++)
		std::cout << "listen on            >>>> |" << ports[i] << "|" << std::endl;
	std::cout << "Host                 >>>> |" << this->host << "|" << std::endl;
	for (int i = 0; i < serverName.size(); i++)
		std::cout << "Server name          >>>> |" << this->serverName[i] << "|" << std::endl;
	std::cout << "Client Max Body Size >>>> |" << this->clientMaxBodySize << "|" << std::endl;
	std::cout << "Root                 >>>> |" << this->root << "|" << std::endl;
	for (std::map<int, std::string>::iterator it = this->errorsPages.begin(); it != this->errorsPages.end(); ++it)
		std::cout << "Error Page           >>>> |" << it->first << "| | |" << it->second << "|" << std::endl;
	std::cout << "----------------------------------------" << std::endl;
}