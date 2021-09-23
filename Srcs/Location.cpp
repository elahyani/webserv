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
					   _isCGI(false),
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
		this->_index = src._index;
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

bool const &Location::getAutoIndex() const
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

void Location::setIndex(std::string index)
{
	this->_index = index;
}

std::string &Location::getIndex()
{
	return this->_index;
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
	this->_fastCgiPass = _cgiPass;
}

void Location::setIsCGI(bool const &iscgi)
{
	_isCGI = iscgi;
}

const bool &Location::isCGI() const
{
	return this->_isCGI;
}

std::string &Location::getFastCgiPass()
{
	return this->_fastCgiPass;
}

void Location::setUploadEnable(std::string uploadEnable)
{
	if (uploadEnable.compare("off") == 0)
		this->_uploadEnable = false;
	else if (uploadEnable.compare("on") == 0)
		this->_uploadEnable = true;
}

const bool &Location::getUploadEnable() const
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
	this->_index.clear();
	this->_fastCgiPass.clear();
	this->_isCGI = false;
	this->_uploadEnable = false;
	this->_uploadStore.clear();
}