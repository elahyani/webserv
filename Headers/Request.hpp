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
    std::string contentDesp;
    std::string body;
};

class Request
{
private:
    // HttpServer _serverData;
    std::map<std::string, std::string> _headers;
    std::map<std::string, std::string> _startLine;
    std::map<int, std::string> _errors;
    std::map<int, std::string> _mapTmp;
    std::vector<Bodies> _bodiesList;
    std::vector<std::string> _cookies;
    std::string _content;
    std::string _method; // GET POST DELETE
    std::string _uriPath;
    std::string _urlQuery; // .?.....
    std::string _protocol; // http/1.1
    std::string _scriptName;
    std::string _body;
    int _bLen;
    int _statusCode;
    bool _requestError;
    int _maxBodySize;

public:
    Request();
    // Request(const std::string &);
    Request(const Request &src);
    Request &operator=(const Request &rhs);
    ~Request();

    void setRequestData(const std::string &, int &);
    void readRequest();
    void parseRequest();
    void parseBody();
    int getBodiesLen(std::string);
    bool checkRequest(std::string &);
    void printRequest();
    void split(std::string, char);
    int checkReqErrors();
    void setReqBody(std::string const &);
    std::string &getReqBody();

    std::vector<Bodies> getBody();

    void setHeaderVal(std::string, std::string);
    std::string &getHeaderVal(std::string const &key);

    void setStartLineVal(std::string, std::string);
    std::string &getStartLineVal(std::string const &key);
    int &getStatusCode();
    void clearRequest();
};

#endif

// curl -i -X POST -H "Content-Type: multipart/form-data" -F "data=@Makefile" http://localhost:5000/
// curl --resolve example.com:5050:127.0.0.1 http://example.com:5050/