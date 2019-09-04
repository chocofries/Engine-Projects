#pragma once
#include "game.h"
#include "gameobject.h"
#include "clock.h"
#include "rhythm.h"
#include "audiomanager.h"
#include "scoremanager.h"

class Game;
class Frog;
class HoriFrog;
class ScalpelHand;
class Tile;
class Background;
class Player;

class Level
{
public:
	Level();
	virtual ~Level();

	enum LevelState
	{
		LEVEL_TITLE,
		LEVEL_GAME,
		LEVEL_LOSE,
		LEVEL_WIN
	};

	virtual void Initialise(Game* _myGame, ShaderLoader* _shaderloader, TextureLoader* _textureLoader, InputManager* _inputManager, Clock* _clock);
	virtual void Update();
	virtual void Draw();

	void ProcessKeyInput();

	std::vector<Texter*>* GetVecTexts();
	bool GetIsInit() const;
	bool GetIsFever() const;
	Tile* GetTile(int _gridX, int _gridY);
	int GetDequeFrogsSize() const;
	Frog* GetFrog(int _dequeIndex);

	void SetScreenMouseX(int _x);
	void SetScreenMouseY(int _y);
	void SetIsFever(bool _isFever);
	void BreakHeart();

private:
	GLuint levelVBO;
	Game* myGame;
	ShaderLoader* shaderLoader;
	TextureLoader* textureLoader;
	InputManager* inputManager;
	AudioManager* audioManager;
	ScoreManager* scoreManager;
	Rhythm* rhythm;
	Clock* clock;
	FMOD::System* audioSystem;
	Camera* levelCamera;
	std::vector<GameObject*> vecObjects;
	std::vector<Texter*> vecTexts;
	std::deque<Frog*> dequeFrogs;
	ScalpelHand* scalpelHand;

	Player* player;
	GameObject* hearts[5]; //Array of hearts

	bool isInit;
	bool isFever;
	double deltaTime;

	int playerHearts;
	int screenMouseX;
	int screenMouseY;
	glm::vec3 rayWorld;
	glm::vec3 intersection;

	Tile* tileGrid[TILES_MAX_X][TILES_MAX_Y];

	Background* background;
	LevelState levelState;
	Texter* gameTextA;
	Texter* gameScoreText;

private:
	//Title Screen Stuff
	Texter* titleButtonStart;
	Texter* titleButtonExit;
	glm::vec3 textColorBlack;
	glm::vec3 textColorGray;
	int titleButtonSelected; //0 = PLAY, 1 = EXIT

	//Win Screen Stuff
	Texter* winText[5];

private:
	void InitLevelTitle();
	void InitLevelGame();
	void InitLevelWin();
	void LevelTitleUpdate();
	void LevelGameUpdate();
	void LevelLoseUpdate();
	void LevelWinUpdate();
};