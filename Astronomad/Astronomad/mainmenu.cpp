#include "mainmenu.h"
#include "swarmling.h"
#include "planet.h"
#include "cubemap.h"
#include "backstar.h"
#include "buttonset.h"
#include "netmanager.h"

MainMenu::MainMenu()
{
	isInit = false;
	boundW = 200.0f;
	boundH = 200.0f;
	menuState = MENU_NORM;
}

MainMenu::~MainMenu()
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
	delete clock;
	delete levelCamera;
	delete menuButtons;
}

void MainMenu::Initialise(Game * _myGame, ShaderLoader * _shaderloader, AssetLoader * _assetloader, InputManager * _inputManager)
{
	myGame = _myGame;
	shaderLoader = _shaderloader;
	assetLoader = _assetloader;
	inputManager = _inputManager;
	levelCamera = new Camera();

	glEnable(GL_TEXTURE_2D);
	glGenBuffers(1, &levelVBO);
	glBindBuffer(GL_ARRAY_BUFFER, levelVBO);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//Init Clock
	clock = new Clock();
	clock->Initialise();

	//Init Text
	Texter* title = new Texter("A MINIMALIST SPACE RTS", "Assets/Fonts/nasalization-rg.ttf", glm::vec3(60.0f, 450.0f, 2.0f), shaderLoader, assetLoader, levelCamera);
	title->SetScale(0.25f);
	title->SetColor(glm::vec3(0.8f, 0.8f, 0.8f));
	vecTexts.push_back(title);

	title = new Texter("ASTRONOMAD", "Assets/Fonts/nasalization-rg.ttf", glm::vec3(-500.0f, 500.0f, 2.0f), shaderLoader, assetLoader, levelCamera);
	title->SetScale(1.0f);
	title->SetColor(glm::vec3(1.0f, 1.0f, 1.0f));
	vecTexts.push_back(title);

	title = new Texter("CREATED BY JOHN BU | NOVEMBER 2018", "Assets/Fonts/nasalization-rg.ttf", glm::vec3(-360.0f, -680.0f, 2.0f), shaderLoader, assetLoader, levelCamera);
	title->SetScale(0.25f);
	title->SetColor(glm::vec3(0.8f, 0.8f, 0.8f));
	vecTexts.push_back(title);

	//Init Menu Buttons
	menuButtons = new ButtonSet(clock);

	Button* tempButton = new Button("SKIRMISH | EASY", 440.0f, 0.0f, this);
	vecObjects.push_back(tempButton);
	menuButtons->AddButton(tempButton);

	tempButton = new Button("SKIRMISH | HARD", 440.0f, -128.0f, this);
	vecObjects.push_back(tempButton);
	menuButtons->AddButton(tempButton);

	tempButton = new Button("ONLINE | CO-OP", 440.0f, -256.0f, this);
	vecObjects.push_back(tempButton);
	menuButtons->AddButton(tempButton);

	tempButton = new Button("NAME | PLAYER", 440.0f, -384.0f, this);
	vecObjects.push_back(tempButton);
	menuButtons->AddButton(tempButton);

	tempButton = new Button("EXIT GAME", 440.0f, -512.0f, this);
	vecObjects.push_back(tempButton);
	menuButtons->AddButton(tempButton);

	//Init Sky Box
	std::vector<std::string> vecCubeMapPaths;
	vecCubeMapPaths.push_back("right.png");
	vecCubeMapPaths.push_back("left.png");
	vecCubeMapPaths.push_back("top.png");
	vecCubeMapPaths.push_back("bot.png");
	vecCubeMapPaths.push_back("front.png");
	vecCubeMapPaths.push_back("back.png");
	skyBox = new CubeMap(this, &vecCubeMapPaths, "Assets/CubeMap2/");
	skyBox->Initialise();
	vecObjects.push_back(skyBox);

	//Init Stars
	for (int i = 0; i < 1000; i++)
	{
		BackStar* bStar = new BackStar(this);
		bStar->Initialise();
		bStar->SetX((float)(rand() % (int)boundW*8.0f) - (boundW*4.0f));
		bStar->SetZ((float)(rand() % (int)boundH*8.0f) - (boundH*4.0f));
		bStar->SetY(-500.0f + 1.0f * (float)i);
		bStar->StarScale(((float)(rand() % 100) * 0.001f) + 0.01f);
		vecObjects.push_back(bStar);
	}

	//Init Planet
	Planet* earth = new Planet("Earth", "Assets/Earth.png",this);
	earth->SetX(0.0f);
	earth->SetZ(0.0f);
	earth->SetUniformScale(10.0f);
	earth->Initialise();
	earth->SetIsDisplayText(false);
	vecObjects.push_back(earth);
	vecPlanets.push_back(earth);

	//Camera
	//levelCamera->SetFollowTarget(earth);
	//levelCamera->SetIsFollowing(true);

	//Init Swarmling
	for (int i = 0; i < 40; i++)
	{
		Swarmling* swarmy = new Swarmling(this);
		swarmy->SetTarget(earth);
		swarmy->SetVecPlanets(&vecPlanets);
		swarmy->SetVecOther(&vecBoids);
		swarmy->SetState(SWA_MENU);
		swarmy->Initialise();
		swarmy->SetX((float)(rand() % (int)boundW) - (boundW * 0.5f));
		swarmy->SetZ((float)(rand() % (int)boundH) - (boundH * 0.5f));
		vecObjects.push_back(swarmy);
		vecBoids.push_back(swarmy);
	}

	isInit = true;
}

