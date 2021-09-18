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
    this->_errors[200] = "OK";
    this->_errors[301] = "Moved Permanently";
    this->_errors[307] = "Temporary Redirect";
    this->_errors[400] = "Bad Request";
    this->_errors[403] = "Forbidden";
    this->_errors[404] = "Not Found";
    this->_errors[405] = "Not Allowed";
    this->_errors[411] = "Length Required";
    this->_errors[413] = "Payload Too Large";
    this->_errors[414] = "Uri Too Long";
    this->_errors[415] = "Unsupported Media Type";
    this->_errors[500] = "Internal Server Error";
    this->_errors[501] = "Not Implemented";
    this->_errors[502] = "Bad Gateway";
    this->_errors[504] = "Gateway Timeout";
    this->_errors[505] = "Http Version Not Supported";
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
    time_t rawTime;

    time(&rawTime);
    std::string tm = ctime(&rawTime);

    // struct stat st;
    // stat(fileName, &st);
    // std::fstream fdRes;
    // fdRes.open(fileName, std::ios::in);
    // if (!fdRes)
    //     throw std::runtime_error("Unable to open response file.");
    // char *_buffRes = new char[st.st_size + 1];
    // fdRes.read(_buffRes, st.st_size);
    // fdRes.close();

    // applyMethod();

    tm.pop_back();
    std::cout << "Protocol >>>> " << this->_request.getStartLineVal("protocol") << std::endl;
    std::cout << "Content-Type >>>> " << this->_request.getHeaderVal("Content-Type") << std::endl;
    std::cout << "Content-Length >>>> " << this->_request.getHeaderVal("Content-Length") << std::endl;
    std::cout << "Connection >>>> " << this->_request.getHeaderVal("Connection") << std::endl;
    this->_headers.append(this->_request.getStartLineVal("protocol"));
    this->_headers.append(" ");
    this->_headers.append(std::to_string(_status));
    this->_headers.append(" ");
    this->_headers.append(this->_errors[_status]);
    this->_headers.append("\r\n");
    this->_headers.append("Server: webServ\r\n");
    this->_headers.append("Date: " + tm.append(" GMT"));
    this->_headers.append("\r\n");
    this->_headers.append("Connection: " + this->_request.getHeaderVal("Connection"));
    this->_headers.append("\r\n");
    this->_headers.append("Content-Type: text/html; charset=UTF-8");
    this->_headers.append("\r\n");
    std::string body = getHtmlTemplate();
    this->_headers.append("Content-Length: " + std::to_string(body.length()));

    // this->_headers.append("\r\n");
    // this->_headers.append("Content-Type: " + this->_request.getHeaderVal("Content-Type"));
    // this->_headers.append("Content-Type: " + this->_request.getHeaderVal("Content-Type"));
    // this->_headers.append("Content-Type: " + this->_request.getHeaderVal("Content-Type"));
    this->_headers.append("\r\n\r\n");
    this->_headers.append(body);

    std::cout << "------------------[RESPONSE]------------------" << std::endl;
    // std::cout << _headers << std::endl;
}

std::string &Response::getHeaders()
{
    return this->_headers;
}

void Response::getMethod()
{
    // if index exist
    // body = readFile(apth_to_file);
    // else
    // body = getTwmplateHtml()
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
    // buildHeaders();
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
    errorPage.replace(errorPage.find("$2"), 2, _errors[status]);
    return errorPage;
}

std::string &Response::getResponse()
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
