/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpServer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ichejra <ichejra@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/13 11:39:58 by ichejra           #+#    #+#             */
/*   Updated: 2021/09/25 12:52:39 by ichejra          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpServer.hpp"

HttpServer::HttpServer() : host(""),
						   clientMaxBodySize(),
						   errorsPages(),
						   root(""),
						   errorCode(),
						   errorPagePath(),
						   locationsNumber(0)
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
		this->ports.assign(src.ports.begin(), src.ports.end());
		this->host = src.host;
		this->clientMaxBodySize = src.clientMaxBodySize;
		//! assign to map 
		this->errorsPages = src.errorsPages;
		this->root = src.root;
		this->serverName.assign(src.serverName.begin(), src.serverName.end());
		this->errorCode = src.errorCode;
		this->errorPagePath = src.errorPagePath;
		this->locationsNumber = src.locationsNumber;
		this->locations.assign(src.locations.begin(), src.locations.end());
	}
	return *this;
}

void HttpServer::setPorts(int port)
{
	this->ports.push_back(port);
}

std::vector<int> const &HttpServer::getPorts() const
{
	return this->ports;
}

void HttpServer::setHost(std::string _host)
{
	this->host = _host;
}

std::string const &HttpServer::getHost() const
{
	return this->host;
}

void HttpServer::setClientMaxBodySize(size_t cmbs)
{
	this->clientMaxBodySize = cmbs;
}

size_t const &HttpServer::getClientMaxBodySize() const
{
	return this->clientMaxBodySize;
}

void HttpServer::setErrorsPages(int _code, std::string _path)
{
	this->errorsPages[_code] = _path;
}

std::map<int, std::string> const &HttpServer::getErrorsPages() const
{
	return this->errorsPages;
}

void HttpServer::setRoot(std::string _root)
{
	_root.pop_back();
	this->root = _root;
}

std::string const &HttpServer::getRoot() const
{
	return this->root;
}

void HttpServer::setServerName(std::string _serverName)
{
	this->serverName.push_back(_serverName);
}

std::vector<std::string> const &HttpServer::getServerName() const
{
	return this->serverName;
}

void HttpServer::setErrorCode(int code)
{
	this->errorCode = code;
}

int const &HttpServer::getErrorCode() const
{
	return this->errorCode;
}

void HttpServer::setErrorPagePath(std::string path)
{
	this->errorPagePath = path;
}

std::string const &HttpServer::getErrorPagePath() const
{
	return this->errorPagePath;
}

void HttpServer::setLocation(Location const &_location)
{
	for (size_t i = 0; i < this->locations.size(); i++)
	{
		if (this->locations[i].getLocationName() == _location.getLocationName())
			throw std::invalid_argument("Error: duplicated location path: " + _location.getLocationName());
	}
	this->locations.push_back(_location);
}

std::vector<Location> const &HttpServer::getLoactions() const
{
	return this->locations;
}

void HttpServer::clearAll()
{
	this->ports.clear();
	this->host.clear();
	this->clientMaxBodySize = 0;
	this->errorsPages.clear();
	this->root.clear();
	this->serverName.clear();
	this->errorCode = 0;
	this->errorPagePath.clear();
	this->locations.clear();
}