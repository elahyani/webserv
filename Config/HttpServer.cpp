/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpServer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ichejra <ichejra@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/09 15:24:18 by ichejra           #+#    #+#             */
/*   Updated: 2021/09/10 18:13:01 by ichejra          ###   ########.fr       */
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

	if (!file.is_open())
		throw std::invalid_argument("Exception:\tFile Not Found!");
	std::cout << "filename: " << this->filename << std::endl;
	std::string buffer;

	while (std::getline(file, buffer))
	{
		// trim buffer
		std::cout << ">>>>>>>>>>>> " << trimContent(buffer) << std::endl;
		///////////////////////////
		//check comments
		// if (buffer[0] == '#')
		// {
		// 	std::cout << "am here\n";
		// 	continue;
		// }
		/////////////////////////////
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
			else if (!this->host.size() && buffer.find("host") != std::string::npos)
			{
				semiColonChecker(buffer);
				this->host = (buffer.substr(buffer.find(":") + 1));
				this->host.pop_back();
			}
			else if (!this->serverName.size() && buffer.find("server_name") != std::string::npos)
			{
				semiColonChecker(buffer);
				this->serverName = buffer.substr(buffer.find(":") + 1);
				this->serverName.pop_back();
			}
			else if (!this->clientMaxBodySize && buffer.find("client_max_body_size") != std::string::npos)
			{
				semiColonChecker(buffer);
				buffer.pop_back();
				this->checkUnit(this->trimContent(buffer.substr(buffer.find(":") + 1)));
				this->clientMaxBodySize = std::stoi(buffer.substr(buffer.find(":") + 1));
			}
			else if (!this->root.size() && buffer.find("root") != std::string::npos)
			{
				semiColonChecker(buffer);
				this->root = buffer.substr(buffer.find(":") + 1);
				this->root.pop_back();
			}
			else if (buffer.find("error_page") != std::string::npos)
			{
				semiColonChecker(buffer);
				this->errorCode = std::stoi(buffer.substr(buffer.find(":") + 1, buffer.find("/")));
				this->errorPagePath = buffer.substr(buffer.find("/"));
				this->errorPagePath.pop_back();
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
		std::cout << "listen on            >>>>" << ports[i] << std::endl;
	std::cout << "Host                 >>>> |" << this->host << "|" << std::endl;
	std::cout << "Server name          >>>> |" << this->serverName << "|" << std::endl;
	std::cout << "Client Max Body Size >>>> |" << this->clientMaxBodySize << "|" << std::endl;
	std::cout << "Root                 >>>> |" << this->root << "|" << std::endl;
	for (std::map<int, std::string>::iterator it = this->errorsPages.begin(); it != this->errorsPages.end(); ++it)
		std::cout << "Error Page           >>>> |" << it->first << "| | |" << it->second << "|" << std::endl;
	std::cout << "----------------------------------------" << std::endl;
}