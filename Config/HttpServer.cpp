/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpServer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ichejra <ichejra@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/13 11:39:58 by ichejra           #+#    #+#             */
/*   Updated: 2021/09/13 17:37:43 by ichejra          ###   ########.fr       */
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
}

void HttpServer::setPorts(int port)
{
	this->ports.push_back(port);
}

std::vector<int> HttpServer::getPorts()
{
	return this->ports;
}

void HttpServer::setHost(std::string _host)
{
	_host.pop_back();
	this->host = _host;
}

std::string HttpServer::getHost()
{
	return this->host;
}

void HttpServer::setClientMaxBodySize(size_t cmbs)
{
	this->clientMaxBodySize = cmbs;
}

size_t HttpServer::getClientMaxBodySize()
{
	return this->clientMaxBodySize;
}

void HttpServer::setErrorsPages(int _code, std::string _path)
{
	this->errorsPages[_code] = _path;
}

std::map<int, std::string> &HttpServer::getErrorsPages()
{
	return this->errorsPages;
}

void HttpServer::setRoot(std::string _root)
{
	_root.pop_back();
	this->root = _root;
}

std::string HttpServer::getRoot()
{
	return this->root;
}

void HttpServer::setServerName(std::string _serverName)
{
	this->serverName.push_back(_serverName);
}

std::vector<std::string> HttpServer::getServerName()
{
	return this->serverName;
}

void HttpServer::setErrorCode(int code)
{
	this->errorCode = code;
}

int HttpServer::getErrorCode()
{
	return this->errorCode;
}

void HttpServer::setErrorPagePath(std::string path)
{
	this->errorPagePath = path;
}

std::string HttpServer::getErrorPagePath()
{
	return this->errorPagePath;
}
