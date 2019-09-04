#include "level.h"
#include "sprite.h"
#include "player.h"
#include "horifrog.h"
#include "vertifrog.h"
#include "omnifrog.h"
#include "scalpelhand.h"
#include "background.h"
#include "tile.h"
#include "heart.h"

Level::Level()
{
	isInit = false;
}

Level::~Level()
{
	while (vecObjects.size() > 0)
	{
		GameObject* temp = vecObjects[vecObjects.size() - 1];
		vecObjects.pop_back();
		delete temp;
	}

	while (vecTexts.size() > 0)
	{
		Texter* temp = vecTexts[vecTexts.size() - 1];
		vecTexts.pop_back();
		delete temp;
	}
	delete levelCamera;
	delete rhythm;
	delete audioManager;
	delete scoreManager;
	delete background;
}

void Level::Initialise(Game* _myGame, ShaderLoader* _shaderloader, TextureLoader* _textureloader, InputManager* _inputManager, Clock* _clock)
{
	shaderLoader = _shaderloader;
	textureLoader = _textureloader;
	inputManager = _inputManager;
	clock = _clock;
	levelCamera = new Camera();

	glEnable(GL_TEXTURE_2D);
	glGenBuffers(1, &levelVBO);
	glBindBuffer(GL_ARRAY_BUFFER, levelVBO);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//Init Level State
	levelState = LEVEL_TITLE;

	//Init Audio Manager
	audioManager = new AudioManager();
	audioManager->Initialise();

	//Init Score Manager
	scoreManager = new ScoreManager();

	//Init Rhythm Manager
	rhythm = new Rhythm(clock, inputManager, audioManager);
	rhythm->SetIsActive(true);
	rhythm->FindNextBeatPosition();

	//Init Background
	background = new Background();
	background->SetVBO(&levelVBO);
	background->SetCamera(levelCamera);
	background->SetShaderLoader(shaderLoader);
	background->SetInputManager(inputManager);
	background->SetTextureLoader(textureLoader);
	background->SetClock(clock);
	background->SetVecTexts(&vecTexts);
	background->Initialise();
	background->SetIsActive(true);

	//Init Level-Title
	InitLevelTitle();

	//Init Level-Game
	InitLevelGame();

	//init Win Screen
	InitLevelWin();
}

void Level::Update()
{
	deltaTime = clock->GetDeltaTick();

	//Camera
	levelCamera->ComputeView();
	levelCamera->ComputeProjection();

	ProcessKeyInput();

	//Update Audio
	audioManager->Update();

	//Update Background
	background->Update(deltaTime);

	if (levelState == LEVEL_GAME)
	{
		//Update Rhythm
		rhythm->Update();
		LevelGameUpdate();
	}
	else if (levelState == LEVEL_TITLE)
	{
		LevelTitleUpdate();
	}
	else if (levelState == LEVEL_LOSE)
	{
		LevelLoseUpdate();
	}
	else
	{
		if (levelState == LEVEL_WIN)
		{
			LevelWinUpdate();
		}
	}
}

void Level::Draw()
{
	deltaTime = clock->GetDeltaTick();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f); //clear

	//Draw Background
	background->Draw();

	if (levelState == LEVEL_GAME)
	{
		//Draw objects
		for (unsigned int i = 0; i < vecObjects.size(); i++)
		{
			if (vecObjects[i]->GetIsActive())
			{
				vecObjects[i]->Draw();
			}
		}
	}

	//Draw texts
	for (unsigned int i = 0; i < vecTexts.size(); i++)
	{
		vecTexts[i]->Render();
	}

	glutSwapBuffers();
	glFinish();
}

void Level::ProcessKeyInput()
{
	
}

std::vector<Texter*>* Level::GetVecTexts()
{
	return &vecTexts;
}

bool Level::GetIsInit() const
{
	return isInit;
}

bool Level::GetIsFever() const
{
	return isFever;
}

Tile * Level::GetTile(int _gridX, int _gridY)
{
	return tileGrid[_gridX][_gridY];
}

int Level::GetDequeFrogsSize() const
{
	int size = dequeFrogs.size();
	return size;
}

Frog* Level::GetFrog(int _dequeIndex)
{
	Frog* frog = dequeFrogs[_dequeIndex];
	return frog;
}

void Level::SetScreenMouseX(int _x)
{
	if (this != nullptr)
	{
		screenMouseX = _x;
	}
}


