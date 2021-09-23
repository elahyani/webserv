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

Response::Response(Request &req, HttpServer &server /* , Server *serv */) : _status(-1),
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
                                                                            _notFound(false),
                                                                            _isLocation(false)
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

    // std::cout << "status >>>>> " << this->_status << std::endl;
}

Response::~Response()
{
}

void Response::getErrorPage(std::string ErrorPagePath)
{
    std::ifstream indexFile(ErrorPagePath);
    if (indexFile)
    {
        std::cout << "ERROR PAGE PATH >>> VALID" << std::endl;
        std::ostringstream ss;
        ss << indexFile.rdbuf();
        _body = ss.str();
    }
}

void Response::manageErrorHeaders(int _status)
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
    this->_headers.append("Connection: close\r\n");
    this->_headers.append("Content-Type: text/html; charset=UTF-8");
    this->_headers.append("\r\n");
    this->_headers.append("Content-Length: " + std::to_string(_body.length()));
    this->_headers.append("\r\n\r\n");
    this->_headers.append(_body);
}

void Response::manageReqErrors()
{
    if (_status == BAD_REQUEST_STATUS)
    {
        if (_server.getErrorsPages()[_status].length())
            getErrorPage(_server.getErrorsPages()[_status]);
        else
            getDefaultErrorPage(_status);
    }
    else if (_status == HTTP_VERSION_NOT_SUPPORTED_STATUS)
    {
        if (_server.getErrorsPages()[_status].length())
            getErrorPage(_server.getErrorsPages()[_status]);
        else
            getDefaultErrorPage(_status);
    }
    manageErrorHeaders(_status);
}

