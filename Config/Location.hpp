/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ichejra <ichejra@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/09 16:02:14 by ichejra           #+#    #+#             */
/*   Updated: 2021/09/13 17:31:17 by ichejra          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>

class Location
{
private:
	bool autoindex;
	std::string locationName;
	std::string root;
	std::string index;
	// std::vector<std::string> allowedMethods;
	std::string allowedMethods;
	// std::map<int, std::string> ret;
	std::string ret;
	std::string fastCgiPass;
	bool uploadEnable;
	std::string uploadStore;

public:
	Location();
	~Location();

	void clearAll();

	void setAutoIndex(std::string);
	bool getAutoIndex();

	void setLocationName(std::string);
	std::string getLocationName();

	void setRoot(std::string);
	std::string getRoot();

	void setIndex(std::string);
	std::string getIndex();

	void setAllowedMethods(std::string);
	std::string getAllowedMethods();

	void setReturn(int, std::string);
	std::string getReturn();

	void setFastCgiPass(std::string);
	std::string getFastCgiPass();

	void setUploadEnable(std::string);
	bool getUploadEnable();

	void setUploadStore(std::string);
	std::string getUploadStore();
};

#endif