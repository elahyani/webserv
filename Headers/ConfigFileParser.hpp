/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFileParser.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asaadi <asaadi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/09 15:23:05 by ichejra           #+#    #+#             */
/*   Updated: 2021/10/05 16:24:48 by asaadi           ###   ########.fr       */
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
	std::string _data;
	size_t _serversNumber;
	size_t _locationsNumber;
	bool _inServer;
	bool _inLocation;
	std::string _filename;
	std::vector<HttpServer> _servers;
	HttpServer _server;
	Location _location;
	std::map<int, std::string> _mapTmp;
	size_t _countauto;
	size_t _isEnabled;
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
	void chekDupServerName();
	std::vector<HttpServer> &getServers();
};

#endif