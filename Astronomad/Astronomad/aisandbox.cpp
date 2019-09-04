#include "aisandbox.h"
#include "planet.h"
#include "sprite.h"
#include "cubemap.h"
#include "model.h"
#include "mothership.h"
#include "Swarmling.h"
#include "backstar.h"
#include "particle.h"
#include "buttonset.h"

AISandboxLevel::AISandboxLevel()
{
	isInit = false;
	boundW = 240.0f;
	boundH = 240.0f;
	currentWinner = FAC_NONE;
	levelDifficulty = 0; //0 = Easy, 1 = Hard
}

AISandboxLevel::~AISandboxLevel()
{
	while (vecObjects.size() > 0)
	{
		GameObject* temp = vecObjects[vecObjects.size() - 1];
		vecObjects.pop_back();
		delete temp;
	}

	while (vecParticles.size() > 0)
	{
		Particle* temp = vecParticles[vecParticles.size() - 1];
		vecParticles.pop_back();
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

void AISandboxLevel::Initialise(Game * _myGame, ShaderLoader * _shaderloader, AssetLoader * _assetloader, InputManager * _inputManager)
{
	myGame = _myGame;
	shaderLoader = _shaderloader;
	assetLoader = _assetloader;
	inputManager = _inputManager;
	levelCamera = new Camera();
	levelCamera->SetCamPos(glm::vec3(0.0f, 80.0f, 0.0f));

	glEnable(GL_TEXTURE_2D);
	glGenBuffers(1, &levelVBO);
	glBindBuffer(GL_ARRAY_BUFFER, levelVBO);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//Init Clock
	clock = new Clock();
	clock->Initialise();

	//Init Text
	textA = new Texter("", "Assets/Fonts/nasalization-rg.ttf", glm::vec3(-1000.0f, -600.0f, 2.0f), shaderLoader, assetLoader, levelCamera);
	textA->SetScale(0.5f);
	textA->SetColor(glm::vec3(1.0f, 1.0f, 1.0f));
	vecTexts.push_back(textA);

	hintTexter = new Texter("", "Assets/Fonts/nasalization-rg.ttf", glm::vec3(300.0f, -600.0f, 2.0f), shaderLoader, assetLoader, levelCamera);
	hintTexter->SetScale(0.5f);
	hintTexter->SetColor(glm::vec3(0.6f, 0.6f, 0.6f));
	vecTexts.push_back(hintTexter);

	//SandboxButtons
	sandboxButtons = new ButtonSet(clock);

	Button* tempButton = new Button("1| SEEK/FLEE", -1200.0f, 0.0f, this);
	vecObjects.push_back(tempButton);
	sandboxButtons->AddButton(tempButton);

	tempButton = new Button("2| PURSUE/EVADE", -1200.0f, -100.0f, this);
	vecObjects.push_back(tempButton);
	sandboxButtons->AddButton(tempButton);

	tempButton = new Button("3| PURSUE & ARRIVAL", -1200.0f, -200.0f, this);
	vecObjects.push_back(tempButton);
	sandboxButtons->AddButton(tempButton);

	tempButton = new Button("4| WANDER & CONTAINMENT", -1200.0f, -300.0f, this);
	vecObjects.push_back(tempButton);
	sandboxButtons->AddButton(tempButton);

	tempButton = new Button("5| FLOCKING & WANDER", -1200.0f, -400.0f, this);
	vecObjects.push_back(tempButton);
	sandboxButtons->AddButton(tempButton);

	tempButton = new Button("6| CROWD PATH FOLLOWING", -1200.0f, -500.0f, this);
	vecObjects.push_back(tempButton);
	sandboxButtons->AddButton(tempButton);


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
		bStar->SetX((float)(rand() % (int)boundW*4.0f) - (boundW * 2.0f));
		bStar->SetZ((float)(rand() % (int)boundH*4.0f) - (boundH));
		bStar->SetY(-1010.0f + 1.0f * (float)i);
		bStar->StarScale(((float)(rand() % 100) * 0.001f) + 0.01f);
		vecObjects.push_back(bStar);
	}

	InitPlanets();

	//Init Player Mothership
	player = new Mothership(this, FAC_RED);
	player->SetVecPlanets(&vecPlanets);
	player->SetVecParticles(&vecParticles);
	player->SetHomePlanet(mars);
	player->SetHintTexter(hintTexter);
	player->SetIsAI(false);
	player->SetIsDemo(true);
	player->SetX(90.0f);
	player->SetZ(90.0f);
	player->SetY(0.1f);
	player->Initialise();
	vecObjects.push_back(player);
	vecBoids.push_back(player);

	//Set camera to follow
	levelCamera->SetFollowTarget(player);
	levelCamera->SetIsFollowing(true);

	//Init Swarmling
	for (int i = 0; i < 400; i++)
	{
		Swarmling* swarmy = new Swarmling(this);
		swarmy->SetTarget(player);
		swarmy->SetVecOther(&vecBoids);
		swarmy->SetVecPlanets(&vecPlanets);
		swarmy->SetVecParticles(&vecParticles);
		swarmy->SetFactionMotherships(player, enemy);
		swarmy->SetY(-0.1f);
		swarmy->Initialise();
		swarmy->SetTargetMother(player);
		swarmy->SetTargetPlanet(mars);
		swarmy->SetState(SWA_MENU);
		swarmy->SetX((float)(rand() % (int)boundW) - (boundW * 0.5f));
		swarmy->SetZ((float)(rand() % (int)boundH) - (boundH * 0.5f));
		swarmy->SetIsActive(false);
		vecObjects.push_back(swarmy);
		vecBoids.push_back(swarmy);
		vecLevelSwarm.push_back(swarmy);
	}

	//Init Particles
	for (int i = 0; i < 15; i++)
	{
		Particle* particle = new Particle(this);
		particle->Initialise();
		particle->SetY(i * 1.0f - 10.0f);
		vecParticles.push_back(particle);
	}

	isInit = true;
}

void AISandboxLevel::Update()
{
	//Update time
	clock->Process();
	double deltaTime = clock->GetDeltaTick();

	//Camera
	levelCamera->SetCamUpDir(glm::vec3(0.0f, 0.0f, 1.0f));
	levelCamera->SetCamLookDir(glm::vec3(0.0f, -1.0f, 0.5f));

	levelCamera->ComputeView();
	levelCamera->ComputeProjection();
	levelCamera->ComputeFollow();

	ProcessKeyInput();

	//Update objects
	for (unsigned int i = 0; i < vecObjects.size(); i++)
	{
		if (vecObjects[i]->GetIsActive())
		{
			vecObjects[i]->Update(deltaTime);
		}
	}

	//Update particles
	for (unsigned int i = 0; i < vecParticles.size(); i++)
	{
		if (vecParticles[i]->GetIsActive())
		{
			vecParticles[i]->Update(deltaTime);
		}
	}

	//Update Text
	//std::string str = "FLEET | " + to_string(player->GetSwarmNum());
	//textA->SetText(str);

	float dist = glm::distance(glm::vec2(player->GetX(), player->GetZ()), glm::vec2(earth->GetX(), earth->GetZ()));

	glutPostRedisplay();
}

void AISandboxLevel::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.55f, 0.25f, 0.55f, 1.0f); //clear

											 //Draw objects
	for (unsigned int i = 0; i < vecObjects.size(); i++)
	{
		if (vecObjects[i]->GetIsActive())
		{
			vecObjects[i]->Draw();
		}
	}

	//Draw particles
	for (unsigned int i = 0; i < vecParticles.size(); i++)
	{
		if (vecParticles[i]->GetIsActive())
		{
			vecParticles[i]->Draw();
		}
	}

	//Draw texts
	for (unsigned int i = 0; i < vecTexts.size(); i++)
	{
		vecTexts[i]->Render();
	}

	glutSwapBuffers();
}

