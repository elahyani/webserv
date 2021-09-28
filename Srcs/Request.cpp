/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asaadi <asaadi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/01 16:54:02 by elahyani          #+#    #+#             */
/*   Updated: 2021/09/21 08:40:38 by asaadi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request() : _content(""),
					 _method(""),
					 _uriPath(""),
					 _urlQuery(""),
					 _protocol(""),
					 _scriptName(""),
					 _bLen(0),
					 _statusCode(200)
{

	this->_methods.push_back("GET");
	this->_methods.push_back("POST");
	this->_methods.push_back("DELETE");
}

Request::Request(const Request &src)
{
	*this = src;
	(void)src;
}

Request &Request::operator=(const Request &rhs)
{
	if (this != &rhs)
	{
		this->_headers = rhs._headers;
		this->_startLine = rhs._startLine;
		this->_errors = rhs._errors;
		this->_mapTmp = rhs._mapTmp;
		this->_methods = rhs._methods;
		this->_bodiesList = rhs._bodiesList;
		this->_content = rhs._content;
		this->_method = rhs._method;
		this->_uriPath = rhs._uriPath;
		this->_urlQuery = rhs._urlQuery;
		this->_protocol = rhs._protocol;
	}
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
				std::cout << "__>" + tmp << std::endl;
				this->split(tmp, ' ');
				if (this->_mapTmp.size() == 3)
				{
					if (_mapTmp[0] != _methods[0] && _mapTmp[0] != _methods[1] && _mapTmp[0] != _methods[2])
						throw std::invalid_argument("Bad Request: Method Not Allowed");
					else if (_mapTmp[1].at(0) != '/')
						throw std::invalid_argument("Bad Request: Absolute path required!");
					else if (_mapTmp[2].find("HTTP/1.1") != std::string::npos)
					{
						this->_mapTmp[2].pop_back();
						if (_mapTmp[2] != "HTTP/1.1")
							throw std::invalid_argument("Bad Request: Wrrong HTTP version!");
					}
					this->_method = this->_mapTmp[0];
					this->_uriPath = this->_mapTmp[1];
					this->_protocol = this->_mapTmp[2];
					if (this->_uriPath.find("?") != std::string::npos)
					{
						this->_urlQuery = this->_uriPath.substr(this->_uriPath.find("?") + 1);
						this->_uriPath = this->_uriPath.substr(0, this->_uriPath.find("?"));
					}
					split(_uriPath, '/');
					for (size_t i = 0; i < _mapTmp.size(); i++)
					{
						if (_mapTmp[i].find(".php") != std::string::npos || _mapTmp[i].find(".py") != std::string::npos)
						{
							this->_scriptName = _mapTmp[i];
							break;
						}
					}
					split(_scriptName, '.');
					this->_startLine["method"] = _method;
					this->_startLine["uri"] = _uriPath;
					this->_startLine["query"] = _urlQuery;
					this->_startLine["protocol"] = _protocol;
					if (_mapTmp[1].compare("php") == 0 || _mapTmp[1].compare("py") == 0)
						this->_startLine["script-name"] = _scriptName;
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
	std::string bodyboundary = "--" + this->_headers["Boundary"];
	std::istringstream bLines(buffer);
	int len = 0;

	while (std::getline(bLines, tmp))
	{
		tmp.pop_back();
		if (tmp.compare(bodyboundary) == 0)
			len++;
	}
	return len;
}

void Request::parseBody()
{
	std::string tmp;
	std::istringstream s(_content.substr(_content.find("\r\n\r\n") + 4));
	this->_bLen = getBodiesLen(_content.substr(_content.find("\r\n\r\n") + 4));
	Bodies bodies[this->_bLen];
	std::string bodyBoundary = "--" + this->_headers["Boundary"];
	int i = 0;

	// std::cout << "BODIES LEN: " << _bLen << std::endl;
	// std::cout << "BODY" << std::endl;
	while (std::getline(s, tmp))
	{
		if (tmp.find(bodyBoundary) != std::string::npos)
		{
			if (tmp.compare(bodyBoundary + "\r") == 0 || tmp.compare(bodyBoundary + "--\r") == 0)
			{
				if (bodies[i].contentType.size()) // i = 0;
				{
					// std::cout << "››››››››››››››››››››››››››››› ADD BODY" << std::endl;
					_bodiesList.push_back(bodies[i]);
					bodies[i].contentDesp = "";
					bodies[i].contentType = "";
					bodies[i].body = "";
					i++;
				}
			}
		}
		if (i == _bLen)
			break;
		else if (!bodies[i].contentDesp.size() && tmp.find("Content-Disposition") != std::string::npos)
		{
			bodies[i].contentDesp = tmp.substr(tmp.find(": ") + 2);
			bodies[i].contentType.pop_back();
		}
		else if (!bodies[i].contentType.size() && tmp.find("Content-Type") != std::string::npos)
		{
			bodies[i].contentType = tmp.substr(tmp.find(": ") + 2);
			bodies[i].contentType.pop_back();
		}
		else if (bodies[i].contentType.size() && tmp.compare(bodyBoundary) != 0 && tmp.compare("\r") != 0)
		{
			bodies[i].body.append(tmp + "\n");
		}
	}
	// exit(1);
}

int Request::checkReqErrors()
{
	std::string pVersion = this->_protocol.substr(this->_protocol.find("/") + 1);

	if (pVersion.compare("1.1") != 0)
		this->_statusCode = 505;
	else if (this->_protocol.compare("HTTP/1.1") != 0)
		this->_statusCode = 400;
	else if (this->_startLine["method"].compare("GET") != 0 && this->_startLine["method"].compare("POST") != 0 && this->_startLine["method"].compare("DELETE") != 0)
		this->_statusCode = 405;
	else if (this->_method.compare("POST") == 0 && !this->_headers["Content-Length"].size())
		this->_statusCode = 400;
	else if (!_startLine["uri"].size() || (_startLine["uri"].size() && _startLine["uri"][0] != '/'))
		this->_statusCode = 400;
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
	std::cout << "Url               -> |" << this->_startLine["uri"] << "|" << std::endl;
	std::cout << "Protocol Version  -> |" << this->_startLine["protocol"] << "|" << std::endl;
	std::cout << "Script Name       -> |" << this->_startLine["script-name"] << "|" << std::endl;
	std::cout << "Host              -> |" << this->_headers["Host"] << "|" << std::endl;
	std::cout << "Connection        -> |" << this->_headers["Connection"] << "|" << std::endl;
	std::cout << "Content Type      -> |" << this->_headers["Content-Type"] << "|" << std::endl;
	std::cout << "Content Length    -> |" << this->_headers["Content-Length"] << "|" << std::endl;
	std::cout << "Transfer Encoding -> |" << this->_headers["Transfer-Encoding"] << "|" << std::endl;
	std::cout << "Boundary          -> |" << this->_headers["Boundary"] << "|" << std::endl;
	for (size_t i = 0; i < _bodiesList.size(); i++)
	{
		std::cout << "Content-Dispos... -> |" << this->_bodiesList[i].contentDesp << "|" << std::endl;
		std::cout << "Content-Type      -> |" << this->_bodiesList[i].contentType << "|" << std::endl;
		std::cout << "Body              -> |" << this->_bodiesList[i].body << "|" << std::endl;
	}
	std::cout << "+++++++++++++++++++++++++++++++++++++" << std::endl;
	// exit(1);
}

void Request::split(std::string line, char splitter)
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

void Request::setHeaderVal(std::string key, std::string val)
{
	this->_headers[key] = val;
}

std::string &Request::getHeaderVal(std::string const &key)
{
	return this->_headers[key];
}

void Request::setStartLineVal(std::string key, std::string val)
{
	this->_startLine[key] = val;
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
	this->_uriPath.clear();
	this->_urlQuery.clear();
	this->_protocol.clear();
	this->_content.clear();
}