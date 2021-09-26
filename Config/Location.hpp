/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ichejra <ichejra@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/09 16:02:14 by ichejra           #+#    #+#             */
/*   Updated: 2021/09/25 12:58:40 by ichejra          ###   ########.fr       */
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
	bool const &getAutoIndex() const;

	void setLocationName(std::string);
	std::string const &getLocationName() const;

	void setRoot(std::string);
	std::string const &getRoot() const;

	void setIndex(std::string);
	std::vector<std::string> const &getIndexes() const;

	void setAllowedMethods(std::string);
	std::vector<std::string> const &getAllowedMethods() const;

	void setReturn(int, std::string);
	std::map<int, std::string> const &getReturn() const;

	void setFastCgiPass(std::string);
	std::string const &getFastCgiPass() const;

	void setUploadEnable(std::string);
	bool const &getUploadEnable() const;

	void setUploadStore(std::string);
	std::string const &getUploadStore() const;
};

#endif