/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elahyani <elahyani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/01 13:44:07 by elahyani          #+#    #+#             */
/*   Updated: 2021/09/15 15:38:11 by elahyani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <sys/socket.h>
#include <iostream>
#include <sstream>
#include <cctype>
#include <vector>
#include <map>

struct Bodies
{
    std::string contentType;
    std::string body;
    std::string contentDesp;
};

class Request
{
private:
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> startLine;
    std::map<int, std::string> errors;
    std::map<int, std::string> mapTmp;
    std::vector<std::string> methods;
    std::vector<Bodies> bodiesList;
    std::string content;
    std::string method; // GET POST DELETE
    std::string urlPath;
    std::string urlQuery; // .?.....
    std::string protocol; // http/1.1
    int _newSockFd;
    int bLen;
    int statusCode;
    bool requestError;

public:
    Request();
    Request(const std::string &);
    Request(const Request &src);
    Request &operator=(const Request &rhs);
    ~Request();

    void readRequest();
    void parseRequest();
    void parseBody();
    int getBodiesLen(std::string);
    bool checkRequest(std::string &);
    void printRequest();
    void split(std::string, std::string);
    int checkReqErrors();

    std::vector<Bodies> getBody();
    std::string getHeaderVal(std::string const &key);
    std::string getStartLineVal(std::string const &key);
    int getStatusCode();
};

#endif

// curl -i -X POST -H "Content-Type: multipart/form-data" -F "data=@Makefile" http://localhost:5000/