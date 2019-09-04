#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <fmod.hpp>
#include "shaderloader.h"
#include "assetloader.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "level.h"
#include "inputmanager.h"


class Level;
class InputManager;
class AudioManager;
class NetManager;
class Client;
class Server;

class Game
{
public:
	Game();
	~Game();

	void Initialise(int _width, int _height);
	void Update();
	void Draw();
	void ShutDownNet();

	static Game& GetGameInstance();
	static void DestroyGameInstance();

	Level* GetCurrentLevel() const;
	void SetScreenMouseX(int _x);
	void SetScreenMouseY(int _y);
	void SetCurrentLevel(int _vecIndex);

	InputManager* GetInputManager();
	NetManager* GetNetManager();

	std::string GetUsername();
	void SetUsername(std::string _name);

protected:
	//Singleton
	static Game* currentGame;

private:
	ShaderLoader * shaderLoader;
	AssetLoader* textureLoader;
	InputManager* inputManager;
	AudioManager* audioManager;
	NetManager* netManager;
	Level* currentLevel;
	std::vector<Level*> vecLevels;

	Client* client;
	Server* server;

	std::string username; //Name of the user (used for server and client name)

	//bool threadIsInit; //If the net manager as been initialized as a client or server
	int currentRows;
	int currentColumns;
	int screenMouseX;
	int screenMouseY;
};