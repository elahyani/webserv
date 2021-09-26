/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpServer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ichejra <ichejra@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/13 11:22:44 by ichejra           #+#    #+#             */
/*   Updated: 2021/09/25 12:51:20 by ichejra          ###   ########.fr       */
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
	HttpServer(HttpServer const &src);
	HttpServer &operator=(HttpServer const &src);

	void setPorts(int port);
	std::vector<int> const &getPorts() const;

	void setHost(std::string);
	std::string const &getHost() const;

	void setClientMaxBodySize(size_t);
	size_t const &getClientMaxBodySize() const;

	void setErrorsPages(int, std::string);
	std::map<int, std::string> const &getErrorsPages() const;

	void setRoot(std::string);
	std::string const &getRoot() const;

	void setServerName(std::string);
	std::vector<std::string> const &getServerName() const;

	void setErrorCode(int);
	int const &getErrorCode() const;

	void setErrorPagePath(std::string);
	std::string const &getErrorPagePath() const;

	void setLocation(Location const &);
	std::vector<Location> const &getLoactions() const;

	void clearAll();
};

#endif