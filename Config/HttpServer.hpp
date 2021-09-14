/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpServer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ichejra <ichejra@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/13 11:22:44 by ichejra           #+#    #+#             */
/*   Updated: 2021/09/14 13:05:14 by ichejra          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPSERVER_HPP
#define HTTPSERVER_HPP

#include "Location.hpp"

class HttpServer
{
private:
	std::vector<int> ports;
	std::string host;
	size_t clientMaxBodySize;
	std::map<int, std::string> errorsPages;
	std::string root;
	std::vector<std::string> serverName;
	int errorCode;
	std::string errorPagePath;
	int locationsNumber;
	std::vector<Location> locations;

public:
	HttpServer();
	~HttpServer();

	void setPorts(int port);
	std::vector<int> &getPorts();

	void setHost(std::string);
	std::string &getHost();

	void setClientMaxBodySize(size_t);
	size_t &getClientMaxBodySize();

	void setErrorsPages(int, std::string);
	std::map<int, std::string> &getErrorsPages();

	void setRoot(std::string);
	std::string &getRoot();

	void setServerName(std::string);
	std::vector<std::string> &getServerName();

	void setErrorCode(int);
	int &getErrorCode();

	void setErrorPagePath(std::string);
	std::string &getErrorPagePath();

	void setLocation(Location const &);
	std::vector<Location> &getLoactions();

	void clearAll();
};

#endif