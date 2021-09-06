/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elahyani <elahyani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/01 13:44:07 by elahyani          #+#    #+#             */
/*   Updated: 2021/09/06 14:46:42 by elahyani         ###   ########.fr       */
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
    std::string contentDesp;
};

class Request
{
private:
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> startLine;
    std::map<int, std::string> mapTmp;
    std::vector<std::string> methods;
    std::vector<Bodies> bodies;
    std::string content;
    std::string method; // GET POST DELETE
    std::string urlPath;
    std::string urlQuery; // .?.....
    std::string pVersion; // http/1.1
    int _newSockFd;
    bool requestError;

public:
    Request(int);
    Request(const Request &src);
    Request &operator=(const Request &rhs);
    ~Request();

    void readRequest();
    void parseRequest();
    void parseBody();
    bool checkRequest(std::string &);
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
    std::string getHeaderVal(std::string key);
    std::string getStartLineVal(std::string key);
};

#endif

// curl -i -X POST -H "Content-Type: multipart/form-data" -F "data=@Makefile" http://localhost:5000/