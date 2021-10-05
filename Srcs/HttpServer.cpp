/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpServer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asaadi <asaadi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/13 11:39:58 by ichejra           #+#    #+#             */
/*   Updated: 2021/09/25 12:52:39 by ichejra          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpServer.hpp"

HttpServer::HttpServer() : _host(""),
						   _clientMaxBodySize(),
						   _errorsPages(),
						   _root(""),
						   _errorCode(),
						   _errorPagePath(),
						   _locationsNumber(0)
{
}

HttpServer::~HttpServer()
{
	this->clearAll();
}

HttpServer::HttpServer(HttpServer const &src)
{
	*this = src;
}

HttpServer &HttpServer::operator=(HttpServer const &src)
{
	//! use assign
	if (this != &src)
	{
		this->_ports.assign(src._ports.begin(), src._ports.end());
		this->_host = src._host;
		this->_clientMaxBodySize = src._clientMaxBodySize;
		//! assign to map
		this->_errorsPages = src._errorsPages;
		this->_root = src._root;
		this->_serverName.assign(src._serverName.begin(), src._serverName.end());
		this->_errorCode = src._errorCode;
		this->_errorPagePath = src._errorPagePath;
		this->_locationsNumber = src._locationsNumber;
		this->_locations.assign(src._locations.begin(), src._locations.end());
	}
	return *this;
}

void HttpServer::setPorts(short port)
{
	this->_ports.push_back(port);
}

std::vector<short> &HttpServer::getPorts()
{
	return this->_ports;
}

void HttpServer::setHost(std::string _host)
{
	this->_host = _host;
}

std::string &HttpServer::getHost()
{
	return this->_host;
}

void HttpServer::setClientMaxBodySize(size_t cmbs)
{
	this->_clientMaxBodySize = cmbs;
}

size_t &HttpServer::getClientMaxBodySize()
{
	return this->_clientMaxBodySize;
}

void HttpServer::setErrorsPages(int _code, std::string _path)
{
	this->_errorsPages[_code] = _path;
}

std::map<int, std::string> &HttpServer::getErrorsPages()
{
	return this->_errorsPages;
}

void HttpServer::setRoot(std::string _root)
{
	this->_root = _root;
}

std::string &HttpServer::getRoot()
{
	return this->_root;
}

void HttpServer::setServerName(std::string _serverName)
{
	this->_serverName.push_back(_serverName);
}

std::vector<std::string> &HttpServer::getServerName()
{
	return this->_serverName;
}

void HttpServer::setErrorCode(int code)
{
	this->_errorCode = code;
}

int &HttpServer::getErrorCode()
{
	return this->_errorCode;
}

void HttpServer::setErrorPagePath(std::string path)
{
	this->_errorPagePath = path;
}

std::string &HttpServer::getErrorPagePath()
{
	return this->_errorPagePath;
}

void HttpServer::setLocation(Location &_location)
{
	for (size_t i = 0; i < this->_locations.size(); i++)
	{
		if (this->_locations[i].getLocationName() == _location.getLocationName())
			throw std::invalid_argument("Error: duplicated location path: " + _location.getLocationName());
	}
	this->_locations.push_back(_location);
}

std::vector<Location> &HttpServer::getLocations()
{
	return this->_locations;
}

void HttpServer::clearAll()
{
	this->_ports.clear();
	this->_host.clear();
	this->_clientMaxBodySize = 0;
	this->_errorsPages.clear();
	this->_root.clear();
	this->_serverName.clear();
	this->_errorCode = 0;
	this->_errorPagePath.clear();
	this->_locations.clear();
}