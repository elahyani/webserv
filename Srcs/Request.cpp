/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elahyani <elahyani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/01 16:54:02 by elahyani          #+#    #+#             */
/*   Updated: 2021/09/02 18:10:29 by elahyani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request() {}

Request::Request(std::string const &reqData)
{
    this->content = reqData;
    this->methods["m1"] = "GET";
    this->methods["m2"] = "POST";
    this->methods["m3"] = "DELETE";
}

Request::Request(const Request &src)
{
    (void)src;
}

Request &Request::operator=(const Request &rhs)
{
    (void)rhs;
    return *this;
}

Request::~Request() {}

void Request::parseRequest()
{
    std::string tmp;
    std::istringstream s(content);
    while (std::getline(s, tmp))
    {
        std::cout << tmp << std::endl;
        if (!this->method.size() && !this->pVersion.size())
        {
            this->tokenize(tmp, " ");
            this->method = this->mapTmp[0];
            this->urlPath = this->mapTmp[1];
            this->pVersion = this->mapTmp[2];
            for (std::map<std::string, std::string>::iterator it = methods.begin(); it != methods.end(); ++it)
            {
                if (it->second == this->method)
                    break;
                std::cout << "Bad Request!" << std::endl;
            }
        }
        if (tmp.find("Host") != std::string::npos)
            this->headers["Host"] = tmp.substr(tmp.find("Host") + 6);
        if (tmp.find("User-Agent") != std::string::npos)
            this->headers["User-Agent"] = tmp.substr(tmp.find("User-Agent") + 12);
        if (tmp.find("Connection") != std::string::npos)
            this->headers["Connection"] = tmp.substr(tmp.find("Connection") + 12);
        if (tmp.find("Content-Type") != std::string::npos)
            this->headers["Content-Type"] = tmp.substr(tmp.find("Content-Type") + 14);
        if (tmp.find("Content-Length") != std::string::npos)
            this->headers["Content-Length"] = tmp.substr(tmp.find("Content-Length") + 16);
        if (tmp.find("Transfer-Encoding") != std::string::npos)
            this->headers["Transfer-Encoding"] = tmp.substr(tmp.find("Transfer-Encoding") + 19);
    }
    this->startLine["method"] = method;
    this->startLine["url"] = urlPath;
    this->startLine["pVersion"] = pVersion;
}

void Request::printRequest()
{
    std::cout << "+++++++++++++++++++++++++++++++++++++" << std::endl;
    std::cout << "Method            -> " << this->startLine["method"] << std::endl;
    std::cout << "Url               -> " << this->startLine["url"] << std::endl;
    std::cout << "Protocol Version  -> " << this->startLine["pVersion"] << std::endl;
    std::cout << "Host              -> " << this->headers["Host"] << std::endl;
    std::cout << "User Agent        -> " << this->headers["User-Agent"] << std::endl;
    std::cout << "Connection        -> " << this->headers["Connection"] << std::endl;
    std::cout << "Content Type      -> " << this->headers["Content-Type"] << std::endl;
    std::cout << "Content Length    -> " << this->headers["Content-Length"] << std::endl;
    std::cout << "Transfer Encoding -> " << this->headers["Transfer-Encoding"] << std::endl;
    std::cout << "Boundry           -> " << boundry << std::endl;
    std::cout << "+++++++++++++++++++++++++++++++++++++" << std::endl;
}

void Request::tokenize(std::string line, std::string splitter)
{
    int i = 0;
    int start = 0;
    int end = line.find(splitter);

    while (end != -1)
    {
        this->mapTmp.insert(std::pair<int, std::string>(i, line.substr(start, end - start)));
        start = end + splitter.size();
        end = line.find(splitter, start);
        i++;
    }
    this->mapTmp.insert(std::pair<int, std::string>(i, line.substr(start, end - start)));
}