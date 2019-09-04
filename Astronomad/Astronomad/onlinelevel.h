#pragma once
#include "level.h"

class Planet;
class Mothership;
class ButtonSet;
class Swarmling;
class Particle;
//class Client;
//class Server;

class OnlineLevel : public Level
{
public:
	OnlineLevel();
	virtual ~OnlineLevel();

	virtual void Initialise(Game* _myGame, ShaderLoader* _shaderloader, AssetLoader* _textureloader, InputManager* _inputManager);
	virtual void Update();
	virtual void Draw();

	virtual void ProcessKeyInput();
	virtual void ResetLevel();

	//Networking
	virtual void NetworkUpdate();
	virtual void ProcessNetworkData(const char* _gameUpdateStr);
	virtual void Disconnect();

	void InitPlanets();

private:
	enum OnlineState
	{
		ONLINE_MENU, //Choosing host or join
		ONLINE_HOST, //Hosting (creating a server)
		ONLINE_JOIN,	//Joining (creating a client, and broadcast for server)
		ONLINE_GAME	//Waiting after server is created, or after client has joined a server
	};

	//NetworkEntity * networkEntity;	//Type of network, Server or client
	//Client* client;
	//Server* server;
	std::string name;				//Name of Player
	
	OnlineState state;

	std::vector<Mothership*> vecPlayers; //Vector of player objects
	std::vector<Planet*> vecPlanets;
	std::vector<GameObject*> vecBoids;
	std::vector<Swarmling*> vecLevelSwarm;
	std::vector<Particle*> vecParticles;
	std::vector<Swarmling*> vecExcess;

	ButtonSet* menuButtons;
	ButtonSet* joinButtons;
	Planet* earth;
	Planet* mars;

	Mothership* enemy;

	Texter* title;
	Texter* textA;

	bool threadIsInit; //If the net manager as been initialized as a client or server
	int myShipID; //The shipID of the current client/server

	//Join state
	bool isServerChosen;
};