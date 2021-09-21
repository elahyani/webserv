/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ichejra <ichejra@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/12 15:23:32 by ichejra           #+#    #+#             */
/*   Updated: 2021/09/21 10:02:52 by ichejra          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location() : autoindex(false),
					   locationName(),
					   root(),
					   fastCgiPass(),
					   uploadEnable(false),
					   uploadStore()
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
		this->autoindex = src.autoindex;
		this->locationName = src.locationName;
		this->root = src.root;
		this->indexes.assign(src.indexes.begin(), src.indexes.end());
		this->allowedMethods = src.allowedMethods;
		this->ret = src.ret;
		this->fastCgiPass = src.fastCgiPass;
		this->uploadEnable = src.uploadEnable;
		this->uploadStore = src.uploadStore;
	}
	return *this;
}

void Location::setAutoIndex(std::string index)
{
	if (index.compare("off") == 0)
		this->autoindex = false;
	else if (index.compare("on") == 0)
		this->autoindex = true;
}

bool &Location::getAutoIndex()
{
	return this->autoindex;
}

void Location::setLocationName(std::string locName)
{
	this->locationName = locName;
}
std::string &Location::getLocationName()
{
	return this->locationName;
}

void Location::setRoot(std::string _root)
{
	_root.pop_back();
	this->root = _root;
}

std::string &Location::getRoot()
{
	return this->root;
}

void Location::setIndex(std::string _index)
{
	this->indexes.push_back(_index);
}

std::vector<std::string> &Location::getIndexes()
{
	return this->indexes;
}

void Location::setAllowedMethods(std::string _allowedMethods)
{
	this->allowedMethods.push_back(_allowedMethods);
}

std::vector<std::string> &Location::getAllowedMethods()
{
	return this->allowedMethods;
}

void Location::setReturn(int code, std::string _ret)
{
	if (code <= 0)
		throw std::invalid_argument("Exception\tReturn code must be a non-zero positive number");
	this->ret.insert(std::pair<int, std::string>(code, _ret));
}

std::map<int, std::string> &Location::getReturn()
{
	return this->ret;
}

void Location::setFastCgiPass(std::string _cgiPass)
{
	_cgiPass.pop_back();
	this->fastCgiPass = _cgiPass;
}

std::string &Location::getFastCgiPass()
{
	return this->fastCgiPass;
}

void Location::setUploadEnable(std::string _uploadEnable)
{
	_uploadEnable.pop_back();
	if (_uploadEnable.compare("off") == 0)
		this->uploadEnable = false;
	else if (_uploadEnable.compare("on") == 0)
		this->uploadEnable = true;
}

bool &Location::getUploadEnable()
{
	return this->uploadEnable;
}

void Location::setUploadStore(std::string _uploadStore)
{
	_uploadStore.pop_back();
	this->uploadStore = _uploadStore;
}

std::string &Location::getUploadStore()
{
	return this->uploadStore;
}

void Location::clearAll()
{
	this->autoindex = false;
	this->ret.clear();
	this->allowedMethods.clear();
	this->locationName.clear();
	this->root.clear();
	this->indexes.clear();
	this->fastCgiPass.clear();
	this->uploadEnable = false;
	this->uploadStore.clear();
}