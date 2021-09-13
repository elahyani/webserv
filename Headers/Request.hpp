/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asaadi <asaadi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/01 13:44:07 by elahyani          #+#    #+#             */
/*   Updated: 2021/09/13 11:00:55 by asaadi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <sstream>
#include <cctype>
#include <vector>
#include <map>
#include "Server.hpp"
class Server;

struct Bodies
{
    std::string contentType;
    std::string contentLength;
    std::string body;
};

class Request
{
private:
    std::map<std::string, std::string> headers;
    std::vector<std::string> methods;
    std::map<std::string, std::string> startLine;
    std::map<int, std::string> mapTmp;
    std::string method; // GET POST DELETE
    std::string urlPath;
    std::string urlQuery; // .?.....
    std::string pVersion; // http/1.1
    std::string host;
    std::string connection;
    std::string contentType;
    std::string contentLength;
    std::string transferEncoding;
    std::string boundry;
    std::string body;
    std::vector<Bodies> bodies;
    std::string content;
    bool requestError;

public:
    Request();
	Request(const std::string & buffer);
    // Request(int &_newSockFd);
    Request(const Request &src);
    Request &operator=(const Request &rhs);
    ~Request();

    void parseRequest();
    void parseBody();
    void printRequest();
    void split(std::string, std::string);
    std::string reqErrorMsg(int &);
    

    std::string getMethod();
    std::string getUrlPath();
    std::string getUrlQuery();
    std::string getProtocol();
    std::string getHost();
    std::string getConection();
    std::string getContentType();
    std::string getContentLength();
    std::string getTransferEncoding();
    std::string getBoundry();
    std::vector<Bodies> getBody();
};

#endif