void Level::SetScreenMouseY(int _y)
{
	if (this != nullptr)
	{
		screenMouseY = _y;
	}
}

void Level::SetIsFever(bool _isFever)
{
	isFever = _isFever;
}

void Level::BreakHeart()
{
	if (playerHearts > 0)
	{
		playerHearts -= 1;
	}
}

void Level::InitLevelTitle()
{
	textColorBlack = glm::vec3(0.15f,0.15f,0.15f);
	textColorGray = glm::vec3(0.25f, 0.25f, 0.25f);

	//Title Buttons
	titleButtonStart = new Texter("PLAY", "Assets/Fonts/riffic-bold.ttf", glm::vec3(-60.0f, -180.0f, 1.0f), shaderLoader, textureLoader, levelCamera, false, textColorBlack, 100.0f);
	titleButtonExit = new Texter("EXIT", "Assets/Fonts/riffic-bold.ttf", glm::vec3(-60.0f, -320.0f, 1.0f), shaderLoader, textureLoader, levelCamera, false, textColorBlack, 100.0f);

	vecTexts.push_back(titleButtonStart);
	vecTexts.push_back(titleButtonExit);

	titleButtonSelected = 0; //Play button
}

void Level::InitLevelGame()
{
	//Init Hearts
	playerHearts = 5;

	for (int ht = 0; ht < 5; ht++) //Worms have five hearts
	{
		Heart* heart = new Heart(rhythm);
		heart->SetVBO(&levelVBO);
		heart->SetCamera(levelCamera);
		heart->SetShaderLoader(shaderLoader);
		heart->SetInputManager(inputManager);
		heart->SetTextureLoader(textureLoader);
		heart->SetClock(clock);
		heart->SetVecTexts(&vecTexts);
		heart->Initialise();
		heart->SetIsActive(true);
		heart->SetX(80.0f*(float)ht - 120.0f);
		heart->SetY(410.0f);
		hearts[ht] = heart;
		vecObjects.push_back(heart);
	}

	//Init Tiles
	bool tileCol = true;
	float tileOffX = ((float)TILE_WIDTH * 4.0f) - 70.0f;
	float tileOffY = ((float)TILE_HEIGHT * 4.0f) - 40.0f;
	for (int tx = 0; tx < TILES_MAX_X; tx++)
	{
		if (tileCol)
		{
			tileCol = false;
		}
		else
		{
			tileCol = true;
		}
		for (int ty = 0; ty < TILES_MAX_Y; ty++)
		{
			if (tileCol)
			{
				tileCol = false;
			}
			else
			{
				tileCol = true;
			}

			Tile* tile = new Tile(tx, ty, TILE_FREE, rhythm, tileCol, this);
			tile->SetVBO(&levelVBO);
			tile->SetCamera(levelCamera);
			tile->SetShaderLoader(shaderLoader);
			tile->SetInputManager(inputManager);
			tile->SetTextureLoader(textureLoader);
			tile->SetClock(clock);
			tile->SetVecTexts(&vecTexts);
			tile->Initialise();
			tile->SetX(((float)TILE_WIDTH*tx) - tileOffX);
			tile->SetY(((float)TILE_HEIGHT*ty) - tileOffY);
			tile->SetZ(-1.0f);
			tile->SetIsActive(true);
			tileGrid[tx][ty] = tile;
			vecObjects.push_back(tile);
		}
	}

	//Init Player
	player = new Player(rhythm, audioManager, scoreManager, this);
	player->SetVBO(&levelVBO);
	player->SetCamera(levelCamera);
	player->SetShaderLoader(shaderLoader);
	player->SetInputManager(inputManager);
	player->SetTextureLoader(textureLoader);
	player->SetClock(clock);
	player->SetVecTexts(&vecTexts);
	player->Initialise();
	player->SetIsActive(true);

	//Init Horifrog Enemies
	int counter = 0;
	for (int e = 0; e < 4; e++)
	{
		for (int r = 0; r < 4; r++)
		{
			HoriFrog* horifrog = new HoriFrog(rhythm, player, this, scoreManager, counter);
			horifrog->SetVBO(&levelVBO);
			horifrog->SetCamera(levelCamera);
			horifrog->SetShaderLoader(shaderLoader);
			horifrog->SetInputManager(inputManager);
			horifrog->SetTextureLoader(textureLoader);
			horifrog->SetClock(clock);
			horifrog->SetVecTexts(&vecTexts);
			horifrog->SetGridX(e);
			horifrog->SetGridY(r);
			horifrog->Initialise();
			horifrog->SetIsActive(false);
			horifrog->SetZ((float)counter);
			vecObjects.push_back(horifrog);
			dequeFrogs.push_front(horifrog);
			counter++;
		}
		counter++;
	}

	//Init Vertifrog Enemies
	for (int e = 0; e < 4; e++)
	{
		for (int r = 0; r < 4; r++)
		{
			VertiFrog* vertifrog = new VertiFrog(rhythm, player, this, scoreManager, counter);
			vertifrog->SetVBO(&levelVBO);
			vertifrog->SetCamera(levelCamera);
			vertifrog->SetShaderLoader(shaderLoader);
			vertifrog->SetInputManager(inputManager);
			vertifrog->SetTextureLoader(textureLoader);
			vertifrog->SetClock(clock);
			vertifrog->SetVecTexts(&vecTexts);
			vertifrog->SetGridX(e);
			vertifrog->SetGridY(r);
			vertifrog->Initialise();
			vertifrog->SetIsActive(false);
			vertifrog->SetZ((float)counter);
			vecObjects.push_back(vertifrog);
			dequeFrogs.push_front(vertifrog);
			counter++;
		}
		counter++;
	}

	//Init Omnifrog Enemies
	for (int o = 0; o < 4; o++)
	{
		OmniFrog* omnifrog = new OmniFrog(rhythm, player, this, scoreManager, counter);
		omnifrog->SetVBO(&levelVBO);
		omnifrog->SetCamera(levelCamera);
		omnifrog->SetShaderLoader(shaderLoader);
		omnifrog->SetInputManager(inputManager);
		omnifrog->SetTextureLoader(textureLoader);
		omnifrog->SetClock(clock);
		omnifrog->SetVecTexts(&vecTexts);
		omnifrog->SetGridX(o);
		omnifrog->SetGridY(o);
		omnifrog->Initialise();
		omnifrog->SetIsActive(false);
		omnifrog->SetZ((float)counter);
		vecObjects.push_back(omnifrog);
		dequeFrogs.push_front(omnifrog);
		counter++;
	}

	//Push player into objects vector
	vecObjects.push_back(player);

	//Init Scalpel Hand Enemy/Boss
	scalpelHand = new ScalpelHand(rhythm, player, this, scoreManager);
	scalpelHand->SetVBO(&levelVBO);
	scalpelHand->SetCamera(levelCamera);
	scalpelHand->SetShaderLoader(shaderLoader);
	scalpelHand->SetInputManager(inputManager);
	scalpelHand->SetTextureLoader(textureLoader);
	scalpelHand->SetClock(clock);
	scalpelHand->SetVecTexts(&vecTexts);
	scalpelHand->Initialise();
	scalpelHand->SetIsActive(false);
	vecObjects.push_back(scalpelHand);

	//Init Text
	gameScoreText = new Texter("Rhythm Worm!", "Assets/Fonts/riffic-bold.ttf", glm::vec3(-760.0f, -360.0f, 1.0f), shaderLoader, textureLoader, levelCamera, false);
	gameScoreText->SetScale(100.0f);
	gameScoreText->SetIsActive(false);
	vecTexts.push_back(gameScoreText);

	gameTextA = new Texter("SCORE", "Assets/Fonts/riffic-bold.ttf", glm::vec3(-760.0f, -280.0f, 1.0f), shaderLoader, textureLoader, levelCamera, false);
	gameTextA->SetIsActive(false);
	gameTextA->SetScale(60.0f);
	vecTexts.push_back(gameTextA);

	isInit = true;
}

