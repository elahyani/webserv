/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elahyani <elahyani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/01 13:44:07 by elahyani          #+#    #+#             */
/*   Updated: 2021/09/17 12:04:14 by asaadi           ###   ########.fr       */
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
    std::map<std::string, std::string> _headers;
    std::map<std::string, std::string> _startLine;
    std::map<int, std::string> _errors;
    std::map<int, std::string> _mapTmp;
    std::vector<std::string> _methods;
    std::vector<Bodies> _bodiesList;
    std::string _content;
    std::string _method; // GET POST DELETE
    std::string _urlPath;
    std::string _urlQuery; // .?.....
    std::string _protocol; // http/1.1
    int _bLen;
    int _statusCode;
    bool _requestError;

public:
    Request();
    // Request(const std::string &);
    Request(const Request &src);
    Request &operator=(const Request &rhs);
    ~Request();

    void setRequestData(const std::string &);
    void readRequest();
    void parseRequest();
    void parseBody();
    int getBodiesLen(std::string);
    bool checkRequest(std::string &);
    void printRequest();
    void split(std::string, std::string);
    int checkReqErrors();

    std::vector<Bodies> getBody();
    std::string &getHeaderVal(std::string const &key);
    std::string &getStartLineVal(std::string const &key);
    int &getStatusCode();
    void clearRequest();
};

#endif

// curl -i -X POST -H "Content-Type: multipart/form-data" -F "data=@Makefile" http://localhost:5000/
// curl --resolve example.com:5050:127.0.0.1 http://example.com:5050/