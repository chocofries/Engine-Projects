#pragma once
#include <algorithm>

#define WINDOW_WIDTH 2560
#define WINDOW_HEIGHT 1440

#define MATH_PI 3.14159265359f

//namespace
//{
//	std::string ToString(sockaddr_in _sockAddress)
//	{
//		//INET_ADDRSTRLEN - maximum length for IPv4 addresses
//		char _pcAddress[INET_ADDRSTRLEN];
//		inet_ntop(AF_INET, &_sockAddress.sin_addr, _pcAddress, INET_ADDRSTRLEN);
//
//		std::string _strAddress = _pcAddress;
//		std::string _strPort = std::to_string(ntohs(_sockAddress.sin_port));
//		std::string _strAddressPort = _strAddress + ':' + _strPort;
//
//		return _strAddressPort;
//	}
//}
//
//template<typename T>
//std::string ToString(const T& _value)
//{
//	std::strstream theStream;
//	theStream << _value << std::ends;
//	return (theStream.str());
//}

class Utils
{
public:

	//Clamps the value between a minimum and a maximum range
	template <typename T>
	static T Clamp(const T& var, const T& min, const T& max)
	{
		return std::max(min, std::min(var, max));
	}

	//Wraps the value between a minimum and a maximum range
	template <typename T>
	static T Wrap(const T& var, const T& min, const T& max)
	{
		T result = 0;
		if (var > max)
		{
			result = min;
		}
		else if (var < min)
		{
			result = max;
		}
		else
		{
			return (var);
		}
		return result;
	}

	//Finds the X component of a vector
	template <typename T>
	static T LengthDirX(const T& length, const T& direction)
	{
		return (length*cos(DegToRad(direction)));
	}

	//Finds the Y component of a vector
	template <typename T>
	static T LengthDirY(const T& length, const T& direction)
	{
		return (length*sin(DegToRad(direction)));
	}

	//Convert Degrees to Radians
	template <typename T>
	static T DegToRad(const T& degrees)
	{
		return ((degrees * MATH_PI) / 180.0f);
	}

	//Convert Radians to Degrees
	template <typename T>
	static T RadToDeg(const T& radians)
	{
		return ((radians * 180) / MATH_PI);
	}
};