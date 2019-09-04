#include "sceneSolarSystem.h"
#include "planet.h"
#include "sprite.h"
#include "cubemap.h"
#include "model.h"
#include "mothership.h"
#include "Swarmling.h"
#include "backstar.h"
#include "particle.h"

SceneSolarSystem::SceneSolarSystem()
{
	isInit = false;
	boundW = 300.0f;
	boundH = 300.0f;
	currentWinner = FAC_NONE;
	levelDifficulty = 0; //0 = Easy, 1 = Hard
}

SceneSolarSystem::~SceneSolarSystem()
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

void SceneSolarSystem::Initialise(Game * _myGame, ShaderLoader * _shaderloader, AssetLoader * _assetloader, InputManager * _inputManager)
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
	textA = new Texter("ARMADA: ", "Assets/Fonts/nasalization-rg.ttf", glm::vec3(-1000.0f, -600.0f, 2.0f), shaderLoader, assetLoader, levelCamera);
	textA->SetScale(0.5f);
	textA->SetColor(glm::vec3(1.0f, 1.0f, 1.0f));
	vecTexts.push_back(textA);

	hintTexter = new Texter("HINT", "Assets/Fonts/nasalization-rg.ttf", glm::vec3(300.0f, -600.0f, 2.0f), shaderLoader, assetLoader, levelCamera);
	hintTexter->SetScale(0.5f);
	hintTexter->SetColor(glm::vec3(0.6f, 0.6f, 0.6f));
	vecTexts.push_back(hintTexter);

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
	player = new Mothership(this,FAC_BLUE);
	player->SetVecPlanets(&vecPlanets);
	player->SetVecParticles(&vecParticles);
	player->SetFactionMotherships(player, enemy);
	player->SetHomePlanet(earth);
	player->SetHintTexter(hintTexter);
	player->SetIsAI(false);
	player->SetX(90.0f);
	player->SetZ(90.0f);
	player->SetY(0.1f);
	player->Initialise();
	vecObjects.push_back(player);
	vecBoids.push_back(player);

	//Init Enemy Mothership
	enemy = new Mothership(this, FAC_RED);
	enemy->SetVecPlanets(&vecPlanets);
	enemy->SetVecParticles(&vecParticles);
	enemy->SetFactionMotherships(player, enemy);
	enemy->SetHomePlanet(mars);
	enemy->SetHintTexter(hintTexter);
	enemy->SetIsAI(true);
	enemy->SetX(50.0f);
	enemy->SetZ(90.0f);
	enemy->SetY(0.1f);
	enemy->Initialise();
	vecObjects.push_back(enemy);
	vecBoids.push_back(enemy);

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

void SceneSolarSystem::Update()
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
	std::string str = "FLEET | " + to_string(player->GetSwarmNum());
	textA->SetText(str);

	float dist = glm::distance(glm::vec2(player->GetX(), player->GetZ()), glm::vec2(earth->GetX(), earth->GetZ()));

	if (enemy->GetAIState() == MOT_INVADE)
	{
		str = "ENEMY IS INVADING!";
		hintTexter->SetColor(glm::vec3(1.0f, 1.0f, 1.0f));
		hintTexter->SetText(str);
	}
	else if (dist < 20.0f)
	{
		str = "GATHER   |V|";

		hintTexter->SetColor(glm::vec3(0.6f, 0.6f, 0.6f));
		hintTexter->SetText(str);
	}
	else
	{
		str = "SHOOT   |SPACE|";
		hintTexter->SetColor(glm::vec3(0.6f, 0.6f, 0.6f));
		hintTexter->SetText(str);
	}

	glutPostRedisplay();
}

void SceneSolarSystem::Draw()
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

void SceneSolarSystem::ProcessKeyInput()
{
	if (inputManager->GetKeyState(27) == KEY_DOWN) //Escape Key
	{
		ResetLevel();
		myGame->SetCurrentLevel(0); //Set level to Main Menu
		myGame->GetCurrentLevel()->ResetLevel();
		audioManager->PlayBGM(false);
	}
}