void Level::InitLevelWin()
{
	//Title Buttons
	winText[0] = new Texter("MISSES: ", "Assets/Fonts/riffic-bold.ttf", glm::vec3(-260.0f, 80.0f, 1.0f), shaderLoader, textureLoader, levelCamera, false, textColorBlack, 50.0f);
	winText[1] = new Texter("FEVERS: ", "Assets/Fonts/riffic-bold.ttf", glm::vec3(-260.0f, 0.0f, 1.0f), shaderLoader, textureLoader, levelCamera, false, textColorBlack, 50.0f);
	winText[2] = new Texter("TOTAL SCORE: ", "Assets/Fonts/riffic-bold.ttf", glm::vec3(-260.0f, -80.0f, 1.0f), shaderLoader, textureLoader, levelCamera, false, textColorBlack, 50.0f);
	winText[3] = new Texter("RANK: ", "Assets/Fonts/riffic-bold.ttf", glm::vec3(-260.0f, -180.0f, 1.0f), shaderLoader, textureLoader, levelCamera, false, textColorBlack, 80.0f);
	winText[4] = new Texter("A", "Assets/Fonts/riffic-bold.ttf", glm::vec3(-60.0f, -190.0f, 1.0f), shaderLoader, textureLoader, levelCamera, false, glm::vec3(0.8f,0.2f,0.0f), 120.0f);

	for (int i = 0; i < 5; i++)
	{
		winText[i]->SetIsActive(false);
		vecTexts.push_back(winText[i]);
	}
}

