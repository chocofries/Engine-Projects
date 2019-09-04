/**************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2005 - 2018 Media Design School

File Name	:	client.cpp
Description	:	Class for chat client operations
Author   	:	Zheng Yu Bu
mail		:	zheng.yu5315@mediadesign.school.nz
**************************************************/

#define _WINSOCK_DEPRECATED_NO_WARNINGS

//Library Includes
#include <WS2tcpip.h>
#include <Windows.h>
#include <iostream>
#include <thread>
#include <string>

//Local Includes
#include "netutils.h"
//#include "consoletools.h"
#include "netmanager.h"
#include "networkentity.h"
#include "socket.h"

//This includes
#include "client.h"


Client::Client()
	:packetData(0)
	, clientSocket(0)
	, isServerDown(false)
{
	ZeroMemory(&serverSocketAddress, sizeof(serverSocketAddress));

	//Create a Packet Array and fill it out with all zeros.
	packetData = new char[MAX_MESSAGE_LENGTH];
	ZeroMemory(packetData, MAX_MESSAGE_LENGTH);

}

Client::~Client()
{
	isOnline = false;

	delete[] packetData;
	packetData = 0;

	delete clientSocket;
	clientSocket = 0;

	delete workQueue;
	workQueue = 0;
}

/***********************
* Initialise: Initialises a client object by creating a client socket and filling out the socket address structure with details of server to send the data to.
* @author:
* @parameter: none
* @return: void
********************/
bool Client::Initialise()
{
	//Local Variables to hold Server's IP address and Port NUmber as entered by the user
	char _cServerIPAddress[128];
	ZeroMemory(&_cServerIPAddress, 128);
	char _cServerPort[10];
	ZeroMemory(&_cServerPort, 10);
	//unsigned short _usServerPort;

	//Local variable to hold the index of the server chosen to connect to
	char serverChosen[5];
	ZeroMemory(serverChosen, 5);
	//unsigned int _uiServerIndex;

	//Local variable to hold client's name
	//char _cUserName[50];
	ZeroMemory(&userName, 50);

	//Zero out the memory for all the member variables.
	ZeroMemory(&userName, strlen(userName));

	//Create a work queue to distribute messages between the main  thread and the receive thread.
	workQueue = new WorkQueue<std::string>();

	//Create a socket object
	clientSocket = new Socket();

	//Get the port number to bind the socket to
	////////// unsigned short _usClientPort = QueryPortNumber(DEFAULT_CLIENT_PORT);
	//Initialise the socket to the port number
	if (!clientSocket->Initialise(DEFAULT_CLIENT_PORT))
	{
		return false;
	}

	//Set the client's online status to true
	isOnline = true;

	//Use a boolean flag to determine if a valid server has been chosen by the client or not
	//bool serverIsChosen = false;
	int broadcastCount = 0;

	//do
	//{
		//----------------------------------------------- BROADCAST TO DETECT SERVER ----------------------------
		isDoBroadcast = true;
		clientSocket->EnableBroadcast();
		BroadcastForServers();
		if (vecServerAddress.size() == 0)
		{
			std::cout << "No Servers Found." << std::endl;
			broadcastCount++;
			return false;
		}
		else
		{
			//Give a list of servers for the user to choose from :
			for (unsigned int i = 0; i < vecServerAddress.size(); i++)
			{
				std::cout << std::endl << "[" << i << "]" << " SERVER : found at " << ToString(vecServerAddress[i]) << std::endl;
			}

			isDoBroadcast = false;
			//std::cout << "Choose a server number to connect to :";
			//gets_s(serverChosen);
		}

		/*
		//----------------------------------------------- DEFAULT SERVER ADDRESS AND PORT ---------------------------

		//if (broadcastCount >= 1 && serverIsChosen == false)
		//{
		//	//Server IP Address
		//	strcpy_s(_cServerIPAddress, "127.0.0.1");

		//	//Get Port number of the server
		//	_usServerPort = DEFAULT_SERVER_PORT;
		//	//Fill in the details of the server's socket address structure.
		//	//This will be used when stamping address on outgoing packets
		//	serverSocketAddress.sin_family = AF_INET;
		//	serverSocketAddress.sin_port = htons(_usServerPort);
		//	inet_pton(AF_INET, _cServerIPAddress, &serverSocketAddress.sin_addr);
		//	serverIsChosen = true;
		//	std::cout << "Attempting to connect to server at " << _cServerIPAddress << ":" << _usServerPort << std::endl;
		//}
		*/

	//} while (serverIsChosen == false);

	//send a hanshake message to the server as part of the client's initialization process.
	//Packet _packet;
	//_packet.Serialize(HANDSHAKE, "Player");
	//SendData(_packet.packetData);

	return true;
}

