/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elahyani <elahyani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/01 16:54:02 by elahyani          #+#    #+#             */
/*   Updated: 2021/09/17 12:04:27 by asaadi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request() : _content(""), _method(""), _urlPath(""), _urlQuery(""), _protocol(""), _bLen(0), _statusCode(200)
{

	this->_methods.push_back("GET");
	this->_methods.push_back("POST");
	this->_methods.push_back("DELETE");
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

Request::~Request()
{
	clearRequest();
}

void Request::setRequestData(const std::string &buffer)
{
	this->_content.append(buffer);
}

void Request::parseRequest()
{
	std::string tmp;
	std::istringstream s(_content);
	try
	{
		while (std::getline(s, tmp))
		{
			// std::cout << ">>>>>>>>>>>>>>>>>>>>>>> " << tmp << std::endl;
			if (!this->_method.size() && !this->_protocol.size())
			{

				this->split(tmp, " ");
				if (this->_mapTmp.size() == 3)
				{
					if (_mapTmp[0] != _methods[0] && _mapTmp[0] != _methods[1] && _mapTmp[0] != _methods[2])
						throw std::invalid_argument("Bad Request: Method Not Allowed");
					if (_mapTmp[1].at(0) != '/')
						throw std::invalid_argument("Bad Request: Absolute path required!");
					if (_mapTmp[2].find("HTTP/1.1") != std::string::npos)
					{
						this->_mapTmp[2].pop_back();
						if (_mapTmp[2] != "HTTP/1.1")
							throw std::invalid_argument("Bad Request: Wrrong HTTP version!");
					}
					this->_method = this->_mapTmp[0];
					this->_urlPath = this->_mapTmp[1];
					this->_protocol = this->_mapTmp[2];
					if (this->_urlPath.find("?") != std::string::npos)
					{
						this->_urlQuery = this->_urlPath.substr(this->_urlPath.find("?") + 1);
						this->_urlPath = this->_urlPath.substr(0, this->_urlPath.find("?"));
					}
					this->_startLine["method"] = _method;
					this->_startLine["url"] = _urlPath;
					this->_startLine["query"] = _urlQuery;
					this->_startLine["protocol"] = _protocol;
				}
				else
					throw std::invalid_argument("Bad Request: Too much or too few arguments!");
			}
			else if (!_headers["Host"].size() && tmp.find("Host") != std::string::npos)
			{
				_headers["Host"] = tmp.substr(tmp.find(": ") + 2);
				_headers["Host"].pop_back();
			}
			else if (!_headers["Connection"].size() && tmp.find("Connection") != std::string::npos)
			{
				_headers["Connection"] = tmp.substr(tmp.find(": ") + 2);
				_headers["Connection"].pop_back();
			}
			else if (!_headers["Content-Type"].size() && tmp.find("Content-Type") != std::string::npos)
			{
				_headers["Content-Type"] = tmp.substr(tmp.find(": ") + 2);
				_headers["Content-Type"].pop_back();
				if (!_headers["Boundary"].size() && tmp.find("boundary") != std::string::npos)
				{
					_headers["Boundary"] = tmp.substr(tmp.find("boundary=") + 9);
					_headers["Boundary"].pop_back();
				}
			}
			else if (!_headers["Content-Length"].size() && tmp.find("Content-Length") != std::string::npos)
			{
				_headers["Content-Length"] = tmp.substr(tmp.find(": ") + 2);
				_headers["Content-Length"].pop_back();
			}
			else if (!_headers["Transfer-Encoding"].size() && tmp.find("Transfer-Encoding") != std::string::npos)
			{
				_headers["Transfer-Encoding"] = tmp.substr(tmp.find(": ") + 2);
				_headers["Transfer-Encoding"].pop_back();
			}
			// std::cout << "len:" << tmp.find("Content-Length") << std::endl;
			else if (this->_headers["Boundary"].size() && tmp.find(this->_headers["Boundary"]) != std::string::npos)
				break;
		}
		if (this->_headers["Boundary"].size() && tmp.find(this->_headers["Boundary"]) != std::string::npos)
			parseBody();
		checkReqErrors();
		// exit(1);
	}
	catch (const std::exception &e)
	{
		clearRequest();
		std::cerr << e.what() << '\n';
	}
}

int Request::getBodiesLen(std::string buffer)
{
	std::string tmp;
	std::istringstream bLines(buffer);
	int len = 0;

	while (std::getline(bLines, tmp))
	{
		if (tmp.find(this->_headers["Boundary"]) != std::string::npos)
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
	std::istringstream s(_content.substr(_content.find("\r\n\r\n") + 4));
	this->_bLen = getBodiesLen(_content.substr(_content.find("\r\n\r\n") + 4));
	Bodies bodies[this->_bLen];

	int i = -1;

	// std::cout << "BODIES LEN: " << bLen << std::endl;
	// std::cout << "BODY" << std::endl;
	while (std::getline(s, tmp))
	{
		// std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> " << tmp << std::endl;
		if (tmp.find(this->_headers["Boundary"]) != std::string::npos)
		{
			// std::cout << "›››››››››››››››››››››››››››››" << std::endl;
			i++;
			bodies[i].contentDesp = "";
			bodies[i].contentType = "";
			bodies[i].body = "";
			this->_bodiesList.push_back(bodies[i]);
		}
		else if (!_bodiesList[i].contentDesp.size() && tmp.find("Content-Disposition") != std::string::npos)
		{
			this->_bodiesList[i].contentDesp = tmp.substr(tmp.find(": ") + 2);
			this->_bodiesList[i].contentDesp.pop_back();
		}
		else if (!_bodiesList[i].contentType.size() && tmp.find("Content-Type") != std::string::npos)
		{
			this->_bodiesList[i].contentType = tmp.substr(tmp.find(": ") + 2);
			this->_bodiesList[i].contentType.pop_back();
		}
		else if (_bodiesList[i].contentType.size() && tmp.find(this->_headers["Boundary"]) == std::string::npos)
			this->_bodiesList[i].body.append(tmp).append("\n");
		else if (tmp.compare(this->_headers["Boundary"].append("--")) == 0)
			break;
	}
}

int Request::checkReqErrors()
{
	std::string pVersion = this->_protocol.substr(this->_protocol.find("/") + 1);

	if (pVersion.compare("1.1") != 0)
		this->_statusCode = 505;
	if (this->_protocol.compare("HTTP/1.1") != 0)
	{
		std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>here1" << std::endl;
		this->_statusCode = 400;
		exit(1);
	}
	if (this->_startLine["method"].compare("GET") != 0 && this->_startLine["method"].compare("POST") != 0 && this->_startLine["method"].compare("DELETE") != 0)
	{
		std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>here2 |" << this->_startLine["method"] << "|" << std::endl;
		this->_statusCode = 400;
		exit(1);
	}
	if (this->_method.compare("POST") == 0 && !this->_headers["Content-Length"].size())
	{
		std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>here3 |" << this->_headers["Content-Length"] << "|" << std::endl;
		this->_statusCode = 400;
		exit(1);
	}
	if (!_startLine["url"].size() || (_startLine["url"].size() && _startLine["url"][0] != '/'))
	{
		std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>here4" << std::endl;
		this->_statusCode = 400;
		exit(1);
	}
	return this->_statusCode;
}

int &Request::getStatusCode()
{
	return this->_statusCode;
}

void Request::printRequest()
{
	std::cout << "+++++++++++++++++++++++++++++++++++++" << std::endl;
	std::cout << "Method            -> |" << this->_startLine["method"] << "|" << std::endl;
	std::cout << "Url               -> |" << this->_startLine["url"] << "|" << std::endl;
	std::cout << "Protocol Version  -> |" << this->_startLine["protocol"] << "|" << std::endl;
	std::cout << "Host              -> |" << this->_headers["Host"] << "|" << std::endl;
	std::cout << "Connection        -> |" << this->_headers["Connection"] << "|" << std::endl;
	std::cout << "Content Type      -> |" << this->_headers["Content-Type"] << "|" << std::endl;
	std::cout << "Content Length    -> |" << this->_headers["Content-Length"] << "|" << std::endl;
	std::cout << "Transfer Encoding -> |" << this->_headers["Transfer-Encoding"] << "|" << std::endl;
	std::cout << "Boundary          -> |" << this->_headers["Boundary"] << "|" << std::endl;
	if (this->_bodiesList.size())
	{
		std::cout << "Content-Dispos... -> |" << this->_bodiesList[0].contentDesp << "|" << std::endl;
		std::cout << "Content-Type      -> |" << this->_bodiesList[0].contentType << "|" << std::endl;
		std::cout << "Body              -> |" << this->_bodiesList[0].body << "|" << std::endl;
	}
	std::cout << "+++++++++++++++++++++++++++++++++++++" << std::endl;
	// exit(1);
}

void Request::split(std::string line, std::string splitter)
{
	int i = 0;
	int start = 0;
	int end = line.find(splitter);

	while (end != -1)
	{
		this->_mapTmp.insert(std::pair<int, std::string>(i, line.substr(start, end - start)));
		start = end + splitter.size();
		end = line.find(splitter, start);
		i++;
	}
	this->_mapTmp.insert(std::pair<int, std::string>(i, line.substr(start, end - start)));
}

std::string &Request::getHeaderVal(std::string const &key)
{
	return this->_headers[key];
}

std::string &Request::getStartLineVal(std::string const &key)
{
	return this->_startLine[key];
}

std::vector<Bodies> Request::getBody()
{
	return this->_bodiesList;
}

void Request::clearRequest()
{
	this->_headers.clear();
	this->_startLine.clear();
	this->_mapTmp.clear();
	this->_errors.clear();
	this->_methods.clear();
	this->_bodiesList.clear();
	this->_method.clear();
	this->_urlPath.clear();
	this->_urlQuery.clear();
	this->_protocol.clear();
	this->_content.clear();
}