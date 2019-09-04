#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <fmod.hpp>
#include "shaderloader.h"
#include "textureloader.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "level.h"
#include "inputmanager.h"
#include "clock.h"

class Level;
class InputManager;

class Game
{
public:
	Game();
	~Game();

	void Initialise(int _width, int _height);
	void Update();
	void Draw();

	static Game& GetGameInstance();
	static void DestroyGameInstance();

	void SetScreenMouseX(int _x);
	void SetScreenMouseY(int _y);

	InputManager* GetInputManager();

protected:
	//Singleton
	static Game* currentGame;

private:
	ShaderLoader * shaderLoader;
	TextureLoader* textureLoader;
	InputManager* inputManager;
	Level* currentLevel;
	Clock* clock;
	std::vector<Level*> vecLevels;

	int currentRows;
	int currentColumns;
	int screenMouseX;
	int screenMouseY;
};