bool Client::BroadcastForServers()
{
	//Make a broadcast packet
	Packet _packet;
	_packet.Serialize(ASTRO_BROADCAST, "Broadcast to Detect Server");

	char _pcTempBuffer[MAX_MESSAGE_LENGTH];
	//Send out a broadcast message using the broadcast address
	//clientSocket->SetRemoteAddress(INADDR_BROADCAST);
	clientSocket->SetRemoteAddress(inet_addr("127.255.255.255"));
	//clientSocket->SetRemotePort(DEFAULT_SERVER_PORT);

	serverSocketAddress.sin_family = AF_INET;
	//serverSocketAddress.sin_addr.S_un.S_addr = INADDR_BROADCAST;
	serverSocketAddress.sin_addr.S_un.S_addr = inet_addr("127.255.255.255");
	//m_ServerSocketAddress.sin_port = htons(DEFAULT_SERVER_PORT);
	//SendData(_packet.PacketData);

	for (int i = 0; i < 10; i++) //Just try  a series of 10 ports to detect a running server; this is needed since we are testing multiple servers on the same local machine
	{
		unsigned short servPort = DEFAULT_SERVER_PORT + i;
		clientSocket->SetRemotePort(servPort);
		serverSocketAddress.sin_port = htons(servPort);

		std::string str = "Broadcast sent to: " + ToString(servPort);
		std::cout << str << std::endl;
		SendData(_packet.packetData);
	}
	ReceiveBroadcastMessages(_pcTempBuffer);

	return true;

}

void Client::ConnectToChosenServer(int _serverIndex)
{
	unsigned int _uiServerIndex = (unsigned int)_serverIndex;
	serverSocketAddress.sin_family = AF_INET;
	serverSocketAddress.sin_port = vecServerAddress[_uiServerIndex].sin_port;
	serverSocketAddress.sin_addr.S_un.S_addr = vecServerAddress[_uiServerIndex].sin_addr.S_un.S_addr;
	std::string _strServerAddress = ToString(vecServerAddress[_uiServerIndex]);
	std::cout << "Connecting to server at " << _strServerAddress << std::endl;

	SendHandshake();
}

void Client::SendHandshake()
{
	Packet _packet;
	_packet.Serialize(HANDSHAKE, netManager->GetUsername().c_str()); //Send Username to server
	SendData(_packet.packetData);
}

void Client::SendGameUpdate(const char* _updateData)
{
	Packet _packet;
	_packet.Serialize(GAME_UPDATE, _updateData);
	SendData(_packet.packetData);
}

std::vector<sockaddr_in>* Client::GetVecServerAddress()
{
	return &vecServerAddress;
}

std::vector<std::string>* Client::GetVecServerName()
{
	return &vecServerName;
}

void Client::SetNetManager(NetManager * _net)
{
	netManager = _net;
}

