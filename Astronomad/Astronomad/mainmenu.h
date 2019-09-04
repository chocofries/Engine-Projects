#pragma once
#include "level.h"

class Planet;
class ButtonSet;

class MainMenu : public Level
{
public:
	MainMenu();
	virtual ~MainMenu();

	virtual void Initialise(Game* _myGame, ShaderLoader* _shaderloader, AssetLoader* _textureloader, InputManager* _inputManager);
	virtual void Update();
	virtual void Draw();

	virtual void ProcessKeyInput();
	virtual void ResetLevel();

private:
	enum MenuState
	{
		MENU_NORM, //Normal menu state
		MENU_NAME //Username input state
	};

	MenuState menuState;

	std::vector<Planet*> vecPlanets;
	std::vector<GameObject*> vecBoids;

	ButtonSet* menuButtons;
};