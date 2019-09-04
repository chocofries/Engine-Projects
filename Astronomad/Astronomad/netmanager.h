/**************************************************
Bachelor of Software Engineering
Media Design School
Auckland
New Zealand

(c) 2005 - 2018 Media Design School

File Name	:	netmanager.h
Description	:	Class for client operations
Author   	:	Zheng Yu Bu
mail		:	zheng.yu5315@mediadesign.school.nz
**************************************************/
#ifndef __NETWORK_H__
#define __NETWORK_H__
#include <vector>

//Types
enum EEntityType
{
	CLIENT = 1,
	SERVER
};

//constants
namespace
{
	unsigned const DEFAULT_SERVER_PORT = 50122;
	unsigned const DEFAULT_CLIENT_PORT = 60123;
	unsigned const MAX_MESSAGE_LENGTH = 256;
	unsigned const MAX_ADDRESS_LENGTH = 32;
}

namespace ErrorRoutines
{
	void PrintWSAErrorInfo(int iError);
}

//Forward Decalarations
class NetworkEntity;
class Client;
class Server;
class Game;

class NetManager
{
public:
	~NetManager();

	bool Initialise(EEntityType _eType, Game* _gameInstance);
	void StartThreads();	//Starts the threads for receiving messages or keepalive
	void Update();			//Update the network manager's entity
	void StartUp();			//A network has an ability to start up
	void JoinThreads();		//Make the threads join up
	void ShutDown();		//& an ability to be shut down
	bool IsOnline();
	bool GetIsInit();
	void SetIsInit(bool _isInit);

	void SetGameIsEnd(int isEnd);

	//Accessor methods
	NetworkEntity* GetNetworkEntity();
	EEntityType GetCurrentType() const;

	// Client/Server methods
	bool ClientServersFound() const;
	int GetClientVecServersSize() const;
	std::vector<std::string>* GetClientVecServerName();
	void ConnectClientToChosenServer(int _serverIndex);
	std::string GetUsername();
	std::vector<std::string>* GetVecClientName();

	// Game Update methods
	void SendQuitCommand();								//Sends quit command to server or clients
	void Disconnect();									//Disconnect from the current session and return to menu
	void ResetLevel();									//Reset current level
	void PassGameUpdate(const char* _gameUpdateStr);	//Pass from network to level
	void SendGameUpdate(const char* _gameUpdate);		//Send from level to network
	void SetEntityID(int _ID);							//Set the ID num of this entity
	int GetEntityID() const;							//Get the ID num of this entity
	char* GetIPAddress();								//Get the IPAddress as a string

	clock_t GetClockStartTime() const;
	void SetClockStartTime(clock_t time);

	// Singleton Methods
	static NetManager& GetInstance();
	static void DestroyInstance();

private:
	//Make the network class a singleton. There is only one instance of the network running
	NetManager();
	NetManager(const NetManager& _kr);
	//CNetwork& operator = (const CNetwork& _kr);

protected:
	Game * gameInstance;

	//A network has a network entity
	NetworkEntity * networkEntity;
	Client* client;
	Server* server;
	EEntityType currentType;
	bool isOnline;
	bool isInit;
	int gameIsEnd; //var passed to threads to let them know if game is ending
	int entityID; //The ID of this entity

	// Singleton Instance
	static NetManager* netManagerSingleton;

	//A local buffer to receive packet data info
	char* packetData;

	//An array to hold the IP Address as a string
	char IPAddress[MAX_ADDRESS_LENGTH];

	//Threads
	std::thread clientReceiveThread, serverReceiveThread, keepAliveThread, processThread;
};
#endif