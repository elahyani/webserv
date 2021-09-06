/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elahyani <elahyani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/06 14:48:42 by elahyani          #+#    #+#             */
/*   Updated: 2021/09/06 17:35:19 by elahyani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Request.hpp"
class Request;

class Response
{
private:
    int _status;
    Request _request;
    std::string _responseMsg;
    std::string _body;
    std::string _headers;
    std::map<int, std::string> errors;

public:
    Response();
    Response(Request &req);
    ~Response();

    bool checkErros(int status);
    std::string getErrorPage(int status);
    void getMethod();
    void postMethod();
    void deleteMethod();
    void buildResponse();
    void buildHeaders();
    std::string getResponse();
    std::string getHtmlTemplate();
};

#endif