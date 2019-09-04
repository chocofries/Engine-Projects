/**************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2005 - 2018 Media Design School

File Name	:	client.h
Description	:	Class for chat client operations
Author   	:	Zheng Yu Bu
mail		:	zheng.yu5315@mediadesign.school.nz
**************************************************/

#ifndef __CLIENT_H__
#define __CLIENT_H__

// Library Includes
#include <Windows.h>
#include <string>

// Local Includes
#include "networkentity.h"
#include "workqueue.h"

// Types

// Constants


//Forward Declaration
class NetManager;
class Socket;

class Client : public NetworkEntity
{
public:
	// Default Constructors/Destructors
	Client();
	virtual ~Client();

	// Methods

	virtual bool Initialise(); //Implicit in the intialization is the creation and binding of the socket
	virtual bool SendData(char* _pcDataToSend);
	virtual void ReceiveData(char* _pcBufferToReceiveData, int& isEnd);
	virtual void ProcessData(char* _pcDataReceived);
	virtual void GetRemoteIPAddress(char* _pcSendersIP);
	virtual unsigned short GetRemotePort();
	bool GetIsServerDown() const;
	bool GetMessageSent() const;

	void GetPacketData(char* _pcLocalBuffer);
	WorkQueue<std::string>* GetWorkQueue();

	//Qs7 : Broadcast to Detect Servers
	bool BroadcastForServers();
	void ConnectToChosenServer(int _serverIndex);
	void SendHandshake();
	void SendGameUpdate(const char* _updateData);
	std::vector<sockaddr_in>* GetVecServerAddress();
	std::vector<std::string>* GetVecServerName();

	void SetNetManager(NetManager* _net);

private:
	// Question 7 : Broadcast to Detect Servers
	void ReceiveBroadcastMessages(char* _pcBufferToReceiveData);

private:
	NetManager * netManager;
	//A buffer to contain all packet data for the client
	char* packetData;
	//A client has a socket object to create the UDP socket at its end.
	Socket* clientSocket;
	// A Sockaddress structure which will have the details of the server 
	sockaddr_in serverSocketAddress;
	//A username to associate with a client
	char userName[50];
	//If the username is accepted by the server
	bool userNameAccepted;
	//If the server is down
	bool isServerDown;
	//If a message is sent
	bool messageSent;
	//A workQueue to distribute messages between the main thread and Receive thread.
	WorkQueue<std::string>* workQueue;

	//Question 7
	//A vector to hold all the servers found after broadcasting
	std::vector<sockaddr_in> vecServerAddress;
	std::vector<std::string> vecServerName; //Vector of server names
	bool isDoBroadcast;
};

#endif