/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asaadi <asaadi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/01 16:54:02 by elahyani          #+#    #+#             */
/*   Updated: 2021/09/13 12:26:17 by asaadi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request() {}

// bool checkRequest(std::string &req)
// {
// 	std::string data;
// 	size_t i;

// 	i = req.find("\r\n\r\n");
// 	if (i == std::string::npos)
// 	{
// 		return false;
// 	}
// 	if (req.find("Content-Length") != std::string::npos)
// 	{

// 		data = req.substr(i + 4);
// 		if (data.find("\r\n\r\n") == std::string::npos)
// 		{
// 			return false;
// 		}
// 	}
// 	return true;
// }

// Request::Request(int &_newSockFd)
// {
// 	int valRead;
// 	char _buffReq[1024] = {0};

// 	while (true)
// 	{
// 		std::memset(_buffReq, 0, sizeof(_buffReq));
// 		valRead = recv(_newSockFd, _buffReq, sizeof(_buffReq), 0);
// 		// if (valRead == -1)
// 		// 	throw std::runtime_error("Unable to receive the request from client.");
// 		// else if (valRead == 0)
// 		//     break;
// 		// else
// 		// {
// 		std::cout << " valRead == " << std::to_string(valRead) << std::endl;
// 		_buffReq[valRead] = '\0';
// 		content.append(_buffReq);
// 		// }
// 		if (checkRequest(content))
// 			break;
// 	}
// 			std::cout << " valRead == " << std::to_string(valRead) << std::endl;
// 	// this->content = _buffReq;
// 	this->methods.push_back("GET");
// 	this->methods.push_back("POST");
// 	this->methods.push_back("DELETE");
// }

Request::Request(const std::string &buffer) : content("")
{
	// content.append(_buffReq);
	this->content.append(buffer);
	std::cout << "**************************************"  << std::endl;
	std::cout << content  << std::endl;
	std::cout << "**************************************"  << std::endl;
	this->methods.push_back("GET");
	this->methods.push_back("POST");
	this->methods.push_back("DELETE");
}

Request::Request(const Request &src)
{
	(void)src;
}

Request &Request::operator=(const Request &rhs)
{
	(void)rhs;
	return *this;
}

Request::~Request() {}

void Request::parseRequest()
{
	std::string tmp;
	std::istringstream s(content);
	try
	{
		while (std::getline(s, tmp))
		{
			// std::cout << "-> " << tmp << std::endl;
			if (!this->method.size() && !this->pVersion.size())
			{

				this->split(tmp, " ");
				for (size_t i = 0; i < mapTmp.size(); i++)
					std::cout << mapTmp[i] << std::endl;
				if (this->mapTmp.size() == 3)
				{
					if (mapTmp[0] != methods[0] && mapTmp[0] != methods[1] && mapTmp[0] != methods[2])
					{
						std::cout << "---------<<< " << mapTmp[0] << std::endl;
						throw std::invalid_argument("Bad Request: Method Not Allowed");
					}
					if (mapTmp[1].at(0) != '/')
						throw std::invalid_argument("Bad Request: Absolute path required!");
					if (mapTmp[2].find("HTTP/1.1") != std::string::npos)
					{
						this->mapTmp[2] = this->mapTmp[2].substr(0, mapTmp[2].length() - 1);
						if (mapTmp[2] != "HTTP/1.1")
							throw std::invalid_argument("Bad Request: Wrrong HTTP version!");
					}
					this->method = this->mapTmp[0];
					this->urlPath = this->mapTmp[1];
					this->pVersion = this->mapTmp[2];
					if (this->urlPath.find("?") != std::string::npos)
					{
						this->urlQuery = this->urlPath.substr(this->urlPath.find("?") + 1);
						this->urlPath = this->urlPath.substr(0, this->urlPath.find("?"));
					}
				}
				else
					throw std::invalid_argument("Bad Request: Too much or too few arguments!");
			}
			else if (!headers["Host"].size() && tmp.find("Host") != std::string::npos)
				headers["Host"] = tmp.substr(tmp.find(": ") + 2);
			else if (!headers["Connection"].size() && tmp.find("Connection") != std::string::npos)
				headers["Connection"] = tmp.substr(tmp.find(": ") + 2);
			else if (!headers["Content-Type"].size() && tmp.find("Content-Type") != std::string::npos)
				headers["Content-Type"] = tmp.substr(tmp.find(": ") + 2);
			else if (!headers["Content-Length"].size() && tmp.find("Content-Length") != std::string::npos)
				headers["Content-Length"] = tmp.substr(tmp.find(": ") + 2);
			else if (!headers["Transfer-Encoding"].size() && tmp.find("Transfer-Encoding") != std::string::npos)
				headers["Transfer-Encoding"] = tmp.substr(tmp.find(": ") + 2);
			// std::cout << "len:" << tmp.find("Content-Length") << std::endl;
		}
		// std::cout << "OUT tmp: |" << tmp << "|" << std::endl;
		if (tmp.find("\r\n\r\n") != std::string::npos)
		{
			std::cout << "Body is here" << std::endl;
			parseBody();
		}
		// exit(1);
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
		exit(1);
	}
	this->startLine["method"] = method;
	this->startLine["url"] = urlPath;
	this->startLine["query"] = urlQuery;
	this->startLine["pVersion"] = pVersion;
}

void Request::parseBody()
{
	std::string tmp;
	std::istringstream s(content.substr(content.find("\r\n\r\n") + 4));

	while (std::getline(s, tmp))
	{
		std::cout << "-> " << tmp << std::endl;
	}
}

void Request::printRequest()
{
	std::cout << "+++++++++++++++++++++++++++++++++++++" << std::endl;
	std::cout << "Method            -> " << this->startLine["method"] << std::endl;
	std::cout << "Url               -> " << this->startLine["url"] << std::endl;
	std::cout << "Protocol Version  -> " << this->startLine["pVersion"] << std::endl;
	std::cout << "Host              -> " << this->headers["Host"] << std::endl;
	std::cout << "Connection        -> " << this->headers["Connection"] << std::endl;
	std::cout << "Content Type      -> " << this->headers["Content-Type"] << std::endl;
	std::cout << "Content Length    -> " << this->headers["Content-Length"] << std::endl;
	std::cout << "Transfer Encoding -> " << this->headers["Transfer-Encoding"] << std::endl;
	std::cout << "Boundry           -> " << boundry << std::endl;
	std::cout << "+++++++++++++++++++++++++++++++++++++" << std::endl;
}

void Request::split(std::string line, std::string splitter)
{
	int i = 0;
	int start = 0;
	int end = line.find(splitter);

	while (end != -1)
	{
		this->mapTmp.insert(std::pair<int, std::string>(i, line.substr(start, end - start)));
		start = end + splitter.size();
		end = line.find(splitter, start);
		i++;
	}
	this->mapTmp.insert(std::pair<int, std::string>(i, line.substr(start, end - start)));
}

std::string Request::getMethod()
{
	return this->startLine["method"];
}

std::string Request::getUrlPath()
{
	return this->startLine["url"];
}

std::string Request::getUrlQuery()
{
	return this->startLine["query"];
}

std::string Request::getProtocol()
{
	return this->headers["pVersion"];
}

std::string Request::getHost()
{
	return this->headers["Host"];
}

std::string Request::getConection()
{
	return this->headers["Connection"];
}

std::string Request::getContentType()
{
	return this->headers["Content-Type"];
}

std::string Request::getContentLength()
{
	return this->headers["ontent-Length"];
}

std::string Request::getTransferEncoding()
{
	return this->headers["Transfer-Encoding"];
}

std::string Request::getBoundry()
{
	return this->headers["Boundary"];
}

std::vector<Bodies> Request::getBody()
{
	return this->bodies;
}

// std::string Request::reqErrorMsg(int &status)
// {
//     /**
//      * 400 bad erquest
//      * 401 Unauthorized
//      * 402 Payement Required
//      * 403 Forbidden
//      * 404 Not Found
//      * 405 Method Nod Allowed
//      * 406 Not Acceptable
//      * 407 Proxy Authentication Required
//      * 408 Request timeout
//      * 409 Conflict
//      * 410 Gone
//      * 411 Length Required
//      * 412 Precondition Failed
//      * 413 Payload Too Large
//      * 414 URI Too Long
//     */
// }
