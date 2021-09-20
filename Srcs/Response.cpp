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

Response::Response(Request &req, HttpServer &server) : _status(-1),
                                                       _request(req),
                                                       _server(server),
                                                       _responseMsg(""),
                                                       _headers(""),
                                                       _body(""),
                                                       _indexPath(""),
                                                       _autoIndexPage(""),
                                                       _dirPath(""),
                                                       _dr(""),
                                                       _autoIndex(false),
                                                       _notFound(false)
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

void Response::checkErrors()
{
    // config errors
}

bool Response::getErrorMsg(int status)
{
    (void)status;
    return true;
}

void Response::indexingFiles()
{
    // auto index
    // listing links to files
    std::cout << "±±±±±±±±±±±±±±±±±±±>>> " << _dr << std::endl;
    DIR *directory = opendir(_dr.c_str());
    struct dirent *en;

    _dirContent.clear();
    if (directory)
    {
        while ((en = readdir(directory)) != nullptr)
            _dirContent.push_back(en->d_name);
        closedir(directory);
    }
    _autoIndexPage = "<!DOCTYPE html>\n<html lang=\"en\">\n\
    \t<head>\n\
    \t\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\
    \t\t<title>Bad Request</title>\n\
    \t\t<style>.container {margin-left: 5%; margin-top:10%;}</style>\n\
    \t</head>\n\
    \t<body>\n\
    \t\t<div class=\"container\">\n\t\t\t<hr>\n";

    for (size_t i = 0; i < _dirContent.size(); i++)
    {
        _autoIndexPage += "\t\t\t<a href=\"/" + _dirContent[i] + "\">" + _dirContent[i] + "</a><br>\n";
    }

    _autoIndexPage += "\t\t\t<hr>\n\t\t</div>\n\t</body>\n</html>";
    std::cout << "+++++++++++++++++++++++++++++++++++++++++---------------------" << std::endl;
    std::cout << _autoIndexPage << std::endl;
    _body = _autoIndexPage;
    std::cout << "+++++++++++++++++++++++++++++++++++++++++---------------------" << std::endl;
}

