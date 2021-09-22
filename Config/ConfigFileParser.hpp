/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFileParser.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ichejra <ichejra@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/09 15:23:05 by ichejra           #+#    #+#             */
/*   Updated: 2021/09/22 12:27:59 by ichejra          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGFILEPARSER_HPP
#define CONFIGFILEPARSER_HPP

#include "Location.hpp"
#include "HttpServer.hpp"
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <cctype>
#include <sstream>

class ConfigFileParser
{
private:
	std::string data;
	int serversNumber;
	int locationsNumber;
	bool inServer;
	bool inLocation;
	std::string filename;
	std::vector<HttpServer> servers;
	HttpServer server;
	Location location;
	std::map<int, std::string> mapTmp;
	size_t countauto;
	size_t isEnabled;
	bool _isCGI;
	bool _isLoc;
	bool _isServ;

public:
	ConfigFileParser();
	~ConfigFileParser();
	ConfigFileParser(ConfigFileParser const &src);
	ConfigFileParser &operator=(ConfigFileParser const &src);

	void parseConfigFile(int ac, char **av);
	std::string trimContent(std::string);
	void printContentData();
	void syntaxChecker(std::string &buffer, int);
	void checkFile(int ac, char **av);
	void checkUnit(std::string);
	void split(std::string line, char splitter);
	void checkMissingAttrs();
	void checkHost(std::string);
	void parseLocation(std::string);
	void checkLocationName(std::string &buffer);
	void checkLocAttr(void);
	void chekDupServerName(void);
};

#endif