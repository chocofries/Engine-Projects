/**************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2005 - 2018 Media Design School

File Name	:	server.cpp
Description	:	Class for chat server operations
Author   	:	Zheng Yu Bu
mail		:	zheng.yu5315@mediadesign.school.nz
**************************************************/

//Library Includes
#include <WS2tcpip.h>
#include <iostream>
#include <utility>
#include <thread>
#include <chrono>

//Local Includes
#include "netmanager.h"
#include "netutils.h"
#include "socket.h"
#include "clock.h"


//Local Includes
#include "server.h"

Server::Server()
	:packetData(0),
	serverSocket(0),
	serverIsActive(true)
{
	ZeroMemory(&clientAddress, sizeof(clientAddress));
}

Server::~Server()
{
	isOnline = false;

	delete mapConnectedClients;
	mapConnectedClients = 0;

	delete serverSocket;
	serverSocket = 0;

	delete workQueue;
	workQueue = 0;

	delete[] packetData;
	packetData = 0;
}

bool Server::Initialise()
{
	packetData = new char[MAX_MESSAGE_LENGTH];

	//Create a work queue to distribute messages between the main  thread and the receive thread.
	workQueue = new WorkQueue<std::pair<sockaddr_in, std::string>>();

	//Create a socket object
	serverSocket = new Socket();

	//Get the port number to bind the socket to
	//unsigned short _usServerPort = QueryPortNumber(DEFAULT_SERVER_PORT);
	unsigned short _usServerPort = DEFAULT_SERVER_PORT;

	//Initialise the socket to the local loop back address and port number
	if (!serverSocket->Initialise(_usServerPort))
	{
		return false;
	}

	//Set the server's online status to true
	isOnline = true;

	//Qs 2: Create the map to hold details of all connected clients
	mapConnectedClients = new std::map < std::string, ClientDetails >();

	return true;
}

void Server::KeepAlive(int& isEnd)
{
	//Set a timer
	Clock clock;
	clock.Initialise();
	double deltaTime = 0;
	double timeElapsed = 0;
	bool keepAliveSent = false;

	//Update Loop
	while (isEnd == 0 && isOnline)
	{
		//Process the time
		clock.Process();
		deltaTime = clock.GetDeltaTick();
		timeElapsed += deltaTime;

		if (keepAliveSent == false) //Set all clients' alive-values to false, then send a keep alive message
		{
			for (auto it = mapConnectedClients->begin(); it != mapConnectedClients->end(); it++)
			{
				ClientDetails client = it->second;
				client.isActive = false;
				int itemp = 0;
			}

			Packet _packet;
			_packet.Serialize(KEEPALIVE, "Still good?");
			SendDataAll(_packet.packetData);
			keepAliveSent = true;
		}

		if (timeElapsed > 2.0f && keepAliveSent == true) //Iterate through the connected clients and remove inactive
		{
			for (auto it = mapConnectedClients->begin(); it != mapConnectedClients->end(); it++)
			{
				ClientDetails client = it->second;
				if (client.isActive == false)
				{
					std::string message = client.strName + " has disconnected.";

					//Send disconnection message to all
					Packet _packet;
					_packet.Serialize(DATA, const_cast<char*>(message.c_str()));
					SendDataAll(_packet.packetData);

					mapConnectedClients->erase(it); //Remove from map
				}
			}
			timeElapsed = 0.0f;
			keepAliveSent = false;
		}
	}
	int hurray = 1;
}

bool Server::AddClient(std::string _strClientName)
{
	//TO DO : Add the code to add a client to the map here...

	std::string username = _strClientName;
	int num = 1;

	for (auto it = mapConnectedClients->begin(); it != mapConnectedClients->end(); ++it)
	{
		//Check to see that the client to be added does not already exist in the map, 
		if (it->first == ToString(clientAddress))
		{
			return false;
		}
		//also check for the existence of the username
		else if (it->second.strName == username)
		{
			//Iterate the concatenation number
			num++;

			//If the username exists, then add a number to the end of it then reiterate
			username = _strClientName + std::to_string(num);

			//Set the map iterator to the beginning again
			it = mapConnectedClients->begin();
		}
	}

	//Add the client to the map.
	ClientDetails _clientToAdd;
	_clientToAdd.strName = username;
	_clientToAdd.clientAddress = this->clientAddress;
	_clientToAdd.isActive = true;

	std::string _strAddress = ToString(clientAddress);
	mapConnectedClients->insert(std::pair < std::string, ClientDetails >(_strAddress, _clientToAdd));
	return true;
}