void Response::indexingFiles()
{
    std::cout << "------------------------------------------------------------" << std::endl;
    std::cout << getPath(_request.getStartLineVal("url")) << std::endl;
    std::cout << "------------------------------------------------------------" << std::endl;
    DIR *directory = opendir(getPath(_request.getStartLineVal("url")).c_str());
    struct dirent *en;
    std::string fileName;

    _autoIndexPage = "<!DOCTYPE html>\n<html lang=\"en\">\n\
    <head>\n\
        <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\
        <title>Bad Request</title>\n\
        </head>\n\
        <body>\n\
            <div style=\"margin-left: 5%; margin-top:10%;\">\n\
            <hr>\n";

    if (directory)
    {
        while ((en = readdir(directory)) != nullptr)
        {
            fileName = en->d_name;
            if (en->d_type == DT_DIR)
                fileName.append("/");
            _autoIndexPage.append("\t\t\t<p><a href=\"/" + fileName + "\">" + fileName + "</a></p>\n");
        }
        closedir(directory);
    }
    _autoIndexPage += "\
                <hr>\n\
            </div>\n\
        </body>\n\
    </html>\n";

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

// std::string Response::getDirectory()
// {
//     char buffer[1000];
//     std::string dirPath = "";

//     if (!getcwd(buffer, sizeof(buffer)))
//         throw std::invalid_argument("Exception: Path not found");
//     else
//     {
//         dirPath = buffer;
//         if (_location.getRoot().length())
//         {
//             if (_location.getRoot().find(dirPath) != std::string::npos || dirPath.find(_location.getRoot()) != std::string::npos)
//                 return _location.getRoot();
//             if (_location.getRoot().front() != '/')
//                 dirPath.append("/");
//             dirPath.append(_location.getRoot());
//         }
//         else
//         {
//             if (_server.getRoot().find(dirPath) != std::string::npos || dirPath.find(_server.getRoot()) != std::string::npos)
//                 return _server.getRoot();
//             if (_server.getRoot().front() != '/')
//                 dirPath.append("/");
//             dirPath.append(_server.getRoot());
//         }
//     }
//     std::cout << "DIR _> " << dirPath << std::endl;
//     return dirPath;
// }

bool Response::isDirectory(const std::string &path)
{
    std::string s = path;
    DIR *r;

    if ((r = opendir(s.c_str())))
    {
        closedir(r);
        return true;
    }
    return false;
}

std::string &Response::getHeaders()
{
    return this->_headers;
}

std::string Response::getDefaultErrorPage(int status)
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

void Response::clearAll()
{
    _status = 0;
    _server.clearAll();
    _request.clearRequest();
    _location.clearAll();
    _responseMsg.clear();
    _headers.clear();
    _body.clear();
    _indexPath.clear();
    _autoIndexPage.clear();
    _dirPath.clear();
    _dr.clear();
    _autoIndex = false;
    _notFound = false;
    _isLocation = false;
    _errors.clear();
    _dirContent.clear();
}

Location Response::getRedirection(std::string locName)
{
    for (size_t i = 0; i < _server.getLocations().size(); i++)
    {
        if (_location.getLocationName().compare(locName) == 0)
        {
            return _server.getLocations()[i];
            break;
        }
    }
    return _location;
}

Location Response::isLocationExist()
{
    // std::cout << "locName |" << _server.getLocations()[i].getLocationName() << "| - isCGI |" << _server.getLocations()[i].getIsCGI() << "|" << std::endl;
    for (size_t i = 0; i < _server.getLocations().size(); i++)
    {
        if (_request.getStartLineVal("url").find(_server.getLocations()[i].getLocationName()) != std::string::npos)
        {
            _location = _server.getLocations()[i];
            _isLocation = true;
        }
    }
    if (_location.getReturn().size())
    {
        if (_location.getReturn().begin()->first == MOVED_PERMANENTLY_STATUS)
        {
            _status = MOVED_PERMANENTLY_STATUS;
            _request.setHeaderVal("Connection", "close");
            _request.setStartLineVal("url", _location.getReturn().begin()->second);
            _redirectedLocation = _location.getReturn().begin()->second;
            _location.clearAll();
            return getRedirection(_location.getLocationName());
        }
    }
    return _location;
}

std::string Response::getUriFilePath(std::string uri)
{
    std::string path = uri;

    if (path.back() != '/')
        path.append("/");
    if ((!_location.getAutoIndex() && uri.compare(_location.getLocationName()) == 0) || path.compare(_location.getIndex()) == 0)
        return path.append(_location.getIndex());
    return path;
}

std::string Response::getRootDirectory()
{
    if (_location.getRoot().size())
        return _location.getRoot();
    return _server.getRoot();
}

std::string Response::getPath(std::string uriFilePath)
{
    std::string path = getRootDirectory();
    std::cout << "dir ___> |" << path << "|" << std::endl;
    if (uriFilePath.front() != '/')
        uriFilePath = "/" + uriFilePath;
    path.append(uriFilePath);
    return path;
}

void Response::getMethod()
{
    std::cout << "GET METHOD" << std::endl;
    std::string filrnamefromuri = getUriFilePath(_request.getStartLineVal("url"));
    std::string directoryPath = getPath(filrnamefromuri);
    std::cout << "uri_> |" << _request.getStartLineVal("url") << "|" << std::endl;
    std::cout << "filrnamefromuri _> |" << filrnamefromuri << "|" << std::endl;
    std::cout << "root _> |" << _server.getRoot() << "|" << std::endl;
    std::cout << "PATH _> |" << directoryPath << "|" << std::endl;

    if (isDirectory(directoryPath))
    {
        std::cout << "LOCATION >>> EXIST" << std::endl;
        if (_location.getAutoIndex())
            indexingFiles();
    }
    else if (filrnamefromuri.find(_location.getIndex()) != std::string::npos)
    {
        // read the file
        if (_location.getRoot().size() && _location.getIndex().size())
        {
            std::cout << "ROOT && INDEX >>> EXIST" << std::endl;
            std::ifstream indexFile(_location.getRoot() + "/" + _location.getIndex());
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
        else
        {
            _body = getHtmlTemplate();
            std::cout << "ACH HADA EMMM AHAAAA ACH HADAAAA" << std::endl;
        }
    }
    else
    {
        std::cout << "LOCATION >>> DOESN'T EXIST" << std::endl;
        _body = notFoundPage();
        _status = NOT_FOUND_STATUS;
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

void Response::buildHeaders()
{
    time_t rawTime;
    std::string tm;

    time(&rawTime);
    tm = ctime(&rawTime);
    tm.pop_back();
    this->_headers.append("HTTP/1.1");
    this->_headers.append(" ");
    this->_headers.append(std::to_string(_status));
    this->_headers.append(" ");
    this->_headers.append(this->_errors[_status]);
    this->_headers.append("\r\n");
    if (_status == MOVED_PERMANENTLY_STATUS)
    {
        this->_headers.append("Location: " + _redirectedLocation);
        this->_headers.append("\r\n");
        this->_headers.append("Connection: close");
        this->_headers.append("\r\n\r\n");
    }
    else
    {
        this->_headers.append("Server: webServ\r\n");
        this->_headers.append("Date: " + tm.append(" GMT"));
        this->_headers.append("\r\n");
        this->_headers.append("Connection: keep-alive");
        this->_headers.append("\r\n");
        this->_headers.append("Content-Type: text/html; charset=UTF-8");
        this->_headers.append("\r\n");
        this->_headers.append("Content-Length: " + std::to_string(_body.length()));
        this->_headers.append("\r\n\r\n");
        this->_headers.append(_body);
    }
}

void Response::generateResponse()
{
    _location = isLocationExist();
    if (_request.getStartLineVal("method").compare("GET") == 0)
        getMethod();
    else if (_request.getStartLineVal("method").compare("POST") == 0)
        postMethod();
    else if (_request.getStartLineVal("method").compare("DELETE") == 0)
        deleteMethod();
    buildHeaders();
}

void Response::buildResponse()
{
    // check for errors
    if (_status != OK_STATUS)
        manageReqErrors();
    else
        generateResponse();
}