#pragma once
#include "game.h"
#include "clock.h"
#include "gameobject.h"
#include "audiomanager.h"
//#include "netmanager.h"

class Game;
class CubeMap;
class NetManager;

enum AstroFaction
{
	FAC_NONE,
	FAC_BLUE,
	FAC_RED,
	FAC_GREEN,
	FAC_PURPLE
};

class Level
{
public:
	Level();
	virtual ~Level();

	virtual void Initialise(Game* _myGame, ShaderLoader* _shaderloader, AssetLoader* _textureloader, InputManager* _inputManager);
	virtual void Update();
	virtual void Draw();

	virtual void ProcessKeyInput();

	virtual void ResetLevel(); //Resets level to initial state


	void ChangeLevel(int _index);

	std::vector<Texter*>* GetVecTexts();

	GLuint GetVBO() const;
	ShaderLoader* GetShaderLoader() const;
	AssetLoader* GetTextureLoader() const;
	InputManager* GetInputManager() const;
	AudioManager* GetAudioManager() const;
	NetManager* GetNetManager() const;
	Camera* GetCamera() const;
	Clock* GetClock() const;
	CubeMap* GetSkyBox() const;
	Level* GetCurrentLevel() const;
	bool GetIsInit() const;
	float GetBoundW() const;
	float GetBoundH() const;
	int GetLevelDifficulty() const;

	void SetAudioManager(AudioManager* _audio);
	void SetNetManager(NetManager* _netmanager);
	void SetLevelDifficulty(int _diff);
	void SetCurrentWinner(AstroFaction _winner);
	void SetScreenMouseX(int _x);
	void SetScreenMouseY(int _y);

	//Networking methods
	virtual void NetworkUpdate(); //Send game snapshot per tick to the server
	virtual void ProcessNetworkData(const char* _gameUpdateStr); //Process Networked Game Update data
	virtual void Disconnect(); //Disconnects from the server/clients
	std::string ParseTag(const char* _updateData, int _tagNum);
	int GetEntityID() const;
	void SetEntityID(int _id);
	std::string GetUsername();
	std::vector<std::string>* GetVecClientName();

protected:
	GLuint levelVBO;
	Game* myGame;
	ShaderLoader* shaderLoader;
	AssetLoader* assetLoader;
	InputManager* inputManager;
	AudioManager* audioManager;
	NetManager* netManager;
	Camera* levelCamera;
	Clock* clock;
	CubeMap* skyBox;

	std::vector<GameObject*> vecObjects;
	std::vector<Texter*> vecTexts;
	AstroFaction currentWinner;
	bool isInit;

	float deltaTime;
	float boundW;
	float boundH;
	int screenMouseX;
	int screenMouseY;
	int levelDifficulty;
	int entityID;
	glm::vec3 rayWorld;
	glm::vec3 intersection;
};