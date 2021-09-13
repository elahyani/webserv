/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ichejra <ichejra@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/12 15:23:32 by ichejra           #+#    #+#             */
/*   Updated: 2021/09/13 17:36:51 by ichejra          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location() : autoindex(false),
					   locationName(),
					   root(),
					   index(),
					   fastCgiPass(),
					   uploadEnable(false),
					   uploadStore()
{
}

Location::~Location()
{
}

void Location::setAutoIndex(std::string index)
{
	index.pop_back();
	if (index.compare("off") == 0)
		this->autoindex = false;
	else if (index.compare("on") == 0)
		this->autoindex = true;
}

bool Location::getAutoIndex()
{
	return this->autoindex;
}

void Location::setLocationName(std::string locName)
{
	this->locationName = locName;
}
std::string Location::getLocationName()
{
	return this->locationName;
}

void Location::setRoot(std::string _root)
{
	this->root = _root;
}

std::string Location::getRoot()
{
	return this->root;
}

void Location::setIndex(std::string _index)
{
	this->index = _index;
}

std::string Location::getIndex()
{
	return this->index;
}

void Location::setAllowedMethods(std::string _allowedMethods)
{
	this->allowedMethods = _allowedMethods;
}

std::string Location::getAllowedMethods()
{
	return this->allowedMethods;
}

void Location::setReturn(int code, std::string _ret)
{
	(void)code;
	this->ret = _ret;
}

std::string Location::getReturn()
{
	return this->ret;
}

void Location::setFastCgiPass(std::string _cgiPass)
{
	this->fastCgiPass = _cgiPass;
}

std::string Location::getFastCgiPass()
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

bool Location::getUploadEnable()
{
	return this->uploadEnable;
}

void Location::setUploadStore(std::string _uploadStore)
{
	this->uploadStore = _uploadStore;
}

std::string Location::getUploadStore()
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
	this->index.clear();
	this->fastCgiPass.clear();
	this->uploadEnable = false;
	this->uploadStore.clear();
}