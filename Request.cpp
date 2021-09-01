/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: elahyani <elahyani@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/01 16:54:02 by elahyani          #+#    #+#             */
/*   Updated: 2021/09/01 17:08:53 by elahyani         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request() {}

Request::Request(std::string const &reqData)
{
    std::cout << "-------------" << std::endl;
    std::cout << reqData << std::endl;
    std::cout << "-------------" << std::endl;
}

Request::Request(const Request &src)
{
    (void)src;
}

Request &Request::operator=(const Request &rhs)
{
    (void)rhs;
    return *this;
}

Request::~Request() {}