void Level::LevelTitleUpdate()
{
	if (background->GetBackground() != BG_TITLE)
	{
		gameScoreText->SetIsActive(false);
		background->SetBackground(BG_TITLE);
		audioManager->PlayBGM(false);
		titleButtonStart->SetIsActive(true);
		titleButtonExit->SetIsActive(true);
	}

	//Choosing buttons using up/down arrows
	if (inputManager->GetArrowState(DIR_UP) == KEY_DOWN)
	{
		if (titleButtonSelected == 0)
		{
			titleButtonSelected = 1;
		}
		else if (titleButtonSelected == 1)
		{
			titleButtonSelected = 0;
		}
	}
	else if (inputManager->GetArrowState(DIR_DOWN) == KEY_DOWN)
	{
		if (titleButtonSelected == 0)
		{
			titleButtonSelected = 1;
		}
		else if (titleButtonSelected == 1)
		{
			titleButtonSelected = 0;
		}
	}

	//Changing color of text based on selected
	switch (titleButtonSelected)
	{
	case 0:
		titleButtonStart->SetColor(textColorGray);
		titleButtonStart->SetScale(120.0f);
		titleButtonExit->SetColor(textColorBlack);
		titleButtonExit->SetScale(100.0f);
		break;
	case 1:
		titleButtonStart->SetColor(textColorBlack);
		titleButtonStart->SetScale(100.0f);
		titleButtonExit->SetColor(textColorGray);
		titleButtonExit->SetScale(120.0f);
		break;
	}

	//Selecting using enter key
	if (inputManager->GetKeyState(13) == KEY_DOWN) //Enter key down
	{
		switch (titleButtonSelected)
		{
		case 0:
			levelState = LEVEL_GAME;
			titleButtonStart->SetIsActive(false);
			titleButtonExit->SetIsActive(false);
			break;
		case 1:
			glutLeaveMainLoop();
			break;
		}
	}
}

