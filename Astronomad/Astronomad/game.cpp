#include "game.h"
#include "mainmenu.h"
#include "audiomanager.h"
#include "netmanager.h"
#include "sceneSolarSystem.h"
#include "resultscreen.h"
#include "onlinelevel.h"


// Static Variables
Game* Game::currentGame = 0;

Game::Game()
{

}

Game::~Game()
{
	while (vecLevels.size() > 0)
	{
		Level* tempLevel = vecLevels[vecLevels.size() - 1];
		vecLevels.pop_back();
		delete tempLevel;
	}
	delete shaderLoader;
	delete textureLoader;
	delete inputManager;
	delete audioManager;
	//ShutDownNet();
}

void Game::Initialise(int _width, int _height)
{
	shaderLoader = new ShaderLoader();
	shaderLoader->CreateProgram("Assets/VertexShader.vs", "Assets/FragmentShader.fs", "vertShader0", "fragShader0");

	textureLoader = new AssetLoader();
	inputManager = new InputManager();
	audioManager = new AudioManager();
	audioManager->Initialise();

	username = "PLAYER";

	//Initialize Net Manager
	netManager = &NetManager::GetInstance();
	//netManager->StartUp(); //Start the manager

	Level* mainMenuLevel = new MainMenu();	 //Index 0
	mainMenuLevel->SetAudioManager(audioManager);
	mainMenuLevel->Initialise(this, shaderLoader, textureLoader, inputManager);
	vecLevels.push_back(mainMenuLevel);

	Level* solarSystemLevel = new SceneSolarSystem();  //Index 1
	solarSystemLevel->SetAudioManager(audioManager);
	solarSystemLevel->Initialise(this, shaderLoader, textureLoader, inputManager);
	vecLevels.push_back(solarSystemLevel);

	Level* resultsLevel = new ResultsLevel();  //Index 2
	resultsLevel->SetAudioManager(audioManager);
	resultsLevel->Initialise(this, shaderLoader, textureLoader, inputManager);
	vecLevels.push_back(resultsLevel);

	Level* lobbyLevel = new OnlineLevel();  //Index 3
	lobbyLevel->SetAudioManager(audioManager);
	lobbyLevel->SetNetManager(netManager);
	lobbyLevel->Initialise(this, shaderLoader, textureLoader, inputManager);
	vecLevels.push_back(lobbyLevel);

	currentLevel = mainMenuLevel;
}

void Game::Update()
{
	if (currentLevel != nullptr)
	{
		if (currentLevel->GetIsInit())
		{
			currentLevel->Update();
			currentLevel->SetScreenMouseX(screenMouseX);
			currentLevel->SetScreenMouseY(screenMouseY);
		}
	}

	audioManager->Update();
	inputManager->SetMouseX(screenMouseX);
	inputManager->SetMouseY(screenMouseY);
	inputManager->UpdateKeys();
}

void Game::Draw()
{
	if (currentLevel != nullptr)
	{
		if (currentLevel->GetIsInit())
		{
			currentLevel->Draw();
		}
	}
}

void Game::ShutDownNet()
{
	if (netManager != nullptr)
	{
		netManager->SendQuitCommand();
	}
}

Game& Game::GetGameInstance()
{
	if (currentGame == 0)
	{
		currentGame = new Game();
	}

	return (*currentGame);
}

void Game::DestroyGameInstance()
{
	delete currentGame;
	currentGame = 0;
}

Level * Game::GetCurrentLevel() const
{
	return currentLevel;
}

void Game::SetScreenMouseX(int _x)
{
	screenMouseX = _x;
}

void Game::SetScreenMouseY(int _y)
{
	screenMouseY = _y;
}

void Game::SetCurrentLevel(int _vecIndex)
{
	if (vecLevels.empty() == false && vecLevels[_vecIndex] != nullptr)
	{
		currentLevel = vecLevels[_vecIndex];
	}
}

InputManager * Game::GetInputManager()
{
	return inputManager;
}

NetManager * Game::GetNetManager()
{
	return netManager;
}

std::string Game::GetUsername()
{
	return username;
}

void Game::SetUsername(std::string _name)
{
	username = _name;
}
