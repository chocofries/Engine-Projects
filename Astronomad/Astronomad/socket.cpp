/**************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2005 - 2018 Media Design School

File Name	:	socket.cpp
Description	:	Class for socketing operations
Author   	:	Zheng Yu Bu
mail		:	zheng.yu5315@mediadesign.school.nz
**************************************************/

//Library Inludes
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <windows.h>
#include <iostream>

//Local Includes 
//#include "network.h"

//This Include
#include "socket.h"


Socket::Socket()
	:handleSocket(0)
{
	ZeroMemory(&socketAddress, sizeof(socketAddress));
}

Socket::~Socket()
{
	closesocket(handleSocket);
}

//Implicit Assumption: We are creating only UDP sockets.....
bool Socket::Initialise(unsigned short _usPortNumber)
{
	int _iError;
	//Create a UDP socket 
	handleSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == handleSocket)
	{
		_iError = WSAGetLastError();
		//ErrorRoutines::PrintWSAErrorInfo(_iError);
		std::cout << "Unable to create socket\n";
		return false;
	}
	else
	{
		std::cout << "Successfully created the socket" << std::endl;
	}

	// now bind the socket to the address and port number supplied.
	socketAddress.sin_family = AF_INET;
	socketAddress.sin_port = htons(_usPortNumber);
	socketAddress.sin_addr.S_un.S_addr = INADDR_ANY;
	//Qs 1 : Change if to while; incrementing the port number every time!
	while (0 != bind(handleSocket, reinterpret_cast<sockaddr*>(&socketAddress), sizeof(socketAddress)))
	{
		// Cannot bind to the specified IP and port; 
		//Try with an incremented port number
		_usPortNumber++;

		socketAddress.sin_port = htons(_usPortNumber);

	}
	std::cout << "Successfully bound the socket " << "to " << GetLocalAddress() << ":" << _usPortNumber << std::endl;

	return true;
}

SOCKET Socket::GetSocketHandle()
{
	return handleSocket;
}

std::string Socket::GetLocalAddress()
{
	char _pcLocalHostName[256];
	char _pcLocalAddress[256];
	struct sockaddr_in* sockaddr_localIP;

	gethostname(_pcLocalHostName, 256);

	struct addrinfo hints, *result, *p;
	int _iReturnValue;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;


	_iReturnValue = getaddrinfo(_pcLocalHostName, NULL, &hints, &result);

	/* loop over all returned results to get the address */
	for (p = result; p != NULL; p = p->ai_next) {
		sockaddr_localIP = (struct sockaddr_in *) p->ai_addr;
		inet_ntop(AF_INET, &(sockaddr_localIP->sin_addr), _pcLocalAddress, 256);
	}

	return std::string(_pcLocalAddress);
}

int Socket::EnableBroadcast()
{
	bool _iBroadCastOption = true;
	int _iResult = setsockopt(handleSocket, SOL_SOCKET, SO_BROADCAST,
		reinterpret_cast<char*>(&_iBroadCastOption), sizeof(_iBroadCastOption));
	if (_iResult == SOCKET_ERROR)
	{
		int _iError = WSAGetLastError();
		//ErrorRoutines::PrintWSAErrorInfo(_iError);
		std::cout << "Unable to enable broadcast option on the socket" << std::endl;
		closesocket(handleSocket);
		return _iError;
	}
	return _iResult;
}


int Socket::DisableBroadcast()
{
	bool _iBroadCastOption = false;
	int _iResult = setsockopt(handleSocket, SOL_SOCKET, SO_BROADCAST,
		reinterpret_cast<char*>(&_iBroadCastOption), sizeof(_iBroadCastOption));
	if (_iResult == SOCKET_ERROR)
	{
		int _iError = WSAGetLastError();
		//ErrorRoutines::PrintWSAErrorInfo(_iError);
		std::cout << "Unable to disable broadcast option on the socket" << std::endl;
		closesocket(handleSocket);
		return _iError;
	}
	return _iResult;
}

void Socket::SetRemotePort(unsigned short _usRemotePort)
{
	remotePort = _usRemotePort;
}

void Socket::SetRemoteAddress(unsigned long _ulRemoteAddress)
{
	remoteIPAddress = _ulRemoteAddress;
}