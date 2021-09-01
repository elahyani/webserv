/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elahyani <elahyani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/01 13:44:07 by elahyani          #+#    #+#             */
/*   Updated: 2021/09/01 14:48:30 by elahyani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>

struct StartLine
{
    std::string method; // GET POST DELETE
    std::string url;
    std::string pVersion; // http/1.1
};

struct HttpHeaders
{
    std::string host;
    std::string userAgent;
    std::string connection;
    std::string contentType;
    std::string contentLength;
};

class Request
{
private:
    StartLine firstLine;
    HttpHeaders headers;
    std::string body;

public:
    Request();
    Request(const Request &src);
    Request &operator=(const Request &rhs);
    ~Request();

    void parseRequest();
    void printRequest();
};

#endif