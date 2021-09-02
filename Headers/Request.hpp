/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elahyani <elahyani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/01 13:44:07 by elahyani          #+#    #+#             */
/*   Updated: 2021/09/02 16:02:59 by elahyani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <sstream>
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
    std::map<std::string, std::string> methods;
    std::map<std::string, std::string> startLine;
    std::map<int, std::string> mapTmp;
    std::string method; // GET POST DELETE
    std::string urlPath;
    std::string pVersion; // http/1.1
    std::string host;
    std::string userAgent;
    std::string connection;
    std::string contentType;
    std::string contentLength;
    std::string transferEncoding;
    std::string boundry;
    std::string body;
    std::vector<Bodies> bodies;
    std::string content;

public:
    Request();
    Request(std::string const &reqData);
    Request(const Request &src);
    Request &operator=(const Request &rhs);
    ~Request();

    void parseRequest();
    void printRequest();
    void tokenize(std::string, std::string);
};

#endif