void Level::LevelGameUpdate()
{
	if (background->GetBackground() != BG_GAME)
	{
		isFever = false;
		for (int tx = 0; tx < TILES_MAX_X; tx++)
		{
			for (int ty = 0; ty < TILES_MAX_Y; ty++)
			{
				tileGrid[tx][ty]->Reset();
			}
		}
		scoreManager->Reset();
		rhythm->Reset();
		player->Reset();
		playerHearts = 5;
		gameScoreText->SetIsActive(true);
		gameTextA->SetIsActive(true);
		background->SetBackground(BG_GAME);
		audioManager->PlayBGM(true);
	}

	//Update Text
	//std::string str = std::to_string(audioManager->GetBGMPosition());
	std::string str = std::to_string(scoreManager->GetFinalScore());
	gameScoreText->SetText(str);

	//Update Objects
	for (unsigned int i = 0; i < vecObjects.size(); i++)
	{
		if (vecObjects[i]->GetIsActive())
		{
			vecObjects[i]->Update(deltaTime);
			glutPostRedisplay();
		}
	}

	//Update Hearts based on health
	for (int h = 0; h < 5; h++)
	{
		if (h > playerHearts - 1)
		{
			hearts[h]->SetIsActive(false);
		}
		else
		{
			hearts[h]->SetIsActive(true);
		}

		if (playerHearts <= 0)
		{
			//Deactivate frogs
			for (unsigned int i = 0; i < dequeFrogs.size(); i++)
			{
				dequeFrogs[i]->SetIsSpawned(false);
				dequeFrogs[i]->SetIsActive(false);
			}
			//Deactivate scalpel hand
			scalpelHand->SetIsActive(false);
			scalpelHand->SetIsAttacking(false);
			gameTextA->SetIsActive(false);
			levelState = LEVEL_LOSE;
		}
	}

	//Check if song is finished, then exit to win screen
	if (audioManager->GetBGMPosition() >= audioManager->GetBGMLength()*0.99f)
	{
		//Deactivate frogs
		for (unsigned int i = 0; i < dequeFrogs.size(); i++)
		{
			dequeFrogs[i]->SetIsSpawned(false);
			dequeFrogs[i]->SetIsActive(false);
		}
		//Deactivate scalpel hand
		scalpelHand->SetIsActive(false);
		scalpelHand->SetIsAttacking(false);
		gameTextA->SetIsActive(false);
		levelState = LEVEL_WIN;
	}

	//Activate Enemies based on beat
	if (rhythm->GetNextPosition() <= (int)audioManager->GetBGMPosition())
	{
		for (unsigned int i = 0; i < dequeFrogs.size(); i++)
		{
			int rng = rand() % dequeFrogs.size();
			if (dequeFrogs[rng]->GetIsSpawned() == false)
			{
				dequeFrogs[rng]->Spawn();
				rhythm->FindNextBeatPosition();
				break;
			}
		}
	}

	//Activate Scalpel-Hand Boss when song is at 3/4 position
	if (audioManager->GetBGMPosition() >= audioManager->GetBGMLength()*0.75f)
	{
		if (scalpelHand->GetIsActive() == false)
		{
			scalpelHand->SetIsActive(true);
			scalpelHand->SetIsAttacking(true);
		}
	}


	//Returning to title screen
	if (inputManager->GetKeyState(27) == KEY_DOWN) //Escape key down
	{
		//Deactivate frogs
		for (unsigned int i = 0; i < dequeFrogs.size(); i++)
		{
			dequeFrogs[i]->SetIsSpawned(false);
			dequeFrogs[i]->SetIsActive(false);
		}
		//Deactivate scalpel hand
		scalpelHand->SetIsActive(false);
		scalpelHand->SetIsAttacking(false);
		gameTextA->SetIsActive(false);
		levelState = LEVEL_TITLE;
	}
}

void Level::LevelLoseUpdate()
{
	if (background->GetBackground() != BG_LOSE)
	{
		gameScoreText->SetIsActive(false);
		background->SetBackground(BG_LOSE);
		audioManager->PlayBGM(false);
		scalpelHand->SetIsActive(false);
		scalpelHand->SetIsAttacking(false);
	}

	//Returning to title screen
	if (inputManager->GetKeyState(27) == KEY_DOWN) //Escape key down
	{
		levelState = LEVEL_TITLE;
	}
}

void Level::LevelWinUpdate()
{
	if (background->GetBackground() != BG_WIN)
	{
		for (int i = 0; i < 5; i++)
		{
			winText[i]->SetIsActive(true);
		}
		scalpelHand->SetIsActive(false);
		scalpelHand->SetIsAttacking(false);
		gameScoreText->SetIsActive(false);
		background->SetBackground(BG_WIN);
		audioManager->PlayBGM(false);
	}

	//Update score texts
	std::string str = "MISSES: ";
	str += std::to_string(scoreManager->GetBadHits());
	winText[0]->SetText(str);

	str = "FEVERS: ";
	str += std::to_string(scoreManager->GetBestHits());
	winText[1]->SetText(str);

	str = "TOTAL SCORE: ";
	str += std::to_string(scoreManager->GetFinalScore());
	winText[2]->SetText(str);

	int finalScore = scoreManager->GetFinalScore();

	if (finalScore >= S_RANK)
	{
		winText[4]->SetText("S");
	}
	else if (finalScore >= A_RANK)
	{
		winText[4]->SetText("A");
	}
	else if (finalScore >= B_RANK)
	{
		winText[4]->SetText("B");
	}
	else if (finalScore >= C_RANK)
	{
		winText[4]->SetText("C");
	}
	else if (finalScore >= D_RANK)
	{
		winText[4]->SetText("D");
	}
	else if (finalScore >= E_RANK)
	{
		winText[4]->SetText("E");
	}
	else if (finalScore >= F_RANK)
	{
		winText[4]->SetText("F");
	}

	//Returning to title screen
	if (inputManager->GetKeyState(27) == KEY_DOWN) //Escape key down
	{
		for (int i = 0; i < 5; i++)
		{
			winText[i]->SetIsActive(false);
		}
		levelState = LEVEL_TITLE;
	}
}