std::string Response::notFoundPage()
{
    std::string notFoundPage = "<!DOCTYPE html>\n\
    <html lang=\"en\">\n\
    <head>\n\
        <meta charset=\"UTF-8\" />\n\
        <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\" />\n\
        <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" />\n\
        <title>Document</title>\n\
        <style>\n\
        .container {margin: 10%;text-align: center;color: rgba(52, 151, 250, 0.705);}\n\
        h1 {font-size: 14rem;font-family: \"Courier New\", Courier, monospace;font-weight: bold;margin:-5rem 0 0 0;}\n\
        .parag {margin:0;font-weight: bold;font-size: 5rem;font-family: \"Courier New\", Courier, monospace;}\n\
        </style>\n\
    </head>\n\
    <body>\n\
        <div class=\"container\">\n\
        <h1>404</h1>\n\
        <p class=\"parag\">Page Not Found</p>\n\
        </div>\n\
    </body>\n\
    </html>\n";

    return notFoundPage;
}

bool Response::isLocationExist()
{
    std::string locPath;

    for (std::vector<Location>::iterator it = _server.getLoactions().begin(); it != _server.getLoactions().end(); ++it)
    {
        if (it->getLocationName().compare(_request.getStartLineVal("url")) == 0)
        {
            _location = *it;
            std::cout << "1‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡>>> " << _location.getLocationName() << std::endl;
            return true;
        }
        else if (_request.getStartLineVal("url").find(it->getLocationName()) != std::string::npos)
        {
            _location = *it;
            std::cout << "2‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡‡>>> " << _location.getLocationName() << std::endl;
            return false;
        }
    }
    return false;
}

bool Response::isDirectory()
{
    DIR *r;

    size_t locLen = _location.getLocationName().size();

    std::cout << "url --->>> |" << _request.getStartLineVal("url") << "|" << std::endl;
    if (_request.getStartLineVal("url").find(_location.getLocationName()) != std::string::npos)
    {
        std::string locPath = _request.getStartLineVal("url").substr(0, locLen);
        std::cout << "LocPath: |" << locPath << "| locName: |" << _location.getLocationName() << "|" << std::endl;
        if (_request.getStartLineVal("url").size() > 1 && locPath.compare(_location.getLocationName()) == 0)
            _dirPath.append(_server.getRoot() + "/" + _request.getStartLineVal("url").substr(locLen));
        std::cout << "dirPath—————————————————————————————————————————————————————>>> |" << _dirPath << "|" << std::endl;
    }
    if ((r = opendir(_dirPath.c_str())))
    {
        closedir(r);
        return true;
    }
    return false;
}

// bool Response::indexIsExist()
// {
//     // check if index and root exist [v]
//     // handle location content [v]

//     for (std::vector<Location>::iterator it = _server.getLoactions().begin(); it != _server.getLoactions().end(); ++it)
//     {
//         if (it->getLocationName().compare(_request.getStartLineVal("url")) == 0)
//         {
//             if (it->getAutoIndex())
//             {
//                 _autoIndex = true;
//             }
//             if ((_index = std::find(it->getIndexes().begin(), it->getIndexes().end(), "index.html")) != it->getIndexes().end())
//             {
//                 _indexPath.append(_server.getRoot());
//                 return true;
//             }
//         }
//     }
//     _notFound = true;
//     return false;
// }

bool Response::autoIndex()
{
    for (std::vector<Location>::iterator it = _server.getLoactions().begin(); it != _server.getLoactions().end(); ++it)
    {
        if (it->getLocationName().compare("/") == 0)
        {
            if (it->getAutoIndex())
                return true;
        }
    }
    return false;
}

void Response::buildHeaders()
{
    time_t rawTime;
    std::string tm;
    std::cout << "--------------------------------------------------------------> HERE" << std::endl;

    time(&rawTime);
    tm = ctime(&rawTime);
    tm.pop_back();
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
    this->_headers.append("Content-Length: " + std::to_string(_body.length()));

    // this->_headers.append("\r\n");
    // this->_headers.append("Content-Type: " + this->_request.getHeaderVal("Content-Type"));
    // this->_headers.append("Content-Type: " + this->_request.getHeaderVal("Content-Type"));
    // this->_headers.append("Content-Type: " + this->_request.getHeaderVal("Content-Type"));
    this->_headers.append("\r\n\r\n");
    this->_headers.append(_body);

    std::cout << "------------------[RESPONSE]------------------" << std::endl;
    // std::cout << _headers << std::endl;
}

std::string &Response::getHeaders()
{
    return this->_headers;
}

void Response::getMethod()
{
    std::cout << "GET METHOD" << std::endl;
    bool isLoc = isLocationExist();
    bool isDir = isDirectory();
    // is LocationExist || isDir(url)
    std::cout << "isLoc ±±±±±>> " << isLoc << " | isDir ±±±±±±>>" << isDir << std::endl;
    if (isLoc || isDir)
    {
        std::cout << "LOCATION >>> EXIST" << std::endl;
        if (_location.getAutoIndex() || isDir)
        {
            if (isDir)
                _dr = _dirPath;
            else if (_location.getAutoIndex())
                _dr = _server.getRoot();
            std::cout << ".." << _dr << std::endl;
            indexingFiles();
            std::cout << ".." << _dr << std::endl;
        }
        else if (_location.getRoot().size() && (_index = std::find(_location.getIndexes().begin(), _location.getIndexes().end(), "index.html")) != _location.getIndexes().end())
        {
            std::cout << "ROOT && INDEX >>> EXIST" << std::endl;
            std::ifstream indexFile(_indexPath.append("/" + *_index));
            if (indexFile)
            {
                std::cout << "INDEX PATH >>> VALID" << std::endl;
                std::ostringstream ss;
                ss << indexFile.rdbuf();
                _body = ss.str();
            }
            else
            {
                std::cout << "INDEX PATH >>> INVALID" << std::endl;
                _body = getHtmlTemplate();
            }
        }
    }
    else
    {
        std::cout << "LOCATION >>> DOESN'T EXIST" << std::endl;
        _body = notFoundPage();
        _status = 404;
    }
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
    buildHeaders();
    _request.clearRequest();
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
        <title>WEBSERV</title>\n\
        <style>\n\
            .container{margin:0;top:0;}\n\
            .error{font-size: 2.5rem;text-align: center;margin-top: 10%;color:dodgerblue;}\n\
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