void AISandboxLevel::ProcessKeyInput()
{
	if (inputManager->GetKeyState(27) == KEY_DOWN) //Escape Key
	{
		ResetLevel();
		myGame->SetCurrentLevel(0); //Set level to Main Menu
		myGame->GetCurrentLevel()->ResetLevel();
		audioManager->PlayBGM(false);
	}
	
	static DemoState currentDemo = DEMO_SEEK;

	if (inputManager->GetKeyState('1') == KEY_DOWN)
	{
		sandboxButtons->SelectButton(0);
		currentDemo = DEMO_SEEK;
	}
	else if (inputManager->GetKeyState('2') == KEY_DOWN)
	{
		sandboxButtons->SelectButton(1);
		currentDemo = DEMO_PURSUE;
	}
	else if (inputManager->GetKeyState('3') == KEY_DOWN)
	{
		sandboxButtons->SelectButton(2);
		currentDemo = DEMO_ARRIVAL;
	}
	else if (inputManager->GetKeyState('4') == KEY_DOWN)
	{
		sandboxButtons->SelectButton(3);
		currentDemo = DEMO_WANDER;
	}
	else if (inputManager->GetKeyState('5') == KEY_DOWN)
	{
		sandboxButtons->SelectButton(4);
		currentDemo = DEMO_FLOCK;
	}
	else if (inputManager->GetKeyState('6') == KEY_DOWN)
	{
		sandboxButtons->SelectButton(5);
		currentDemo = DEMO_FOLLOW;
	}
	else
	{
		sandboxButtons->ResetPressTime();
	}

	for (unsigned int i = 0; i < vecLevelSwarm.size(); i++)
	{
		vecLevelSwarm[i]->SetDemoState(currentDemo);
		vecLevelSwarm[i]->SetTargetPlanet(sun);
	}

}

