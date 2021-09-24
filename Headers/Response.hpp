/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elahyani <elahyani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 14:48:42 by elahyani          #+#    #+#             */
/*   Updated: 2021/09/07 16:44:19 by elahyani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <ctime>
#include <sys/stat.h>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <string>
#include <dirent.h>
#include <unistd.h>
// #include "Server.hpp"
// class Server;
#include "Request.hpp"
#include "HttpServer.hpp"

#define OK_STATUS 200
#define MOVED_PERMANENTLY_STATUS 301
#define TEMPORARY_REDIRECT_STATUS 307
#define BAD_REQUEST_STATUS 400
#define FORBIDDEN_STATUS 403
#define NOT_FOUND_STATUS 404
#define NOT_ALLOWED_STATUS 405
#define LENGTH_REQUIRED_STATUS 411
#define PAYLOAD_TOO_LARGE_STATUS 413
#define URI_TOO_LONG_STATUS 414
#define UNSUPPORTED_MEDIA_TYPE_STATUS 415
#define INTERNAL_SERVER_ERROR_STATUS 500
#define NOT_IMPLEMENTED_STATUS 501
#define BAD_GATEWAY_STATUS 502
#define GATEWAY_TIMEOUT_STATUS 504
#define HTTP_VERSION_NOT_SUPPORTED_STATUS 505

class Response
{
private:
    int _status;
    Request &_request;
    HttpServer &_server;
    // Server *_serv;
    Location _location;
    std::string _responseMsg;
    std::string _headers;
    std::string _body;
    std::string _indexPath;
    std::string _autoIndexPage;
    std::string _dirPath;
    std::string _dr;
    bool _autoIndex;
    bool _notFound;
    bool _isLocation;
    std::map<int, std::string> _errors;
    std::vector<std::string> _dirContent;
    std::string _redirectedLocation;

public:
    Response();
    Response(Request &req, HttpServer &server /* , Server *serv */);
    ~Response();

    void buildHeaders();
    void getErrorPage(std::string);
    void manageErrors();                     //TODO ****
    void manageErrorHeaders(int _status);    // TODO 1
    void generateResponse();                 // TODO 2
    std::string getPath(std::string);        // TODO 3
    std::string getUriFilePath(std::string); // TODO 4
    std::string getRootDirectory();
    void readFile(std::string path);             // TODO 7
    void getMethod();                            //
    void postMethod();                           //
    void deleteMethod();                         //
    void buildResponse();                        //
    std::string getDefaultErrorPage(int status); //
    std::string &getResponse();
    std::string getHtmlTemplate(); //
    std::string &getHeaders();
    std::string notFoundPage();
    bool indexIsExist();
    bool autoIndex();
    void indexingFiles();
    Location isLocationExist();
    Location getRedirection(std::string locName);
    bool isDirectory(const std::string &); //TODO 5
    std::string getDirectory();            // TODO 6
    void clearAll();
    // std::string readFile
};

#endif