/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFileParser.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ichejra <ichejra@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/09 15:23:05 by ichejra           #+#    #+#             */
/*   Updated: 2021/09/13 16:31:45 by ichejra          ###   ########.fr       */
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
	std::vector<Location> locations;
	Location location;
	std::map<int, std::string> mapTmp;

public:
	ConfigFileParser();
	~ConfigFileParser();

	void parseConfigFile(int ac, char **av);
	std::string trimContent(std::string);
	void printContentData();
	void semiColonChecker(std::string &buffer);
	void checkFile(int ac, char **av);
	void checkUnit(std::string);
	void split(std::string line, char splitter);
	void checkMissingAttrs();
	void checkHost(std::string);
	void parseLocation(std::string);
};

#endif