void Client::ReceiveBroadcastMessages(char* _pcBufferToReceiveData)
{
	//set a timer on the socket for one second
	struct timeval timeValue;
	timeValue.tv_sec = 2000;
	timeValue.tv_usec = 0;

	if (setsockopt(clientSocket->GetSocketHandle(), SOL_SOCKET, SO_RCVTIMEO,
		(char*)&timeValue, sizeof(struct timeval)) < 0)
	{
		std::cout << "Error in something.";
	}

	//Receive data into a local buffer
	char _buffer[MAX_MESSAGE_LENGTH];
	sockaddr_in _FromAddress;
	int iSizeOfAdd = sizeof(sockaddr_in);
	//char _pcAddress[50];

	while (isDoBroadcast)
	{
		// pull off the packet(s) using recvfrom()
		int _iNumOfBytesReceived = recvfrom(			// pulls a packet from a single source...
			this->clientSocket->GetSocketHandle(),		// client-end socket being used to read from
			_buffer,									// incoming packet to be filled
			MAX_MESSAGE_LENGTH,							// length of incoming packet to be filled
			0,											// flags
			reinterpret_cast<sockaddr*>(&_FromAddress),	// address to be filled with packet source
			&iSizeOfAdd									// size of the above address struct.
		);

		if (_iNumOfBytesReceived < 0)
		{
			//Error in receiving data 
			int _iError = WSAGetLastError();
			//std::cout << "recvfrom failed with error " << _iError;
			if (_iError == WSAETIMEDOUT) // Socket timed out on Receive
			{
				isDoBroadcast = false; //Do not broadcast any more
				break;
			}
			_pcBufferToReceiveData = 0;
		}
		else if (_iNumOfBytesReceived == 0)
		{
			//The remote end has shutdown the connection
			_pcBufferToReceiveData = 0;
		}
		else
		{
			//There is valid data received.
			strcpy_s(_pcBufferToReceiveData, strlen(_buffer) + 1, _buffer);
			serverSocketAddress = _FromAddress;
			vecServerAddress.push_back(serverSocketAddress);

			//There is valid data received.
			//strcpy_s(_pcBufferToReceiveData, strlen(_buffer) + 1, _buffer);
			//strcpy_s(m_pcPacketData, strlen(_buffer) + 1, _buffer);
			//Put this packet data in the workQ
			//serverSocketAddress = fromAddress;
			ProcessData(_pcBufferToReceiveData); //Process the data
		}
	}//End of while loop
}

bool Client::SendData(char* _pcDataToSend)
{
	int _iBytesToSend = (int)strlen(_pcDataToSend) + 1;

	char _RemoteIP[MAX_ADDRESS_LENGTH];
	inet_ntop(AF_INET, &serverSocketAddress.sin_addr, _RemoteIP, sizeof(_RemoteIP));
	//std::cout << "Trying to send " << _pcDataToSend << " to " << _RemoteIP << ":" << ntohs(m_ServerSocketAddress.sin_port) << std::endl;
	char _message[MAX_MESSAGE_LENGTH];
	strcpy_s(_message, strlen(_pcDataToSend) + 1, _pcDataToSend);

	int iNumBytes = sendto(
		clientSocket->GetSocketHandle(),				// socket to send through.
		_pcDataToSend,									// data to send
		_iBytesToSend,									// number of bytes to send
		0,												// flags
		reinterpret_cast<sockaddr*>(&serverSocketAddress),	// address to be filled with packet target
		sizeof(serverSocketAddress)							// size of the above address struct.
	);
	//iNumBytes;
	if (_iBytesToSend != iNumBytes)
	{
		std::cout << "There was an error in sending data from client to server" << std::endl;
		//netManager->Disconnect();
		return false;
	}

	messageSent = true;
	return true;
}

