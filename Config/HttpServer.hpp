/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpServer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ichejra <ichejra@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/09 15:23:05 by ichejra           #+#    #+#             */
/*   Updated: 2021/09/11 18:02:55 by ichejra          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPSERVER_HPP
#define HTTPSERVER_HPP

#include "Location.hpp"
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <cctype>

class HttpServer
{
private:
	std::vector<int> ports;
	std::string host;
	size_t clientMaxBodySize;
	std::map<int, std::string> errorsPages;
	std::string root;
	std::vector<std::string> serverName;
	int serversNumber;
	bool inServer;
	int errorCode;
	std::string errorPagePath;
	std::string filename;
	std::map<int, std::string> mapTmp;

public:
	HttpServer();
	~HttpServer();

	void parseConfigFile(int ac, char **av);
	std::string getPort();
	std::string getClientMaxBodySize();
	std::string getRoot();
	std::map<int, std::string> getErrorsPages();
	std::string trimContent(std::string);
	int getServersNumber();
	void printContentData();
	void semiColonChecker(std::string &buffer);
	void checkFile(int ac, char **av);
	void checkUnit(std::string);
	void split(std::string line, char splitter);
};

#endif