void Server::SetNotAlive()
{
	for (auto it = mapConnectedClients->begin(); it != mapConnectedClients->end(); it++)
	{
		ClientDetails client = it->second;
		client.isActive = false;
		int itemp = 0;
	}
}

bool Server::SendData(char* _pcDataToSend)
{
	int _iBytesToSend = (int)strlen(_pcDataToSend) + 1;

	int iNumBytes = sendto(
		serverSocket->GetSocketHandle(),				// socket to send through.
		_pcDataToSend,									// data to send
		_iBytesToSend,									// number of bytes to send
		0,												// flags
		reinterpret_cast<sockaddr*>(&clientAddress),	// address to be filled with packet target
		sizeof(clientAddress)							// size of the above address struct.
	);
	//iNumBytes;
	if (_iBytesToSend != iNumBytes)
	{
		std::cout << "There was an error in sending data from client to server" << std::endl;
		return false;
	}
	return true;
}

bool Server::SendDataTo(char* _pcDataToSend, sockaddr_in _clientAdrress)
{
	int _iBytesToSend = (int)strlen(_pcDataToSend) + 1;

	int iNumBytes = sendto(
		serverSocket->GetSocketHandle(),				// socket to send through.
		_pcDataToSend,									// data to send
		_iBytesToSend,									// number of bytes to send
		0,												// flags
		reinterpret_cast<sockaddr*>(&_clientAdrress),	// address to be filled with packet target
		sizeof(_clientAdrress)							// size of the above address struct.
	);
	//iNumBytes;
	if (_iBytesToSend != iNumBytes)
	{
		std::cout << "There was an error in sending data from client to server" << std::endl;
		return false;
	}
	return true;
}

bool Server::SendDataAll(char * _pcDataToSend)
{
	//int _iBytesToSend = (int)strlen(_pcDataToSend) + 1;

	if (netManager != nullptr && mapConnectedClients != nullptr)
	{
		//Iterate through the map of connected clients, send the data to all
		for (auto it = mapConnectedClients->begin(); it != mapConnectedClients->end(); ++it)
		{
			ClientDetails clientDetails = it->second;
			sockaddr_in clientAddress = clientDetails.clientAddress;

			SendDataTo(_pcDataToSend, clientAddress);
		}
	}

	return true;
}

void Server::ReceiveData(char* _pcBufferToReceiveData, int& isEnd)
{

	int iSizeOfAdd = sizeof(clientAddress);
	int _iNumOfBytesReceived;
	int _iPacketSize;

	//Make a thread local buffer to receive data into
	char _buffer[MAX_MESSAGE_LENGTH];

	while (isEnd == 0 && isOnline)
	{
		// pull off the packet(s) using recvfrom()
		_iNumOfBytesReceived = recvfrom(			// pulls a packet from a single source...
			serverSocket->GetSocketHandle(),						// client-end socket being used to read from
			_buffer,							// incoming packet to be filled
			MAX_MESSAGE_LENGTH,					   // length of incoming packet to be filled
			0,										// flags
			reinterpret_cast<sockaddr*>(&clientAddress),	// address to be filled with packet source
			&iSizeOfAdd								// size of the above address struct.
		);
		if (_iNumOfBytesReceived < 0)
		{
			int _iError = WSAGetLastError();
			if (_iError == WSAECONNRESET)
			{
				auto it = mapConnectedClients->find(ToString(clientAddress));
				if (it != mapConnectedClients->end())
				{
					ClientDetails client = it->second;
					client.isActive = false;

					std::string message = client.strName + " has disconnected.";

					mapConnectedClients->erase(it); //Remove from map

					//Send disconnection message to all
					Packet _packet;
					_packet.Serialize(DATA, const_cast<char*>(message.c_str()));
					SendDataAll(_packet.packetData);

					std::cout << message << std::endl;
				}

			}
			else
			{
				//ErrorRoutines::PrintWSAErrorInfo(_iError);

			}
			//return false;
		}
		else
		{
			_iPacketSize = static_cast<int>(strlen(_buffer)) + 1;
			strcpy_s(_pcBufferToReceiveData, _iPacketSize, _buffer);
			char _IPAddress[100];
			inet_ntop(AF_INET, &clientAddress.sin_addr, _IPAddress, sizeof(_IPAddress));

			std::cout << "Server Received \"" << _pcBufferToReceiveData << "\" from " <<
				_IPAddress << ":" << ntohs(clientAddress.sin_port) << std::endl;
			//Push this packet data into the WorkQ
			workQueue->push(std::make_pair(clientAddress, _pcBufferToReceiveData));
		}
		//std::this_thread::yield();
	} //End of while (true)
}