void MainMenu::Update()
{
	//Update time
	clock->Process();
	GLfloat currentTime = (GLfloat)glutGet(GLUT_ELAPSED_TIME);

	float orbitSpeed = -0.1f;
	float orbitDistance = 40.0f;
	currentTime = (currentTime / 1000.0f)*orbitSpeed;
	double deltaTime = clock->GetDeltaTick();

	//Camera
	glm::vec3 camRotate = glm::vec3(sin(currentTime), 0, cos(currentTime));
	levelCamera->SetCamPos(glm::vec3(sin(currentTime)*orbitDistance, 5.0f, cos(currentTime)*orbitDistance));
	levelCamera->SetCamUpDir(glm::vec3(0.0f, 1.0f, 0.0f));
	levelCamera->SetCamLookDir(-camRotate);

	levelCamera->ComputeView();
	levelCamera->ComputeProjection();
	//levelCamera->ComputeFollow();

	ProcessKeyInput();

	//Update objects
	for (unsigned int i = 0; i < vecObjects.size(); i++)
	{
		if (vecObjects[i]->GetIsActive())
		{
			vecObjects[i]->Update(deltaTime);
		}
	}

	//Update Username Text
	menuButtons->SetSingleText(3, "NAME | " + myGame->GetUsername());

	glutPostRedisplay();
}

void MainMenu::Draw()
{
	Level::Draw();
}

void MainMenu::ProcessKeyInput()
{
	static std::string customName;

	if (menuState == MENU_NORM)
	{
		if (inputManager->GetArrowState(DIR_DOWN) == KEY_DOWN)
		{
			menuButtons->NextButton();
		}
		if (inputManager->GetArrowState(DIR_UP) == KEY_DOWN)
		{
			menuButtons->PrevButton();
		}

		if (inputManager->GetKeyState(13) == KEY_DOWN && menuButtons->GetEnterPressed() == false) //Enter Key
		{
			menuButtons->SetEnterPressed(true);

			switch (menuButtons->GetCurrentID())
			{
			case 0: //Skirmish - Easy
			{
				ResetLevel();
				myGame->SetCurrentLevel(1); //Set level to Solar System Singleplayer
				myGame->GetCurrentLevel()->SetLevelDifficulty(0); //Easy
				myGame->GetCurrentLevel()->ResetLevel();
				audioManager->PlayBGM(true);
				break;
			}
			case 1:  //Skirmish - Hard
			{
				ResetLevel();
				myGame->SetCurrentLevel(1); //Set level to Solar System Singleplayer
				myGame->GetCurrentLevel()->SetLevelDifficulty(1); //Hard
				myGame->GetCurrentLevel()->ResetLevel();
				audioManager->PlayBGM(true);
				break;
			}
			case 2:  //Multiplayer
			{
				myGame->SetCurrentLevel(3); //Set level to online level
				myGame->GetCurrentLevel()->SetLevelDifficulty(2); //Insane
				myGame->GetCurrentLevel()->ResetLevel();
				audioManager->PlayBGM(true);
				ResetLevel();
				break;
			}
			case 3: //User Name
			{
				menuState = MENU_NAME;
				customName = "";
				menuButtons->SetSingleState(3, BS_SPECIAL);
				break;
			}
			case 4: //Exit Game
			{
				myGame->GetNetManager()->SetGameIsEnd(true);
				glutLeaveMainLoop(); //Exit the game
				break;
			}
			default:
				break;
			}
		}
		else if (inputManager->GetKeyState(13) == KEY_UP && menuButtons->GetEnterPressed() == true)
		{
			menuButtons->SetEnterPressed(false);
		}

		if ((inputManager->GetArrowState(DIR_DOWN) == KEY_UP) &&
			(inputManager->GetArrowState(DIR_UP) == KEY_UP))
		{
			menuButtons->ResetPressTime();
		}
	}
	else if (menuState == MENU_NAME)
	{
		static bool pressed = false;
		static bool backspaced = false;
		unsigned char lastKeyDown = inputManager->GetLastKeyDown();


		if (isalnum(lastKeyDown) && pressed == false) //If the last key was alphanumeric
		{
			if (inputManager->GetKeyState(lastKeyDown) == KEY_DOWN)
			{
				pressed = true;
				customName += toupper(lastKeyDown);
			}
		}
		else if (pressed == true)
		{
			if (inputManager->GetKeyState(lastKeyDown) == KEY_UP)
			{
				pressed = false;
			}
		}

		if (inputManager->GetKeyState(8) == KEY_DOWN && backspaced == false) //Back Space
		{
			if (customName.size() > 0)
			{
				customName.pop_back();
			}

			backspaced = true;
		}
		else if (inputManager->GetKeyState(8) == KEY_UP && backspaced == true)
		{
			backspaced = false;
		}

		myGame->SetUsername(customName);

		if (inputManager->GetKeyState(13) == KEY_DOWN && menuButtons->GetEnterPressed() == false) //Enter Key
		{
			if (GetUsername().size() <= 0 || GetUsername().size() > 7)
			{
				customName = "PLAYER";
				myGame->SetUsername(customName);
			}
			menuState = MENU_NORM;
			menuButtons->SetSingleState(3, BS_HOVER);
			menuButtons->SetEnterPressed(true);
		}
		else if (inputManager->GetKeyState(13) == KEY_UP && menuButtons->GetEnterPressed() == true)
		{
			menuButtons->SetEnterPressed(false);
		}


	}
}

void MainMenu::ResetLevel()
{
	menuButtons->SetEnterPressed(true);
	for (auto it = vecBoids.begin(); it != vecBoids.end(); it++)
	{
		GameObject* swarmy = *it;
		clock->Initialise();
		swarmy->SetX((float)(rand() % (int)boundW) - (boundW * 0.5f));
		swarmy->SetZ((float)(rand() % (int)boundH) - (boundH * 0.5f));
	}
}
