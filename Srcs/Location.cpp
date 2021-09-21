/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ichejra <ichejra@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/12 15:23:32 by ichejra           #+#    #+#             */
/*   Updated: 2021/09/15 13:13:50 by ichejra          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location() : _autoindex(false),
					   _locationName(),
					   _root(),
					   _fastCgiPass(),
					   _uploadEnable(false),
					   _uploadStore()
{
}

Location::~Location()
{
	this->clearAll();
}

Location::Location(Location const &src)
{
	*this = src;
}

Location &Location::operator=(Location const &src)
{
	//! use assign
	if (this != &src)
	{
		this->_autoindex = src._autoindex;
		this->_locationName = src._locationName;
		this->_root = src._root;
		this->_indexes.assign(src._indexes.begin(), src._indexes.end());
		this->_allowedMethods = src._allowedMethods;
		this->_ret = src._ret;
		this->_fastCgiPass = src._fastCgiPass;
		this->_uploadEnable = src._uploadEnable;
		this->_uploadStore = src._uploadStore;
	}
	return *this;
}

void Location::setAutoIndex(std::string index)
{
	if (index.compare("off") == 0)
		this->_autoindex = false;
	else if (index.compare("on") == 0)
		this->_autoindex = true;
}

bool &Location::getAutoIndex()
{
	return this->_autoindex;
}

void Location::setLocationName(std::string locName)
{
	this->_locationName = locName;
}
std::string &Location::getLocationName()
{
	return this->_locationName;
}

void Location::setRoot(std::string _root)
{
	this->_root = _root;
}

std::string &Location::getRoot()
{
	return this->_root;
}

void Location::setIndex(std::string _index)
{
	this->_indexes.push_back(_index);
}

std::vector<std::string> &Location::getIndexes()
{
	return this->_indexes;
}

void Location::setAllowedMethods(std::string _allowedMethods)
{
	this->_allowedMethods.push_back(_allowedMethods);
}

std::vector<std::string> &Location::getAllowedMethods()
{
	return this->_allowedMethods;
}

void Location::setReturn(int code, std::string _ret)
{
	this->_ret.insert(std::pair<int, std::string>(code, _ret));
}

std::map<int, std::string> &Location::getReturn()
{
	return this->_ret;
}

void Location::setFastCgiPass(std::string _cgiPass)
{
	_cgiPass.pop_back();
	this->_fastCgiPass = _cgiPass;
}

std::string &Location::getFastCgiPass()
{
	return this->_fastCgiPass;
}

void Location::setUploadEnable(std::string _uploadEnable)
{
	_uploadEnable.pop_back();
	if (_uploadEnable.compare("off") == 0)
		this->_uploadEnable = false;
	else if (_uploadEnable.compare("on") == 0)
		this->_uploadEnable = true;
}

bool &Location::getUploadEnable()
{
	return this->_uploadEnable;
}

void Location::setUploadStore(std::string _uploadStore)
{
	_uploadStore.pop_back();
	this->_uploadStore = _uploadStore;
}

std::string &Location::getUploadStore()
{
	return this->_uploadStore;
}

void Location::clearAll()
{
	this->_autoindex = false;
	this->_ret.clear();
	this->_allowedMethods.clear();
	this->_locationName.clear();
	this->_root.clear();
	this->_indexes.clear();
	this->_fastCgiPass.clear();
	this->_uploadEnable = false;
	this->_uploadStore.clear();
}