void SceneSolarSystem::ResetLevel()
{
	clock->Initialise();

	//Reset Motherships
	player->SetX(earth->GetX());
	player->SetZ(earth->GetZ());
	player->ResetSpeeds();
	player->ClearVectors();
	player->SetIsAlive(true);

	enemy->SetX(mars->GetX());
	enemy->SetZ(mars->GetZ());
	enemy->ResetAI();
	enemy->ResetSpeeds();
	enemy->ClearVectors();
	enemy->SetIsAlive(true);

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

	earth->SetFaction(FAC_BLUE);
	earth->ClearVectors();

	mars->SetFaction(FAC_RED);
	mars->ClearVectors();

	if (levelDifficulty == 0)  //---------------- EASY MODE --------------------------------------
	{
		//Reset Swarmlings
		int i = 0;
		for (auto it = vecLevelSwarm.begin(); it != vecLevelSwarm.end(); it++)
		{
			Swarmling* swarmy = *it;

			swarmy->SetVelocity(glm::vec2(0.0f, 1.0f));

			if (i < 50) //Activate starting enemy swarm
			{
				swarmy->SetIsActive(true);
				swarmy->SetState(SWA_PLANET);
				swarmy->SetFaction(FAC_RED);
				swarmy->SetTargetMother(enemy);
				swarmy->SetTargetPlanet(mars);
				swarmy->SetX(mars->GetX());
				swarmy->SetZ(mars->GetZ());
				mars->AddToSwarm(swarmy);
			}
			else if (i >= 50 && i < 100) //Activate starting player swarm
			{
				swarmy->SetIsActive(true);
				swarmy->SetState(SWA_PLANET);
				swarmy->SetFaction(FAC_BLUE);
				swarmy->SetTargetMother(player);
				swarmy->SetTargetPlanet(earth);
				swarmy->SetX(earth->GetX());
				swarmy->SetZ(earth->GetZ());
				earth->AddToSwarm(swarmy);
			}
			else
			{
				swarmy->SetIsActive(false);
			}

			i++;
		}
	}
	else //---------------- HARD MODE --------------------------------------
	{
		//Reset Swarmlings
		int i = 0;
		for (auto it = vecLevelSwarm.begin(); it != vecLevelSwarm.end(); it++)
		{
			Swarmling* swarmy = *it;

			swarmy->SetVelocity(glm::vec2(0.0f, 1.0f));

			if (i < 70) //Activate starting enemy swarm
			{
				swarmy->SetIsActive(true);
				swarmy->SetState(SWA_PLANET);
				swarmy->SetFaction(FAC_RED);
				swarmy->SetTargetMother(enemy);
				swarmy->SetTargetPlanet(mars);
				swarmy->SetX(mars->GetX());
				swarmy->SetZ(mars->GetZ());
				mars->AddToSwarm(swarmy);
			}
			else if (i >= 70 && i < 100) //Activate starting player swarm
			{
				swarmy->SetIsActive(true);
				swarmy->SetState(SWA_PLANET);
				swarmy->SetFaction(FAC_BLUE);
				swarmy->SetTargetMother(player);
				swarmy->SetTargetPlanet(earth);
				swarmy->SetX(earth->GetX());
				swarmy->SetZ(earth->GetZ());
				earth->AddToSwarm(swarmy);
			}
			else
			{
				swarmy->SetIsActive(false);
			}

			i++;
		}
	}

	//Reset Particles
	for (auto it = vecParticles.begin(); it != vecParticles.end(); it++)
	{
		Particle* temp = *it;
		temp->SetIsActive(false);
	}
}

void SceneSolarSystem::InitPlanets()
{
	//Init Blue Home Planet (Earth)
	earth = new Planet("EARTH","Assets/Earth.png", this);
	earth->SetX(120.0f);
	earth->SetZ(-120.0f);
	earth->SetVecLevelSwarm(&vecLevelSwarm);
	earth->SetUniformScale(10.0f);
	earth->SetAvoidRadius(12.0f);
	earth->SetMaxSwarm(100);
	earth->Initialise();
	earth->SetIsHomePlanet(true);
	earth->SetIsDisplayText(true);
	earth->SetFaction(FAC_BLUE);
	vecObjects.push_back(earth);
	vecPlanets.push_back(earth);

	//Init Red Home Planet (Mars)
	mars = new Planet("MARS", "Assets/Mars.jpg", this);
	mars->SetX(-120.0f);
	mars->SetZ(120.0f);
	mars->SetVecLevelSwarm(&vecLevelSwarm);
	mars->SetUniformScale(9.0f);
	mars->SetAvoidRadius(11.0f);
	mars->SetMaxSwarm(100);
	mars->Initialise();
	mars->SetIsHomePlanet(true);
	mars->SetIsDisplayText(true);
	mars->SetFaction(FAC_RED);
	vecObjects.push_back(mars);
	vecPlanets.push_back(mars);

	//Init Moon
	Planet* planet = new Planet("MOON", "Assets/Moon.jpg", this);
	planet->SetX(100.0f);
	planet->SetZ(-60.0f);
	planet->SetVecLevelSwarm(&vecLevelSwarm);
	planet->SetUniformScale(4.0f);
	planet->SetAvoidRadius(5.0f);
	planet->SetMaxSwarm(15);
	planet->Initialise();
	planet->SetIsHomePlanet(false);
	planet->SetIsDisplayText(true);
	planet->SetFaction(FAC_NONE);
	vecObjects.push_back(planet);
	vecPlanets.push_back(planet);

	//Init Mercury
	planet = new Planet("MERCURY", "Assets/Mercury.jpg", this);
	planet->SetX(-60.0f);
	planet->SetZ(100.0f);
	planet->SetVecLevelSwarm(&vecLevelSwarm);
	planet->SetUniformScale(8.0f);
	planet->SetAvoidRadius(9.0f);
	planet->SetMaxSwarm(20);
	planet->Initialise();
	planet->SetIsHomePlanet(false);
	planet->SetIsDisplayText(true);
	planet->SetFaction(FAC_NONE);
	vecObjects.push_back(planet);
	vecPlanets.push_back(planet);

	//Init Venus
	planet = new Planet("VENUS", "Assets/Venus.jpg", this);
	planet->SetX(-50.0f);
	planet->SetZ(40.0f);
	planet->SetVecLevelSwarm(&vecLevelSwarm);
	planet->SetUniformScale(7.0f);
	planet->SetAvoidRadius(8.0f);
	planet->SetMaxSwarm(20);
	planet->Initialise();
	planet->SetIsHomePlanet(false);
	planet->SetIsDisplayText(true);
	planet->SetFaction(FAC_NONE);
	vecObjects.push_back(planet);
	vecPlanets.push_back(planet);

	//Init Sol
	planet = new Planet("SOL", "Assets/Sun.jpg", this);
	planet->SetX(30.0f);
	planet->SetZ(0.0f);
	planet->SetVecLevelSwarm(&vecLevelSwarm);
	planet->SetUniformScale(16.0f);
	planet->SetAvoidRadius(20.0f);
	planet->SetMaxSwarm(40);
	planet->Initialise();
	planet->SetIsHomePlanet(false);
	planet->SetIsDisplayText(true);
	planet->SetFaction(FAC_NONE);
	vecObjects.push_back(planet);
	vecPlanets.push_back(planet);
}
