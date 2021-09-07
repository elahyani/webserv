/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elahyani <elahyani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 14:49:51 by elahyani          #+#    #+#             */
/*   Updated: 2021/09/07 16:46:02 by elahyani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(Request &req) : _status(-1), _request(req), _responseMsg(""), _headers(""), _body("")
{
    this->errors[200] = "OK";
    this->errors[301] = "Moved Permanently";
    this->errors[307] = "Temporary Redirect";
    this->errors[400] = "Bad Request";
    this->errors[403] = "Forbidden";
    this->errors[404] = "Not Found";
    this->errors[405] = "Not Allowed";
    this->errors[411] = "Length Required";
    this->errors[413] = "Payload Too Large";
    this->errors[414] = "Uri Too Long";
    this->errors[415] = "Unsupported Media Type";
    this->errors[500] = "Internal Server Error";
    this->errors[501] = "Not Implemented";
    this->errors[502] = "Bad Gateway";
    this->errors[504] = "Gateway Timeout";
    this->errors[505] = "Http Version Not Supported";
    this->_status = this->_request.getStatusCode();

    std::cout << "status >>>>> " << this->_status << std::endl;
}

Response::~Response()
{
}

bool Response::checkErrors(int status)
{
    (void)status;
    return true;
}

void Response::buildHeaders()
{
    std::cout << "Protocol >>>> " << this->_request.getStartLineVal("protocol") << std::endl;
    std::cout << "Content-Type >>>> " << this->_request.getHeaderVal("Content-Type") << std::endl;
    std::cout << "Content-Length >>>> " << this->_request.getHeaderVal("Content-Length") << std::endl;
    std::cout << "Connection >>>> " << this->_request.getHeaderVal("Connection") << std::endl;
    this->_headers.append(this->_request.getStartLineVal("protocol"));
    this->_headers.append(" ");
    this->_headers.append(std::to_string(_status));
    this->_headers.append(" ");
    this->_headers.append(this->errors[_status]);
    this->_headers.append("\r\n");
    this->_headers.append("");
}

void Response::getMethod()
{
    std::cout << "GET METHOD" << std::endl;
}

void Response::postMethod()
{
    std::cout << "POST METHOD" << std::endl;
}

void Response::deleteMethod()
{
    std::cout << "DELETE METHOD" << std::endl;
}

void Response::buildResponse()
{
    // check for errors
    // get redirection
    if (_request.getStartLineVal("method").compare("GET") == 0)
        getMethod();
    else if (_request.getStartLineVal("method").compare("POST") == 0)
        postMethod();
    else if (_request.getStartLineVal("method").compare("DELETE") == 0)
        deleteMethod();
}

std::string Response::getErrorPage(int status)
{
    //examples
    std::string errorPage = "<!DOCTYPE html>\n\
    <html>\n\
        <head>\n\
        <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\
        <title>Bad Request</title>\n\
        <style>\n\
            .container{margin:0;top:0;}\n\
            .error{font-size: 2.5rem;text-align: center;margin-top: 40%;color:dodgerblue;}\n\
        </style>\n\
        </head>\n\
        <body>\n\
            <div class=\"container\">\n\
                <div class=\"error\">\n\
                    <h1>$1</h1>\n\
                    <h1>$2</h1>\n\
                </div>\n\
            </div>\n\
        </body>\n\
    </html>";

    errorPage.replace(errorPage.find("$1"), 2, std::to_string(status));
    errorPage.replace(errorPage.find("$2"), 2, errors[status]);
    return errorPage;
}

std::string Response::getResponse()
{
    return this->_responseMsg;
}

std::string Response::getHtmlTemplate()
{
    std::string htmlTemplate = "<!DOCTYPE html>\n\
    <html>\n\
        <head>\n\
        <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\
        <title>Bad Request</title>\n\
        <style>\n\
            .container{margin:0;top:0;}\n\
            .error{font-size: 2.5rem;text-align: center;margin-top: 40%;color:dodgerblue;}\n\
        </style>\n\
        </head>\n\
        <body>\n\
            <div class=\"container\">\n\
                <div class=\"error\">\n\
                    <h1>Welcome to WeBsErVeReReReReR</h1>\n\
                </div>\n\
            </div>\n\
        </body>\n\
    </html>";

    return htmlTemplate;
}