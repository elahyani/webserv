/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elahyani <elahyani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 14:49:51 by elahyani          #+#    #+#             */
/*   Updated: 2021/09/06 17:34:50 by elahyani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(Request &req) : _status(-1), _request(req), _responseMsg("")
{
    errors[404] = "Not Found";
    //...
}

Response::~Response()
{
}

bool Response::checkErros(int status)
{
    (void)status;
    return true;
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
    return "";
}