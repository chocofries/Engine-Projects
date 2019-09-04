/**************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2005 - 2018 Media Design School

File Name	:	netutils.h
Description	:	Miscellaneous utilities for networking
Author   	:	Zheng Yu Bu
mail		:	zheng.yu5315@mediadesign.school.nz
**************************************************/

#pragma once

#if !defined(__UTILS_H__)
#define __UTILS_H__

// Library Includes
#include <WS2tcpip.h>
#include <string>
#include <strstream>

// Local Includes

// Types

// Constants


// Prototypes

#define VALIDATE(a) if (!a) return (false)

namespace {
	std::string ToString(sockaddr_in _sockAddress)
	{
		//INET_ADDRSTRLEN - maximum length for IPv4 addresses
		char _pcAddress[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &_sockAddress.sin_addr, _pcAddress, INET_ADDRSTRLEN);

		std::string _strAddress = _pcAddress;
		std::string _strPort = std::to_string(ntohs(_sockAddress.sin_port));
		std::string _strAddressPort = _strAddress + ':' + _strPort;

		return _strAddressPort;
	}
}

template<typename T>
std::string ToString(const T& _value)
{
	std::strstream theStream;
	theStream << _value << std::ends;
	return (theStream.str());
}


#endif    // __UTILS_H__
