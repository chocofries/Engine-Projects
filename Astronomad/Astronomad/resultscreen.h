#pragma once
#include "level.h"

class Planet;
class ButtonSet;

class ResultsLevel : public Level
{
public:
	ResultsLevel();
	virtual ~ResultsLevel();

	virtual void Initialise(Game* _myGame, ShaderLoader* _shaderloader, AssetLoader* _assetloader, InputManager* _inputManager);
	virtual void Update();
	virtual void Draw();

	virtual void ProcessKeyInput();
	virtual void ResetLevel();

private:
	std::vector<Planet*> vecPlanets;
	std::vector<GameObject*> vecBoids;

	Texter* title;
};