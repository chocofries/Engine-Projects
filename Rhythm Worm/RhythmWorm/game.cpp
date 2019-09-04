#include <vld.h>
#include "game.h"


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
}

void Game::Initialise(int _width, int _height)
{
	shaderLoader = new ShaderLoader();
	shaderLoader->CreateProgram("Assets/VertexShader.vs", "Assets/FragmentShader.fs", "vertShader0", "fragShader0");

	clock = new Clock();
	clock->Initialise();
	clock->Process();

	textureLoader = new TextureLoader();

	inputManager = new InputManager();

	currentLevel = new Level();
	currentLevel->Initialise(this, shaderLoader, textureLoader, inputManager, clock);
	vecLevels.push_back(currentLevel);
}

void Game::Update()
{
	clock->Process();

	if (currentLevel != nullptr)
	{
		if (currentLevel->GetIsInit())
		{
			currentLevel->Update();
			currentLevel->SetScreenMouseX(screenMouseX);
			currentLevel->SetScreenMouseY(screenMouseY);
		}
	}
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

void Game::SetScreenMouseX(int _x)
{
	screenMouseX = _x;
}

void Game::SetScreenMouseY(int _y)
{
	screenMouseY = _y;
}

InputManager * Game::GetInputManager()
{
	return inputManager;
}
