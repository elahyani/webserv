/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asaadi <asaadi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/01 16:54:02 by elahyani          #+#    #+#             */
/*   Updated: 2021/09/17 12:04:27 by asaadi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(const std::string &buffer) : content(""), method(""), urlPath(""), urlQuery(""), protocol(""), _newSockFd(0), bLen(0), statusCode(200)
{
	this->content.append(buffer);
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

// bool Request::checkRequest(std::string &req)
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
// 		data = data.substr(0, data.length() - 4);
// 		if (data.find("\r\n\r\n") == std::string::npos)
// 		{
// 			return false;
// 		}
// 	}
// 	return true;
// }

// void Request::readRequest()
// {
// 	int valRead;
// 	char _buffReq[16384] = {0};

// 	while (true)
// 	{
// 		std::memset(_buffReq, 0, sizeof(_buffReq));
// 		valRead = recv(_newSockFd, _buffReq, sizeof(_buffReq) - 1, 0);
// 		if (valRead == -1)
// 			throw std::runtime_error("Unable to receive the request from client.");
// 		_buffReq[valRead] = '\0';
// 		content.append(_buffReq);
// 		std::cout << ".........." << _buffReq << ".........." << std::endl;
// 		if (checkRequest(content))
// 			break;
// 	}
// 	// std::cout << content << std::endl;
// }

void Request::parseRequest()
{
	std::string tmp;
	std::istringstream s(content);
	try
	{
		while (std::getline(s, tmp))
		{
			// std::cout << ">>>>>>>>>>>>>>>>>>>>>>> " << tmp << std::endl;
			if (!this->method.size() && !this->protocol.size())
			{

				this->split(tmp, " ");
				if (this->mapTmp.size() == 3)
				{
					if (mapTmp[0] != methods[0] && mapTmp[0] != methods[1] && mapTmp[0] != methods[2])
						throw std::invalid_argument("Bad Request: Method Not Allowed");
					if (mapTmp[1].at(0) != '/')
						throw std::invalid_argument("Bad Request: Absolute path required!");
					if (mapTmp[2].find("HTTP/1.1") != std::string::npos)
					{
						this->mapTmp[2].pop_back();
						if (mapTmp[2] != "HTTP/1.1")
							throw std::invalid_argument("Bad Request: Wrrong HTTP version!");
					}
					this->method = this->mapTmp[0];
					this->urlPath = this->mapTmp[1];
					this->protocol = this->mapTmp[2];
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
			{
				headers["Content-Type"] = tmp.substr(tmp.find(": ") + 2);
				if (!headers["Boundary"].size() && tmp.find("boundary") != std::string::npos)
					headers["Boundary"] = tmp.substr(tmp.find("boundary=") + 9);
			}
			else if (!headers["Content-Length"].size() && tmp.find("Content-Length") != std::string::npos)
				headers["Content-Length"] = tmp.substr(tmp.find(": ") + 2);
			else if (!headers["Transfer-Encoding"].size() && tmp.find("Transfer-Encoding") != std::string::npos)
				headers["Transfer-Encoding"] = tmp.substr(tmp.find(": ") + 2);
			// std::cout << "len:" << tmp.find("Content-Length") << std::endl;
			else if (this->headers["Boundary"].size() && tmp.find(this->headers["Boundary"]) != std::string::npos)
				break;
		}
		if (this->headers["Boundary"].size() && tmp.find(this->headers["Boundary"]) != std::string::npos)
			parseBody();
		checkReqErrors();
		// exit(1);
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}
	this->startLine["method"] = method;
	this->startLine["url"] = urlPath;
	this->startLine["query"] = urlQuery;
	this->startLine["protocol"] = protocol;
}

int  Request::getBodiesLen(std::string buffer)
{
	std::string tmp;
	std::istringstream bLines(buffer);
	int len = 0;

	while (std::getline(bLines, tmp))
	{
		if (tmp.find(this->headers["Boundary"]) != std::string::npos)
		{
			// std::cout << "" << std::endl;
			len++;
		}
	}
	return len;
}

void Request::parseBody()
{
	std::string tmp;
	std::istringstream s(content.substr(content.find("\r\n\r\n") + 4));
	this->bLen = getBodiesLen(content.substr(content.find("\r\n\r\n") + 4));
	Bodies bodies[this->bLen];

	int i = -1;

	// std::cout << "BODIES LEN: " << bLen << std::endl;
	// std::cout << "BODY" << std::endl;
	while (std::getline(s, tmp))
	{
		// std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> " << tmp << std::endl;
		if (tmp.find(this->headers["Boundary"]) != std::string::npos)
		{
			// std::cout << "›››››››››››››››››››››››››››››" << std::endl;
			i++;
			bodies[i].contentDesp = "";
			bodies[i].contentType = "";
			bodies[i].body = "";
			this->bodiesList.push_back(bodies[i]);
		}
		else if (!bodiesList[i].contentDesp.size() && tmp.find("Content-Disposition") != std::string::npos)
			this->bodiesList[i].contentDesp = tmp.substr(tmp.find(": ") + 2);
		else if (!bodiesList[i].contentType.size() && tmp.find("Content-Type") != std::string::npos)
			this->bodiesList[i].contentType = tmp.substr(tmp.find(": ") + 2);
		else if (bodiesList[i].contentType.size() && tmp.find(this->headers["Boundary"]) == std::string::npos)
			this->bodiesList[i].body.append(tmp).append("\n");
		else if (tmp.compare(this->headers["Boundary"].append("--")) == 0)
			break;
	}
}

int Request::checkReqErrors()
{
	std::string pVersion = this->protocol.substr(this->protocol.find("/") + 1);

	if (pVersion.compare("1.1") != 0)
		this->statusCode = 505;
	if (this->protocol.compare("HTTP/1.1") != 0)
	{
		this->statusCode = 400;
	}
	if (this->startLine["method"].compare("GET") != 0 && this->startLine["method"].compare("POST") != 0 && this->startLine["method"].compare("DELETE") != 0)
		this->statusCode = 400;
	if (this->method.compare("POST") == 0)
	{
		if (!this->headers["Content-Length"].size())
			this->statusCode = 400;
	}
	if (!startLine["url"].size() || (startLine["url"].size() && startLine["url"][0] != '/'))
		this->statusCode = 400;
	return this->statusCode;
}

int & Request::getStatusCode()
{
	return this->statusCode;
}

void Request::printRequest()
{
	std::cout << "+++++++++++++++++++++++++++++++++++++" << std::endl;
	std::cout << "Method            -> |" << this->startLine["method"] << "|" << std::endl;
	std::cout << "Url               -> |" << this->startLine["url"] << "|" << std::endl;
	std::cout << "Protocol Version  -> |" << this->startLine["protocol"] << "|" << std::endl;
	std::cout << "Host              -> |" << this->headers["Host"] << "|" << std::endl;
	std::cout << "Connection        -> |" << this->headers["Connection"] << "|" << std::endl;
	std::cout << "Content Type      -> |" << this->headers["Content-Type"] << "|" << std::endl;
	std::cout << "Content Length    -> |" << this->headers["Content-Length"] << "|" << std::endl;
	std::cout << "Transfer Encoding -> |" << this->headers["Transfer-Encoding"] << "|" << std::endl;
	std::cout << "Boundary          -> |" << this->headers["Boundary"] << "|" << std::endl;
	if (this->bodiesList.size())
	{
		std::cout << "Content-Dispos... -> |" << this->bodiesList[0].contentDesp << "|" << std::endl;
		std::cout << "Content-Type      -> |" << this->bodiesList[0].contentType << "|" << std::endl;
		std::cout << "Body              -> |" << this->bodiesList[0].body << "|" << std::endl;
	}
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

std::string & Request::getHeaderVal(std::string const &key)
{
	return this->headers[key];
}

std::string & Request::getStartLineVal(std::string const &key)
{
	return this->startLine[key];
}

std::vector<Bodies> Request::getBody()
{
	return this->bodiesList;
}
