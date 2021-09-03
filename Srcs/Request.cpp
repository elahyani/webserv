/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elahyani <elahyani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/01 16:54:02 by elahyani          #+#    #+#             */
/*   Updated: 2021/09/03 19:04:34 by elahyani         ###   ########.fr       */
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
        // std::cout << "-> " << tmp << std::endl;
        if (!this->method.size() && !this->pVersion.size())
        {
            this->split(tmp, " ");
            this->method = this->mapTmp[0];
            this->urlPath = this->mapTmp[1];
            this->pVersion = this->mapTmp[2];
            for (size_t i = 0; i < this->method.size(); i++)
            {
                if (islower(this->method[i]))
                {
                    std::cout << "Bad Request: syntax error!" << std::endl;
                    this->requestError = true;
                }
            }
            for (std::map<std::string, std::string>::iterator it = methods.begin(); it != methods.end(); ++it)
            {
                if (it->second == this->method)
                    break;
                else
                {
                    std::cout << "Bad Request: method not allowed!" << std::endl;
                    this->requestError = true;
                }
            }
            if (this->urlPath.find("?") != std::string::npos)
            {
                this->urlQuery = this->urlPath.substr(this->urlPath.find("?") + 1);
                this->urlPath = this->urlPath.substr(0, this->urlPath.find("?"));
            }
            if (this->pVersion.find("HTTP/1.1") == std::string::npos)
            {
                std::cout << "|" << pVersion << "|" << std::endl;
                std::cout << "Bad Request: wrrong http version!" << std::endl;
                this->requestError = true;
            }
        }
        else if (!headers["Host"].size() && tmp.find("Host") != std::string::npos)
            headers["Host"] = tmp.substr(tmp.find(": ") + 2);
        else if (!headers["User-Agent"].size() && tmp.find("User-Agent") != std::string::npos)
            headers["User-Agent"] = tmp.substr(tmp.find(": ") + 2);
        else if (!headers["Connection"].size() && tmp.find("Connection") != std::string::npos)
            headers["Connection"] = tmp.substr(tmp.find(": ") + 2);
        else if (!headers["Content-Type"].size() && tmp.find("Content-Type") != std::string::npos)
            headers["Content-Type"] = tmp.substr(tmp.find(": ") + 2);
        else if (!headers["Content-Length"].size() && tmp.find("Content-Length") != std::string::npos)
            headers["Content-Length"] = std::stoi(tmp.substr(tmp.find(": ") + 2));
        else if (!headers["Transfer-Encoding"].size() && tmp.find("Transfer-Encoding") != std::string::npos)
            headers["Transfer-Encoding"] = tmp.substr(tmp.find(": ") + 2);
    }
    // parseBody();
    this->startLine["method"] = method;
    this->startLine["url"] = urlPath;
    this->startLine["pVersion"] = pVersion;
}

void Request::parseBody()
{
    // body parser
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

void Request::split(std::string line, std::string splitter)
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