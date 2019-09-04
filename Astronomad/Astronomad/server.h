/**************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2005 - 2018 Media Design School

File Name	:	server.h
Description	:	Class for chat server operations
Author   	:	Zheng Yu Bu
mail		:	zheng.yu5315@mediadesign.school.nz
**************************************************/

#ifndef __SERVER_H__
#define __SERVER_H__

// Library Includes
#include <Windows.h>
#include <map>
#include <time.h>

// Local Includes
#include "networkentity.h"
#include "WorkQueue.h"

// Types

// Constants

//Forward Declaration
class NetManager;
class Socket;

//Structure to hold the details of all connected clients
struct ClientDetails
{
	sockaddr_in clientAddress;
	bool isActive;
	std::string strName;
	//time_t m_timeOfLastMessage;
};

class Server : public NetworkEntity
{
public:
	// Default Constructors/Destructors
	Server();
	virtual ~Server();

	// Virtual Methods from the Network Entity Interface.
	virtual bool Initialise(); //Implicit in the intialization is the creation and binding of the socket
	virtual bool SendData(char* _pcDataToSend);
	bool SendDataTo(char* _pcDataToSend, sockaddr_in _clientAdrress);
	bool SendDataAll(char* _pcDataToSend);
	void KeepAlive(int& _gameIsEnd);

	virtual void ReceiveData(char* _pcBufferToReceiveData, int& isEnd);
	virtual void ProcessData(); //ProcessData(std::pair<sockaddr_in, std::string> dataItem);
	virtual void GetRemoteIPAddress(char* _pcSendersIP);
	virtual unsigned short GetRemotePort();

	std::string FindUsername(std::string _address); //Finds stored username from an address
	ClientDetails FindClient(std::string _address); //Finds client details from an address
	WorkQueue<std::pair<sockaddr_in, std::string>>* GetWorkQueue();

	void SetNetManager(NetManager* _net);
	void SendGameUpdate(const char* _updateData);

	std::vector<std::string>* GetVecClientName();

private:
	bool AddClient(std::string _strClientName);
	void SetNotAlive();

private:
	NetManager * netManager;

	bool serverIsActive;
	//A Buffer to contain all packet data for the server
	char* packetData;
	//A server has a socket object to create the UDP socket at its end.
	Socket* serverSocket;
	// Make a member variable to extract the IP and port number of the sender from whom we are receiving
	//Since it is a UDP socket capable of receiving from multiple clients; these details will change depending on who has sent the packet we are currently processing.
	sockaddr_in clientAddress;

	//Qs 2 : Make a map to hold the details of all the client who have connected. 
	//The structure maps client addresses to client details
	std::map<std::string, ClientDetails>* mapConnectedClients;

	//A workQueue to distribute messages between the main thread and Receive thread.
	WorkQueue<std::pair<sockaddr_in, std::string>>* workQueue;

	std::vector<std::string> vecClientName; //Vector of clients' usernames
};

#endif