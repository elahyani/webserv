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
					 _body(""),
					 _bLen(0),
					 _statusCode(200),
					 _maxBodySize(0)
{
	_headers["Connection"] = "keep-alive";
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
	// clearRequest();
}

void Request::setRequestData(const std::string &buffer, int &max_body_size)
{
	this->_content = buffer;
	_maxBodySize = max_body_size;
}

void Request::parseRequest()
{
	std::string tmp;
	std::istringstream s(_content);

	_headers["Connection"] = "keep-alive";
	_statusCode = 200;
	try
	{
		while (std::getline(s, tmp))
		{
			if (!this->_method.size() && !this->_protocol.size())
			{
				this->split(tmp, ' ');
				if (this->_mapTmp.size() == 3)
				{
					this->_method = this->_mapTmp[0];
					this->_uriPath = this->_mapTmp[1];
					this->_protocol = this->_mapTmp[2];
					_protocol.pop_back();
					if (_method != "GET" && _method != "POST" && _method != "DELETE")
						throw std::runtime_error("Bad Request: Method Not Allowed");
					else if (_uriPath.at(0) != '/')
						throw std::runtime_error("Bad Request: Absolute path required!");
					else if (_protocol.find("HTTP") != std::string::npos)
					{
						if (_protocol.substr(0, _protocol.find("/")).compare("HTTP") != 0)
							throw std::runtime_error("Exception: Syntax error at line 2-> " + tmp);
						if (_protocol.substr(_protocol.find("/") + 1).compare("1.1") != 0)
							throw std::runtime_error("Bad Request: Wrrong HTTP version!");
					}
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
					throw std::runtime_error("Bad Request: Too many or too few arguments!");
			}
			else if (tmp.find("Host") != std::string::npos)
			{
				if (!_headers["Host"].size())
				{
					if (tmp.find("Host: ") == std::string::npos)
						throw std::runtime_error("Exception: Syntax error at line 1-> " + tmp);
					_headers["Host"] = tmp.substr(tmp.find(": ") + 2);
					_headers["Host"].pop_back();
					if (_headers["Host"][0] == ':')
						throw std::runtime_error("Exception: Syntax error at line 2-> " + tmp);
				}
				else
					throw std::runtime_error("Execption: Duplicated Header : " + tmp);
			}
			else if (tmp.find("Connection") != std::string::npos)
			{
				if (tmp.find("Connection: ") == std::string::npos)
					throw std::runtime_error("Exception: Syntax error at line -> " + tmp);
				if (tmp.find(":") == std::string::npos)
					throw std::runtime_error("Exception: Syntax error at line -> " + tmp);
				_headers["Connection"] = tmp.substr(tmp.find(": ") + 2);
				_headers["Connection"].pop_back();
				if (_headers["Connection"].find("close") != std::string::npos)
					_headers["Connection"] = "close";
				else
					_headers["Connection"] = "keep-alive";
			}
			else if (tmp.find("Content-Type") != std::string::npos)
			{
				if (!_headers["Content-Type"].size())
				{
					if (tmp.find(":") == std::string::npos || std::count(tmp.begin(), tmp.end(), ':') > 1)
						throw std::runtime_error("Exception: Syntax error at line -> " + tmp);
					_headers["Content-Type"] = tmp.substr(tmp.find(": ") + 2);
					_headers["Content-Type"].pop_back();
					if (!_headers["Boundary"].size() && tmp.find("boundary") != std::string::npos)
					{
						_headers["Boundary"] = tmp.substr(tmp.find("boundary=") + 9);
						_headers["Boundary"].pop_back();
					}
				}
				else
					throw std::runtime_error("Execption: Duplicated Header : " + tmp);
			}
			else if (tmp.find("Content-Length") != std::string::npos)
			{
				if (!_headers["Content-Length"].size())
				{
					if (tmp.find(":") == std::string::npos || std::count(tmp.begin(), tmp.end(), ':') > 1)
						throw std::runtime_error("Exception: Syntax error at line -> " + tmp);
					_headers["Content-Length"] = tmp.substr(tmp.find(": ") + 2);
					_headers["Content-Length"].pop_back();
					if (!_headers["Content-Length"].size())
						throw std::runtime_error("Exception: Syntax error at line -> " + tmp);
					std::cout << "++++++________++++++++++==============>>> |" << _headers["Content-Length"] << "|" << _maxBodySize << "|" << std::endl;
					for (size_t i = 0; i < _headers["Content-Length"].size(); i++)
					{
						if (!std::isdigit(_headers["Content-Length"][i]))
							throw std::runtime_error("Exception: Syntax error at line -> " + tmp);
					}
					try
					{
						if (std::stoi(_headers["Content-Length"]) > _maxBodySize)
						{
							_statusCode = 413;
							throw std::runtime_error("Execption: Request Entity Too Large");
						}
					}
					catch (const std::exception &e)
					{
						this->_statusCode = 413;
						std::cout << "CATCHED 413:)" << std::endl;
						std::cerr << e.what() << '\n';
					}
				}
				else
					throw std::runtime_error("Execption: Duplicated Header : " + tmp);
			}
			else if (tmp.find("Transfer-Encoding") != std::string::npos)
			{
				if (!_headers["Transfer-Encoding"].size())
				{
					if (tmp.find(":") == std::string::npos || std::count(tmp.begin(), tmp.end(), ':') > 1)
						throw std::runtime_error("Exception: Syntax error at line -> " + tmp);
					_headers["Transfer-Encoding"] = tmp.substr(tmp.find(": ") + 2);
					_headers["Transfer-Encoding"].pop_back();
				}
				else
					throw std::runtime_error("Execption: Duplicated Header : " + tmp);
			}
			else if (tmp.find("Cookie") != std::string::npos)
			{
				if (!_headers["Cookie"].size())
				{
					if (tmp.find(":") == std::string::npos || std::count(tmp.begin(), tmp.end(), ':') > 1)
						throw std::runtime_error("Exception: Syntax error at line -> " + tmp);
					_headers["Cookie"] = tmp.substr(tmp.find(": ") + 2);
					_headers["Cookie"].pop_back();
					split(_headers["Cookie"], '&');
					for (size_t i = 0; i < _mapTmp.size(); i++)
						_cookies.push_back(_mapTmp[i]);
				}
				else
					throw std::runtime_error("Execption: Duplicated Header : " + tmp);
			}
			// std::cout << "len:" << tmp.find("Content-Length") << std::endl;
			else if ((this->_headers["Boundary"].size() && tmp.find(this->_headers["Boundary"]) != std::string::npos) || tmp.find("\r\n\r\n") != std::string::npos)
			{
				std::cout << "-----------> |" << this->_headers["Boundary"].size() << "|" << std::endl;
				std::cout << "-----------> |" << tmp << "|" << std::endl;
				std::cout << "AM OUT BUT WITH WHAT?" << std::endl;
				break;
			}
		}
		if ((this->_headers["Boundary"].size() && tmp.find(this->_headers["Boundary"]) != std::string::npos) || _headers["Content-Length"].size() || _headers["Transfer-Encoding"].size())
			parseBody();
		// exit(1);
	}
	catch (const std::exception &e)
	{
		std::cout << "CATCHED :)" << std::endl;
		_statusCode = 400;
		setStartLineVal("protocol", "HTTP/1.1");
		setHeaderVal("Connection", "close");
		std::cerr << e.what() << '\n';
	}
	checkReqErrors();
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

//! refactor this shit
void Request::parseBody()
{
	std::string tmp;
	std::istringstream s(_content.substr(_content.find("\r\n\r\n") + 4));
	this->_bLen = getBodiesLen(_content.substr(_content.find("\r\n\r\n") + 4));
	Bodies bodies[this->_bLen];
	std::string bodyBoundary = "--" + this->_headers["Boundary"];
	int i = 0;

	std::cout << "BODIES LEN: " << _bLen << std::endl;
	std::cout << "-+-+-+-+-+-+-+-" << std::endl;
	std::cout << _content.substr(_content.find("\r\n\r\n") + 4) << std::endl;
	std::cout << "-+-+-+-+-+-+-+-" << std::endl;
	// exit(1);
	if (this->_headers["Boundary"].size())
	{
		std::cout << "read body" << std::endl;
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
	}
	else
	{
		std::string body = "";

		std::cout << "I WAS HERE" << std::endl;
		while (std::getline(s, tmp))
		{
			if (tmp.back() == '\r')
				tmp.pop_back();
			body.append(tmp);
		}
		if (body.size())
			this->_bLen++;
		setReqBody(body);
		// std::cout << "bodyyyyy -> " << getReqBody() << std::endl;
	}
	// exit(1);
}
//! *******************************************
//! *******************************************
int Request::checkReqErrors()
{
	std::string pVersion = this->_protocol.substr(this->_protocol.find("/") + 1);

	// std::cout << "max body size -> " << _serverData.getClientMaxBodySize() << std::endl;

	std::cout << "--> lens |" << getReqBody().size() << "|" << getBody().size() << "|" << _bLen << "|" << std::endl;
	for (size_t i = 0; i < _method.size(); i++)
	{
		if (std::islower(_method[i]))
		{
			_statusCode = 400;
			return this->_statusCode;
		}
	}
	if (!_method.size())
		this->_statusCode = 400;
	else if (pVersion.compare("1.1") != 0)
	{
		setStartLineVal("protocol", "HTTP/1.1");
		this->_statusCode = 505;
	}
	else if (this->_protocol.compare("HTTP/1.1") != 0)
	{
		setStartLineVal("protocol", "HTTP/1.1");
		this->_statusCode = 400;
	}
	else if (this->_method.compare("GET") != 0 && this->_method.compare("POST") != 0 && this->_method.compare("DELETE") != 0)
		this->_statusCode = 501;
	else if (this->_method.compare("POST") == 0 && !this->_headers["Content-Length"].size() && !this->_headers["Transfer-Encoding"].size())
	{
		std::cout << "1" << std::endl;
		this->_statusCode = 400;
	}
	else if (_headers["Content-Length"].size() && !_bLen && _statusCode != 413)
	{
		std::cout << "2" << std::endl;
		this->_statusCode = 400;
	}
	else if (!_startLine["uri"].size() || (_startLine["uri"].size() && _startLine["uri"][0] != '/'))
	{
		std::cout << "3" << std::endl;
		this->_statusCode = 400;
	}
	else if (!_headers["Host"].size())
	{
		std::cout << "4" << std::endl;
		this->_statusCode = 400;
	}

	if (this->_statusCode != 200)
		setHeaderVal("Connection", "close");
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
	std::cout << "Query             -> |" << this->_startLine["query"] << "|" << std::endl;
	std::cout << "Protocol Version  -> |" << this->_startLine["protocol"] << "|" << std::endl;
	std::cout << "Script Name       -> |" << this->_startLine["script-name"] << "|" << std::endl;
	std::cout << "Host              -> |" << this->_headers["Host"] << "|" << std::endl;
	std::cout << "Connection        -> |" << this->_headers["Connection"] << "|" << std::endl;
	std::cout << "Content Type      -> |" << this->_headers["Content-Type"] << "|" << std::endl;
	std::cout << "Content Length    -> |" << this->_headers["Content-Length"] << "|" << std::endl;
	std::cout << "Cookie            -> |" << this->_headers["Cookie"] << "|" << std::endl;
	std::cout << "Transfer Encoding -> |" << this->_headers["Transfer-Encoding"] << "|" << std::endl;
	std::cout << "Boundary          -> |" << this->_headers["Boundary"] << "|" << std::endl;
	// for (size_t i = 0; i < _bodiesList.size(); i++)
	// {
	// 	std::cout << "Content-Dispos... -> |" << this->_bodiesList[i].contentDesp << "|" << std::endl;
	// 	std::cout << "Content-Type      -> |" << this->_bodiesList[i].contentType << "|" << std::endl;
	// 	std::cout << "Body              -> |" << this->_bodiesList[i].body << "|" << std::endl;
	// }
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

void Request::setReqBody(std::string const &body)
{
	this->_body = body;
}

std::string &Request::getReqBody()
{
	return this->_body;
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
	this->_bodiesList.clear();
	this->_method.clear();
	this->_uriPath.clear();
	this->_urlQuery.clear();
	this->_protocol.clear();
	this->_content.clear();
	this->_cookies.clear();
}