void AISandboxLevel::ResetLevel()
{
	clock->Initialise();

	//Reset Motherships
	player->SetX(mars->GetX());
	player->SetZ(mars->GetZ());
	player->ResetSpeeds();
	player->ClearVectors();
	player->SetIsAlive(true);

	//Reset Planets
	for (auto it = vecPlanets.begin(); it != vecPlanets.end(); it++)
	{
		Planet* planet = *it;

		if (planet->GetIsHomePlanet() == false)
		{
			planet->SetFaction(FAC_NONE);
			planet->ClearVectors();
		}
	}

	earth->SetFaction(FAC_NONE);
	earth->ClearVectors();

	mars->SetFaction(FAC_RED);
	mars->ClearVectors();

	//Reset Swarmlings
	int i = 0;
	for (auto it = vecLevelSwarm.begin(); it != vecLevelSwarm.end(); it++)
	{
		Swarmling* swarmy = *it;

		glm::vec2 velo = glm::vec2((float)(rand()%200 - 100), (float)(rand() % 200 - 100));
		velo = glm::normalize(velo);
		swarmy->SetVelocity(velo);
		swarmy->SetX((float)(rand() % (int)boundW) - (boundW * 0.5f));
		swarmy->SetZ((float)(rand() % (int)boundH) - (boundH * 0.5f));

		if (i < 200)
		{
			swarmy->SetIsActive(true);
			swarmy->SetState(SWA_DEMO);
			swarmy->SetFaction(FAC_RED);
			swarmy->SetTargetMother(player);
			swarmy->SetTargetPlanet(mars);
			mars->AddToSwarm(swarmy);
		}
		else
		{
			swarmy->SetIsActive(true);
			swarmy->SetState(SWA_DEMO);
			swarmy->SetFaction(FAC_BLUE);
			swarmy->SetTargetMother(player);
			swarmy->SetTargetPlanet(earth);
			mars->AddToSwarm(swarmy);
		}

		i++;
	}


	//Reset Particles
	for (auto it = vecParticles.begin(); it != vecParticles.end(); it++)
	{
		Particle* temp = *it;
		temp->SetIsActive(false);
	}
}

void AISandboxLevel::InitPlanets()
{
	//Init Blue Home Planet (Earth)
	earth = new Planet("EARTH", "Assets/Earth.png", this);
	earth->SetX(90.0f);
	earth->SetZ(-90.0f);
	earth->SetVecLevelSwarm(&vecLevelSwarm);
	earth->SetUniformScale(10.0f);
	earth->SetAvoidRadius(12.0f);
	earth->SetMaxSwarm(100);
	earth->Initialise();
	earth->SetIsHomePlanet(false);
	earth->SetIsDisplayText(false);
	earth->SetFaction(FAC_BLUE);
	vecObjects.push_back(earth);
	vecPlanets.push_back(earth);

	//Init Red Home Planet (Mars)
	mars = new Planet("MARS", "Assets/Mars.jpg", this);
	mars->SetX(-90.0f);
	mars->SetZ(90.0f);
	mars->SetVecLevelSwarm(&vecLevelSwarm);
	mars->SetUniformScale(9.0f);
	mars->SetAvoidRadius(11.0f);
	mars->SetMaxSwarm(100);
	mars->Initialise();
	mars->SetIsHomePlanet(false);
	mars->SetIsDisplayText(false);
	mars->SetFaction(FAC_RED);
	vecObjects.push_back(mars);
	vecPlanets.push_back(mars);

	//Init Sol
	sun = new Planet("SOL", "Assets/Sun.jpg", this);
	sun->SetX(0.0f);
	sun->SetZ(0.0f);
	sun->SetVecLevelSwarm(&vecLevelSwarm);
	sun->SetUniformScale(16.0f);
	sun->SetAvoidRadius(20.0f);
	sun->SetMaxSwarm(40);
	sun->Initialise();
	sun->SetIsHomePlanet(false);
	sun->SetIsDisplayText(false);
	sun->SetFaction(FAC_NONE);
	vecObjects.push_back(sun);
	vecPlanets.push_back(sun);
}
