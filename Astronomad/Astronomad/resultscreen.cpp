#include "resultscreen.h"
#include "swarmling.h"
#include "planet.h"
#include "cubemap.h"
#include "backstar.h"
#include "buttonset.h"

ResultsLevel::ResultsLevel()
{
	isInit = false;
	boundW = 200.0f;
	boundH = 200.0f;
	currentWinner = FAC_NONE;
}

ResultsLevel::~ResultsLevel()
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
}

void ResultsLevel::Initialise(Game * _myGame, ShaderLoader * _shaderloader, AssetLoader * _assetloader, InputManager * _inputManager)
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
	title = new Texter("RESULTS", "Assets/Fonts/nasalization-rg.ttf", glm::vec3(-500.0f, 500.0f, 2.0f), shaderLoader, assetLoader, levelCamera);
	title->SetScale(1.0f);
	title->SetColor(glm::vec3(1.0f, 1.0f, 1.0f));
	vecTexts.push_back(title);
	Texter* text = new Texter("PRESS ESCAPE TO EXIT TO MENU", "Assets/Fonts/nasalization-rg.ttf", glm::vec3(-580.0f, -500.0f, 2.0f), shaderLoader, assetLoader, levelCamera);
	text->SetScale(0.5f);
	text->SetColor(glm::vec3(0.6f, 0.6f, 0.6f));
	vecTexts.push_back(text);

	//Init Sky Box
	std::vector<std::string> vecCubeMapPaths;
	vecCubeMapPaths.push_back("right.png");
	vecCubeMapPaths.push_back("left.png");
	vecCubeMapPaths.push_back("top.png");
	vecCubeMapPaths.push_back("bot.png");
	vecCubeMapPaths.push_back("front.png");
	vecCubeMapPaths.push_back("back.png");
	skyBox = new CubeMap(this, &vecCubeMapPaths, "Assets/CubeMap/");
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
	Planet* mars = new Planet("MARS", "Assets/Mars.jpg", this);
	mars->SetX(0.0f);
	mars->SetZ(0.0f);
	mars->SetUniformScale(10.0f);
	mars->Initialise();
	mars->SetIsDisplayText(false);
	vecObjects.push_back(mars);
	vecPlanets.push_back(mars);

	//Init Swarmling
	for (int i = 0; i < 400; i++)
	{
		Swarmling* swarmy = new Swarmling(this);
		swarmy->SetTarget(mars);
		swarmy->SetVecPlanets(&vecPlanets);
		swarmy->SetVecOther(&vecBoids);
		swarmy->SetState(SWA_MENU);
		swarmy->SetFaction(FAC_RED);
		swarmy->Initialise();
		swarmy->SetX((float)(rand() % (int)boundW) - (boundW * 0.5f));
		swarmy->SetZ((float)(rand() % (int)boundH) - (boundH * 0.5f));
		vecObjects.push_back(swarmy);
		vecBoids.push_back(swarmy);
	}

	isInit = true;
}

void ResultsLevel::Update()
{
	//Update time
	clock->Process();
	GLfloat currentTime = (GLfloat)glutGet(GLUT_ELAPSED_TIME);

	//Update Text
	if (currentWinner == FAC_BLUE)
	{
		title->SetText("YOU ARE VICTORIOUS");
		title->SetPosition(glm::vec3(-680.0f, 500.0f, 2.0f));
		title->SetColor(glm::vec3(0.4f, 0.9f, 1.0f));
	}
	else
	{
		title->SetText("YOU HAVE BEEN DEFEATED");
		title->SetPosition(glm::vec3(-880.0f, 500.0f, 2.0f));
		title->SetColor(glm::vec3(1.0f, 0.4f, 0.4f));
	}


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

	ProcessKeyInput();

	//Update objects
	for (unsigned int i = 0; i < vecObjects.size(); i++)
	{
		if (vecObjects[i]->GetIsActive())
		{
			vecObjects[i]->Update(deltaTime);
		}
	}

	glutPostRedisplay();
}

void ResultsLevel::Draw()
{
	Level::Draw();
}

void ResultsLevel::ProcessKeyInput()
{
	if (inputManager->GetKeyState(27) == KEY_DOWN) //Escape Key
	{
		ResetLevel();
		myGame->SetCurrentLevel(0); //Set level to main menu
		myGame->GetCurrentLevel()->ResetLevel();
	}
}

void ResultsLevel::ResetLevel()
{
	for (auto it = vecBoids.begin(); it != vecBoids.end(); it++)
	{
		GameObject* swarmy = *it;
		clock->Initialise();
		swarmy->SetX((float)(rand() % (int)boundW) - (boundW * 0.5f));
		swarmy->SetZ((float)(rand() % (int)boundH) - (boundH * 0.5f));
	}
}