void Client::ReceiveData(char* _pcBufferToReceiveData, int& isEnd)
{
	sockaddr_in fromAddress; // Make a local variable to extract the IP and port number of the sender from whom we are receiving
							 //In this case; it should be the details of the server; since the client only ever receives from the server
	int sizeOfAdd = sizeof(fromAddress);
	int numOfBytesReceived;

	//Receive data into a local buffer
	char buffer[MAX_MESSAGE_LENGTH];
	//For debugging purpose only, convert the Address structure to a string.
	char address[50];
	ZeroMemory(&address, 50);

	while (isEnd == 0 && isOnline)
	{
		// pull off the packet(s) using recvfrom()W
		numOfBytesReceived = recvfrom(			// pulls a packet from a single source...
			this->clientSocket->GetSocketHandle(),						// client-end socket being used to read from
			buffer,							// incoming packet to be filled
			MAX_MESSAGE_LENGTH,					   // length of incoming packet to be filled
			0,										// flags
			reinterpret_cast<sockaddr*>(&fromAddress),	// address to be filled with packet source
			&sizeOfAdd								// size of the above address struct.
		);
		inet_ntop(AF_INET, &fromAddress, address, sizeof(address));

		if (numOfBytesReceived < 0)
		{
			//Error in receiving data 
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
			//std::cout << "Error in receiving data  " << WSAGetLastError() << std::endl;
			//isServerDown = true;

			_pcBufferToReceiveData = 0;
		}
		else if (numOfBytesReceived == 0)
		{
			//The remote end has shutdown the connection
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
			std::cout << "The remote end has shutdown. " << WSAGetLastError() << std::endl;;
			//isServerDown = true;
			_pcBufferToReceiveData = 0;
			break;
		}
		else
		{
			//There is valid data received.
			strcpy_s(packetData, strlen(buffer) + 1, buffer);
			//strcpy_s(m_pcPacketData, strlen(_buffer) + 1, _buffer);
			//Put this packet data in the workQ
			serverSocketAddress = fromAddress;
			workQueue->push(packetData);
		}
		//std::this_thread::yield(); //Yield the processor; giving the main a chance to run.
	}
}

void Client::ProcessData(char* _pcDataReceived)
{

	Packet packetReceived;
	packetReceived = packetReceived.Deserialize(_pcDataReceived);
	switch (packetReceived.messageType)
	{
	case HANDSHAKE_SUCCESS:
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
		std::cout << "Welcome User!" << std::endl;
		std::cout << "SERVER> " << packetReceived.messageContent << std::endl;
		userNameAccepted = true;
		break;
	}
	case HANDSHAKE_FAIL:
	{
		std::cout << "Handshake failed! Server is full!" << std::endl;
		break;
	}
	case DATA:
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
		std::cout << packetReceived.messageContent << std::endl;
		break;
	}
	case COMMAND:
	{
		//std::cout << "User received a command " << std::endl;
		std::string recvdStr = packetReceived.messageContent;

		char command;

		//Set the command char to the correct one
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
			std::cout << "Available commands: " << std::endl;
			std::cout << "!?     List commands" << std::endl;
			std::cout << "!q     Quit application" << std::endl;
		}

		//Process command char
		if (command == 'q' || command == 'Q') //if quitting
		{
			netManager->Disconnect();
			netManager->ResetLevel();
			//Disconnect and return to menu
		}

		break;
	}
	case ASTRO_BROADCAST:
	{
		std::cout << "Broadcast recieved by server!" << std::endl;
		vecServerName.push_back(packetReceived.messageContent);
		break;
	}
	case KEEPALIVE:
	{
		//Send keep alive message back to server
		Packet packet;
		packet.Serialize(KEEPALIVE, "Still alive.");
		SendData(packet.packetData);
		break;
	}
	case GAME_UPDATE:
	{
		//If it's a game update packet, send data to network manager, which then sends it to the current level
		const char* gameUpdateStr = packetReceived.messageContent;
		netManager->PassGameUpdate(gameUpdateStr);
		std::cout << "Updated!" << std::endl;
		break;
	}
	case PLAYER_NUM:
	{
		int num = atoi(packetReceived.messageContent);
		netManager->SetEntityID(num);
		break;
	}
	default:
		break;

	}
}

void Client::GetRemoteIPAddress(char *_pcSendersIP)
{
	inet_ntop(AF_INET, &(serverSocketAddress.sin_addr), _pcSendersIP, sizeof(_pcSendersIP));
	return;
}

unsigned short Client::GetRemotePort()
{
	return ntohs(serverSocketAddress.sin_port);
}

bool Client::GetIsServerDown() const
{
	return isServerDown;
}

bool Client::GetMessageSent() const
{
	return messageSent;
}

void Client::GetPacketData(char* _pcLocalBuffer)
{
	strcpy_s(_pcLocalBuffer, strlen(packetData) + 1, packetData);
}

WorkQueue<std::string>* Client::GetWorkQueue()
{
	return workQueue;
}