void Server::GetRemoteIPAddress(char *_pcSendersIP)
{
	char _temp[MAX_ADDRESS_LENGTH];
	int _iAddressLength;
	inet_ntop(AF_INET, &(clientAddress.sin_addr), _temp, sizeof(_temp));
	_iAddressLength = static_cast<int>(strlen(_temp)) + 1;
	strcpy_s(_pcSendersIP, _iAddressLength, _temp);
}

unsigned short Server::GetRemotePort()
{
	return ntohs(clientAddress.sin_port);
}

void Server::ProcessData() //(std::pair<sockaddr_in, std::string> dataItem)
{
	while (isOnline)
	{
		//Retrieve off a message from the queue and process it
		std::pair<sockaddr_in, std::string> dataItem;

		//GET THE DATA ITEM FROM THE WORKQUEUE!
		if (!workQueue->empty())
		{
			char * c = netManager->GetIPAddress();
			GetRemoteIPAddress(c);
			workQueue->pop(dataItem);
		}
		else
		{
			continue;
		}

		Packet _packetRecvd, _packetToSend;
		_packetRecvd = _packetRecvd.Deserialize(const_cast<char*>(dataItem.second.c_str()));

		switch (_packetRecvd.messageType)
		{
		case HANDSHAKE:
		{
			std::string message = "Users in chatroom : ";

			for (auto it = mapConnectedClients->begin(); it != mapConnectedClients->end(); ++it)
			{
				ClientDetails clientDetails = it->second;
				message += clientDetails.strName;
			}

			std::cout << "Server received a handshake message " << std::endl;

			if (mapConnectedClients->size() < 3) //Limit maximum clients to 3 (max players 4)
			{
				if (AddClient(_packetRecvd.messageContent))
				{
					_packetToSend.Serialize(HANDSHAKE_SUCCESS, netManager->GetUsername().c_str()); //Send username to client
					SendDataTo(_packetToSend.packetData, dataItem.first);

					std::string playerNumStr = ToString(mapConnectedClients->size());
					_packetToSend.Serialize(PLAYER_NUM, const_cast<char*>(playerNumStr.c_str()));
					SendDataTo(_packetToSend.packetData, dataItem.first);

					std::string username = FindUsername(ToString(clientAddress));
					std::string sendStr = username + " has entered the chat.";

					//vecClientName.push_back(_packetRecvd.messageContent);

					Packet _packet;
					_packet.Serialize(DATA, const_cast<char*>(sendStr.c_str()));
					SendDataAll(_packet.packetData);
				}
			}
			else
			{
				_packetToSend.Serialize(HANDSHAKE_FAIL, "Server is full.");
				SendDataTo(_packetToSend.packetData, dataItem.first);
			}

			break;
		}
		case DATA:
		{
			_packetToSend.Serialize(DATA, _packetRecvd.messageContent);


			//Find username of ip address
			std::string username = FindUsername(ToString(clientAddress));

			std::string rStr = _packetRecvd.messageContent;
			std::string sendStr = username + ": " + rStr;

			Packet _packet;
			_packet.Serialize(DATA, const_cast<char*>(sendStr.c_str()));
			SendDataAll(_packet.packetData);

			break;
		}
		case COMMAND:
		{
			std::cout << "Server received a command " << std::endl;
			std::string recvdStr = _packetRecvd.messageContent;

			char command;

			for (unsigned int i = 0; i < strlen(recvdStr.c_str()) - 1; i++)
			{
				if (recvdStr.at(i) == '!') //Command
				{
					command = recvdStr.at(i + 1);
					break;
				}
			}

			if (command == '?') //if wanting list of commands
			{
				//Command the client
				Packet _packetComm;
				_packetComm.Serialize(COMMAND, "!?");
				SendDataTo(_packetComm.packetData, dataItem.first);
			}

			if (command == 'q' || command == 'Q') //if quitting
			{
				//Remove this client from the map
				auto it = mapConnectedClients->find(ToString(clientAddress));
				if (it != mapConnectedClients->end())
				{
					std::string username = FindUsername(ToString(clientAddress));
					std::string message = username + " has left the chat.";
					std::cout << message << std::endl;

					//Command the client to exit
					Packet _packetComm;
					_packetComm.Serialize(COMMAND, "!q");
					SendDataTo(_packetComm.packetData, dataItem.first);

					mapConnectedClients->erase(it); //Remove from map

					//Send client leaving message to all
					Packet _packet;
					_packet.Serialize(DATA, const_cast<char*>(message.c_str()));
					SendDataAll(_packet.packetData);
				}
			}

			break;
		}
		case ASTRO_BROADCAST:
		{
			std::cout << "Received a broadcast packet" << std::endl;
			//Just send out a packet to the back to the client again which will have the server's IP and port in it's sender fields
			_packetToSend.Serialize(ASTRO_BROADCAST, netManager->GetUsername().c_str()); //Send name of server to the client
			SendDataTo(_packetToSend.packetData, dataItem.first);
			break;
		}
		case KEEPALIVE:
		{
			//Set this client's alive value to true in the map
			auto it = mapConnectedClients->find(ToString(clientAddress));
			if (it != mapConnectedClients->end())
			{
				ClientDetails client = it->second;
				client.isActive = true;
			}
			break;
		}
		case GAME_UPDATE:
		{
			//If it's a game update packet, send data to network manager, which then sends it to the current level
			const char* gameUpdateStr = _packetRecvd.messageContent;
			netManager->PassGameUpdate(gameUpdateStr);
			std::cout << "Updated!" << std::endl;

			//Packet _packet;
			//_packet.Serialize(GAME_UPDATE, _packetRecvd.messageContent);
			//SendDataAll(_packet.packetData);
			break;
		}
		default:
			break;
		}
	}
}

std::string Server::FindUsername(std::string _address)
{
	std::string username = "User";

	//Find username of ip address
	auto it = mapConnectedClients->find(_address);
	if (it != mapConnectedClients->end())
	{
		ClientDetails clientDet = it->second;
		username = clientDet.strName;
	}

	return username;
}

ClientDetails Server::FindClient(std::string _address)
{
	ClientDetails client;

	auto it = mapConnectedClients->find(ToString(clientAddress));
	if (it != mapConnectedClients->end())
	{
		client = it->second;
	}

	return client;
}

WorkQueue<std::pair<sockaddr_in, std::string>>* Server::GetWorkQueue()
{
	return workQueue;
}

void Server::SetNetManager(NetManager * _net)
{
	netManager = _net;
}

void Server::SendGameUpdate(const char * _updateData)
{
	Packet _packet;
	_packet.Serialize(GAME_UPDATE, _updateData);
	SendDataAll(_packet.packetData);
}

std::vector<std::string>* Server::GetVecClientName()
{
	return &vecClientName;
}
