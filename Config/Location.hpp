/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ichejra <ichejra@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/09 16:02:14 by ichejra           #+#    #+#             */
/*   Updated: 2021/09/20 18:25:17 by ichejra          ###   ########.fr       */
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
	std::vector<std::string> indexes;
	std::vector<std::string> allowedMethods;
	std::map<int, std::string> ret;
	std::string fastCgiPass;
	bool uploadEnable;
	std::string uploadStore;

public:
	Location();
	~Location();
	Location(Location const &src);
	Location &operator=(Location const &src);

	void clearAll();

	void setAutoIndex(std::string);
	bool &getAutoIndex();

	void setLocationName(std::string);
	std::string &getLocationName();

	void setRoot(std::string);
	std::string &getRoot();

	void setIndex(std::string);
	std::vector<std::string> &getIndexes();

	void setAllowedMethods(std::string);
	std::vector<std::string> &getAllowedMethods();

	void setReturn(int, std::string);
	std::map<int, std::string> &getReturn();

	void setFastCgiPass(std::string);
	std::string &getFastCgiPass();

	void setUploadEnable(std::string);
	bool &getUploadEnable();

	void setUploadStore(std::string);
	std::string &getUploadStore();
};

#endif