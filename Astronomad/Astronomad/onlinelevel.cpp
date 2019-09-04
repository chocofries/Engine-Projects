#include "onlinelevel.h"
#include "swarmling.h"
#include "planet.h"
#include "mothership.h"
#include "cubemap.h"
#include "backstar.h"
#include "buttonset.h"
#include "netmanager.h"
#include "particle.h"

OnlineLevel::OnlineLevel()
{
	isInit = false;
	boundW = 300.0f;
	boundH = 300.0f;
	state = ONLINE_MENU;
	isServerChosen = false;
	myShipID = 0;
}

OnlineLevel::~OnlineLevel()
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
	delete menuButtons;
	delete joinButtons;
}

void OnlineLevel::Initialise(Game * _myGame, ShaderLoader * _shaderloader, AssetLoader * _assetloader, InputManager * _inputManager)
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

	//Titles
	title = new Texter("ONLINE - COOP", "Assets/Fonts/nasalization-rg.ttf", glm::vec3(-500.0f, 500.0f, 2.0f), shaderLoader, assetLoader, levelCamera);
	title->SetScale(1.0f);
	title->SetColor(glm::vec3(1.0f, 1.0f, 1.0f));
	vecTexts.push_back(title);

	//title = new Texter("CREATED BY JOHN BU | NOVEMBER 2018", "Assets/Fonts/nasalization-rg.ttf", glm::vec3(-360.0f, -680.0f, 2.0f), shaderLoader, assetLoader, levelCamera);
	//title->SetScale(0.25f);
	//title->SetColor(glm::vec3(0.8f, 0.8f, 0.8f));
	//vecTexts.push_back(title);

	//Game UI
	textA = new Texter("", "Assets/Fonts/nasalization-rg.ttf", glm::vec3(-1000.0f, -600.0f, 2.0f), shaderLoader, assetLoader, levelCamera);
	textA->SetScale(0.5f);
	textA->SetColor(glm::vec3(1.0f, 1.0f, 1.0f));
	vecTexts.push_back(textA);

	//Init Menu Buttons
	menuButtons = new ButtonSet(clock);

	Button* tempButton = new Button("HOST GAME", 0.0f, 0.0f, this);
	vecObjects.push_back(tempButton);
	menuButtons->AddButton(tempButton);

	tempButton = new Button("JOIN GAME", 0.0f, -128.0f, this);
	vecObjects.push_back(tempButton);
	menuButtons->AddButton(tempButton);

	tempButton = new Button("RETURN", 0.0f, -256.0f, this);
	vecObjects.push_back(tempButton);
	menuButtons->AddButton(tempButton);

	menuButtons->SelectButton(0);

	//Init Server Buttons
	joinButtons = new ButtonSet(clock);

	for (unsigned int i = 0; i < 5; i++)
	{
		std::string serverID = "SERVER | " + to_string(i);
		Button* temp = new Button(serverID, 0.0f, -128.0f * (float)i, this);
		vecObjects.push_back(temp);
		joinButtons->AddButton(temp);
	}

	joinButtons->SelectButton(0);
	joinButtons->SetEveryIsActive(false);

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

	//Init Planet
	InitPlanets();

	//Camera
	levelCamera->SetFollowTarget(earth);
	levelCamera->SetIsFollowing(true);

	//Planet* earth = new Planet("Earth", "Assets/Earth.png", this);
	//earth->SetX(0.0f);
	//earth->SetZ(0.0f);
	//earth->SetVecLevelSwarm(&vecLevelSwarm);
	//earth->SetVecExcess(&vecExcess);
	//earth->SetIsNetworked(false);
	//earth->SetPlanetID(0);
	//earth->SetMaxSwarm(100);
	//earth->SetUniformScale(10.0f);
	//earth->SetFaction(FAC_BLUE);
	//earth->SetIsHomePlanet(true);
	//earth->SetIsDisplayText(true);
	//earth->Initialise();
	//vecObjects.push_back(earth);
	//vecPlanets.push_back(earth);


	//Init Players
	for (int i = 0; i < 4; i++)
	{
		Mothership* testShip = new Mothership(this, FAC_BLUE);
		testShip->SetVecPlanets(&vecPlanets);
		testShip->SetVecExcess(&vecExcess);
		testShip->SetVecParticles(&vecParticles);
		testShip->SetHomePlanet(earth);
		testShip->SetIsAI(false);
		testShip->SetIsNetworked(true);
		testShip->SetIsActive(false);
		testShip->SetX(earth->GetX() + ((float)(rand() % 20) - 10.0f));
		testShip->SetZ(earth->GetZ() + ((float)(rand() % 20) - 10.0f));
		testShip->SetY(0.1f);
		testShip->Initialise();
		testShip->SetShipID(i);
		vecObjects.push_back(testShip);
		vecPlayers.push_back(testShip);
	}

	//Init Enemy Mothership
	enemy = new Mothership(this, FAC_RED);
	enemy->SetVecPlanets(&vecPlanets);
	enemy->SetVecExcess(&vecExcess);
	enemy->SetVecParticles(&vecParticles);
	enemy->SetFactionMotherships(vecPlayers[myShipID], enemy);
	enemy->SetHomePlanet(mars);
	enemy->SetIsNetworked(false);
	enemy->SetIsAI(true);
	enemy->ResetAI();
	enemy->SetX(50.0f);
	enemy->SetZ(90.0f);
	enemy->SetY(0.1f);
	enemy->SetShipID(9);
	enemy->Initialise();
	//vecPlayers.push_back(enemy);
	vecObjects.push_back(enemy);
	vecBoids.push_back(enemy);

	//Init Swarmling
	for (int i = 0; i < 500; i++)
	{
		Swarmling* swarmy = new Swarmling(this);
		//swarmy->SetTargetMother(vecPlayers[myShipID]);
		swarmy->SetVecPlanets(&vecPlanets);
		swarmy->SetVecOther(&vecBoids);
		swarmy->SetVecParticles(&vecParticles);
		swarmy->SetFactionMotherships(vecPlayers[myShipID], enemy);
		swarmy->SetTargetMother(vecPlayers[myShipID]);
		swarmy->SetState(SWA_PLANET);
		swarmy->Initialise();
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

void OnlineLevel::Update()
{
	//Update time
	clock->Process();
	GLfloat currentTime = (GLfloat)glutGet(GLUT_ELAPSED_TIME);

	float orbitSpeed = -0.5f;
	float orbitDistance = 40.0f;
	currentTime = (currentTime / 1000.0f)*orbitSpeed;
	deltaTime = (float)clock->GetDeltaTick();

	//Camera
	//glm::vec3 camRotate = glm::vec3(sin(currentTime), 0, cos(currentTime));
	//levelCamera->SetCamPos(glm::vec3(sin(currentTime)*orbitDistance, 2.0f, cos(currentTime)*orbitDistance));
	//levelCamera->SetCamUpDir(glm::vec3(0.0f, 1.0f, 0.0f));
	//levelCamera->SetCamLookDir(-camRotate);

	//Camera
	levelCamera->SetCamUpDir(glm::vec3(0.0f, 0.0f, 1.0f));
	levelCamera->SetCamLookDir(glm::vec3(0.0f, -1.0f, 0.5f));

	levelCamera->ComputeView();
	levelCamera->ComputeProjection();
	levelCamera->ComputeFollow();

	ProcessKeyInput();

	//Ensure threads are started if not already -------------------------------------------
	if (netManager->GetInstance().GetNetworkEntity() != nullptr && netManager->GetIsInit())
	{
		if (!threadIsInit) //Initialize Threads for receive or keepalive
		{
			netManager->StartThreads();
			threadIsInit = true;
		}
		else //If threads have been started
		{
			netManager->Update(); //Update the netmanager
		}
	}
	//-------------------------------------------------------------------------------------

	if (state == ONLINE_GAME)  //Update text
	{
		vecPlayers[myShipID]->SetShipName(GetUsername());
		std::string str = GetUsername() + "'S FLEET | " + to_string(vecPlayers[entityID]->GetSwarmNum());
		textA->SetText(str);
		textA->SetIsActive(true);
		title->SetIsActive(false);
	}
	else
	{
		textA->SetIsActive(false);
		title->SetIsActive(true);

	}

	if (state == ONLINE_JOIN)
	{
		//Iterate through servers found by broad cast and activate that number of join buttons (up to 5)
		if (netManager->ClientServersFound() && isServerChosen == false)
		{
			for (int i = 0; i < netManager->GetClientVecServersSize(); i++)
			{
				if (i >= 5) { break; } //Ensure no more than 5

				std::string strName;
				if (!netManager->GetClientVecServerName()->empty())
				{
					strName = netManager->GetClientVecServerName()->at(i) + "'S SERVER";
				}
				else
				{
					strName = "MYSTERY SERVER";
				}

				joinButtons->SetSingleText(i, strName);
				joinButtons->SetSingleIsActive(i, true);	//Set that specific button as active
				joinButtons->SetButtonNum(i+1);				//Set number of active buttons as this
			}
		}
	}
	else if (state == ONLINE_GAME)
	{
		entityID = netManager->GetEntityID();
		levelCamera->SetFollowTarget(vecPlayers[entityID]);
		levelCamera->SetIsFollowing(true);

		if (netManager != nullptr)
		{
			NetworkUpdate(); //Serialize then send game update data to everyone else
		}

		for (unsigned int i = 0; i < vecPlanets.size(); i++)
		{
			vecPlanets[i]->SetIsActive(true);
			vecPlanets[i]->SetIsDisplayText(true);
		}

		/*for (unsigned int i = 0; i < vecPlayers.size(); i++)
		{
			if (vecPlayers[i]->GetIsNetworked())
			{
				vecPlayers[i]->SetIsActive(false);
			}
		}*/
	}
	else
	{
		for (unsigned int i = 0; i < vecPlanets.size(); i++)
		{
			vecPlanets[i]->SetIsDisplayText(false);
		}
	}

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

	glutPostRedisplay();
}

void OnlineLevel::Draw()
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

void OnlineLevel::ProcessKeyInput()
{
	if (inputManager->GetArrowState(DIR_DOWN) == KEY_DOWN)
	{
		menuButtons->NextButton();
		joinButtons->NextButton();
	}
	if (inputManager->GetArrowState(DIR_UP) == KEY_DOWN)
	{
		menuButtons->PrevButton();
		joinButtons->PrevButton();
	}

	if (inputManager->GetKeyState(13) == KEY_DOWN && menuButtons->GetEnterPressed() == false) //Enter Key
	{
		menuButtons->SetEnterPressed(true);
		joinButtons->SetEnterPressed(true);

		if (state == ONLINE_MENU) //--------------------------------------------------------- ONLINE MENU -------------------------------------------
		{
			switch (menuButtons->GetCurrentID())
			{
			case 0: //Host Game
			{
				state = ONLINE_HOST;

				if (!netManager->GetIsInit())
				{
					netManager = &NetManager::GetInstance();
					netManager->StartUp();
				}

				if (!netManager->GetInstance().Initialise(SERVER, myGame))
				{
					std::cout << "Unable to initialise the Network...";
				}
				//server = static_cast<Server*>(netManager->GetInstance().GetNetworkEntity());

				menuButtons->SetEveryIsActive(false);

				//testShip->SetIsNetworked(true);
				state = ONLINE_GAME;

				//Update Active objects
				for (unsigned int i = 0; i < vecPlayers.size(); i++)
				{
					vecPlayers[i]->SetIsActive(true);
				}
				vecPlayers[myShipID]->SetX(earth->GetX());
				vecPlayers[myShipID]->SetZ(earth->GetZ());

				enemy->SetIsActive(true);
				earth->SetIsActive(true);
				mars->SetIsActive(true);

				for (unsigned int i = 0; i < vecLevelSwarm.size(); i++)
				{
					Swarmling* swarmy = vecLevelSwarm[i];

					swarmy->SetVelocity(glm::vec2(0.0f, 1.0f));

					if (i < 50) //Activate starting player swarm
					{
						swarmy->SetTargetMother(vecPlayers[myShipID]);
						swarmy->SetTargetPlanet(earth);
						swarmy->SetFaction(FAC_BLUE);
						swarmy->SetState(SWA_PLANET);
						swarmy->SetX(earth->GetX() + (float)(rand() % 20) - 10.0f);
						swarmy->SetZ(earth->GetZ() + (float)(rand() % 20) - 10.0f);
						earth->AddToSwarm(swarmy);
						swarmy->SetIsActive(true);
					}
					else if (i >= 50 && i < 200) //Activate starting enemy swarm
					{
						swarmy->SetTargetMother(enemy);
						swarmy->SetTargetPlanet(mars);
						swarmy->SetTarget(mars);
						swarmy->SetFaction(FAC_RED);
						swarmy->SetState(SWA_PLANET);
						swarmy->SetX(mars->GetX() + (float)(rand() % 20) - 10.0f);
						swarmy->SetZ(mars->GetZ() + (float)(rand() % 20) - 10.0f);
						mars->AddToSwarm(swarmy);
						swarmy->SetIsActive(true);
					}
					else
					{
						swarmy->SetIsActive(false);
					}
				}

				break;
			}
			case 1:  //Join Game
			{
				state = ONLINE_JOIN;

				if (!netManager->GetIsInit())
				{
					netManager = &NetManager::GetInstance();
					netManager->StartUp();
				}

				/*if (!netManager->IsOnline())
				{
					netManager->StartUp();
				}*/

				if (!netManager->GetInstance().Initialise(CLIENT, myGame))
				{
					std::cout << "Unable to initialise the Network...";
					state = ONLINE_MENU;
					break;
				}

				joinButtons->SelectButton(0);
				menuButtons->SetEveryIsActive(false);
				break;
			}
			case 2:  //Return to menu
			{
				menuButtons->SetEnterPressed(false);
				myGame->SetCurrentLevel(0); //Set level to main menu
				audioManager->PlayBGM(false);
				ResetLevel();
				break;
			}
			default:
				break;
			}
		}
		else if (state == ONLINE_JOIN)  //--------------------------------------------------- JOINING A SERVER -------------------------------------------
		{
			int buttonID = joinButtons->GetCurrentID(); //ID of the chosen server button
			netManager->ConnectClientToChosenServer(buttonID); //Tell client to join that chosen server
			joinButtons->SetEveryIsActive(false);
			isServerChosen = true;

			//Update Active objects
			for (unsigned int i = 0; i < vecPlayers.size(); i++)
			{
				vecPlayers[i]->SetIsActive(true);
			}
			vecPlayers[myShipID]->SetX(earth->GetX());
			vecPlayers[myShipID]->SetZ(earth->GetZ());

			enemy->SetIsActive(true);
			earth->SetIsActive(true);
			mars->SetIsActive(true);

			for (unsigned int i = 0; i < vecLevelSwarm.size(); i++)
			{
				Swarmling* swarmy = vecLevelSwarm[i];

				swarmy->SetVelocity(glm::vec2(0.0f, 1.0f));

				if (i < 50) //Activate starting player swarm
				{
					swarmy->SetTargetMother(vecPlayers[myShipID]);
					swarmy->SetTargetPlanet(earth);
					swarmy->SetFaction(FAC_BLUE);
					swarmy->SetState(SWA_PLANET);
					swarmy->SetX(earth->GetX() + (float)(rand() % 20) - 10.0f);
					swarmy->SetZ(earth->GetZ() + (float)(rand() % 20) - 10.0f);
					earth->AddToSwarm(swarmy);
					swarmy->SetIsActive(true);
				}
				else if (i >= 50 && i < 200) //Activate starting enemy swarm
				{
					swarmy->SetTargetMother(enemy);
					swarmy->SetTargetPlanet(mars);
					swarmy->SetTarget(mars);
					swarmy->SetFaction(FAC_RED);
					swarmy->SetState(SWA_PLANET);
					swarmy->SetX(mars->GetX() + (float)(rand() % 20) - 10.0f);
					swarmy->SetZ(mars->GetZ() + (float)(rand() % 20) - 10.0f);
					mars->AddToSwarm(swarmy);
					swarmy->SetIsActive(true);
				}
				else
				{
					swarmy->SetIsActive(false);
				}
			}

			//testShip->SetIsNetworked(false);
			state = ONLINE_GAME;
		}

		//if (state == LOBBY_HOST) //---------------------------------------------------- HOSTING A SERVER -------------------------------------------
		//{
		//	testShip->SetIsNetworked(true);
		//	state = LOBBY_WAIT;
		//}
	}
	else if (inputManager->GetKeyState(13) == KEY_UP && menuButtons->GetEnterPressed() == true) //Enter Key
	{
		menuButtons->SetEnterPressed(false);
		joinButtons->SetEnterPressed(false);
	}

	if ((inputManager->GetArrowState(DIR_DOWN) == KEY_UP) &&
		(inputManager->GetArrowState(DIR_UP) == KEY_UP))
	{
		menuButtons->ResetPressTime();
		joinButtons->ResetPressTime();
	}

	if (inputManager->GetKeyState(27) == KEY_DOWN && state != ONLINE_JOIN) //Escape Key
	{
		if (state == ONLINE_GAME)
		{
			Disconnect();
		}
		myGame->SetCurrentLevel(0); //Set level to main menu
		audioManager->PlayBGM(false);
		ResetLevel();
	}
}

void OnlineLevel::NetworkUpdate()
{
	static float tickElapsed = 0.0f;
	float tickRate = 0.06666f;
	EEntityType type = netManager->GetCurrentType();

	myShipID = entityID; //Set the ship to control as the ID of this net entity

	if (tickElapsed >= tickRate) //On each tick, send data of state of all objects this frame
	{
		for (unsigned int i = 0; i < vecPlayers.size(); i++)
		{
			if (i == myShipID)
			{
				vecPlayers[i]->SetIsNetworked(false); //Set ID ship as local control
			}
			else
			{
				vecPlayers[i]->SetIsNetworked(true); //Set non-ID ships as networked
			}
		}

		for (unsigned int i = 0; i < vecPlanets.size(); i++)
		{
			if (netManager->GetCurrentType() == SERVER)
			{
				vecPlanets[i]->SetIsNetworked(false); //Does not sync to clients
				enemy->SetIsNetworked(false);
			}
			else if (netManager->GetCurrentType() == CLIENT)
			{
				vecPlanets[i]->SetIsNetworked(true); //Has to keep sync with the server
				enemy->SetIsNetworked(true);
			}
		}

		if (netManager->GetCurrentType() == SERVER) //If server, send all data of players
		{
			if (vecPlayers[myShipID] != nullptr)
			{
				vecPlayers[myShipID]->Serialize();
				std::string update = vecPlayers[myShipID]->GetGameUpdateData();

				if (vecPlayers[myShipID]->GetIsChanged())
				{
					netManager->SendGameUpdate(update.c_str());
				}
			}

			//Send data of enemy
			if (enemy != nullptr)
			{
				enemy->Serialize();
				std::string update = enemy->GetGameUpdateData();
				if (enemy->GetIsChanged())
				{
					netManager->SendGameUpdate(update.c_str());
				}
			}

			for (unsigned int i = 0; i < vecPlanets.size(); i++) //Send data of planets
			{
				if (vecPlanets[i] != nullptr)
				{
					vecPlanets[i]->Serialize();
					std::string planetUpdate = vecPlanets[i]->GetGameUpdateData();

					if (vecPlanets[i]->GetIsChanged())
					{
						netManager->SendGameUpdate(planetUpdate.c_str());
					}
				}
			}
		}
		else if (netManager->GetCurrentType() == CLIENT) //If client, only send player data
		{
			if (vecPlayers[myShipID] != nullptr)
			{
				vecPlayers[myShipID]->Serialize();
				std::string update = vecPlayers[myShipID]->GetGameUpdateData();

				if (vecPlayers[myShipID]->GetIsChanged())
				{
					netManager->SendGameUpdate(update.c_str());
				}
			}

			for (unsigned int i = 0; i < vecPlanets.size(); i++) //Send data of planets
			{
				if (vecPlanets[i] != nullptr)
				{
					vecPlanets[i]->Serialize();
					std::string update = vecPlanets[i]->GetGameUpdateData();

					if (vecPlanets[i]->GetIsChanged())
					{
						netManager->SendGameUpdate(update.c_str());
					}
				}
			}
		}
		tickElapsed = 0.0f;
	}
	else
	{
		tickElapsed += deltaTime;
	}
}

void OnlineLevel::ProcessNetworkData(const char* _gameUpdateStr)
{
	//Check which object-type (1st Tag)
	std::string firstTag = ParseTag(_gameUpdateStr, 0);
	int ID = atoi(ParseTag(_gameUpdateStr, 1).c_str());

	if (firstTag == "MOT") //If tag is mothership
	{	
		if (netManager->GetCurrentType() == SERVER) //If server, send the data back to all
		{
			netManager->SendGameUpdate(_gameUpdateStr);
		}

		if (ID < (int)vecPlayers.size())
		{
			if (vecPlayers[ID]->GetIsNetworked())
			{
				vecPlayers[ID]->Deserialize(_gameUpdateStr);
			}
		}
		
		if (ID == 9) //Enemy
		{
			enemy->Deserialize(_gameUpdateStr);
		}
	}

	if (firstTag == "PLA") //If tag is for planet
	{
		vecPlanets[ID]->Deserialize(_gameUpdateStr);
	}
}

void OnlineLevel::Disconnect()
{
	if (netManager != nullptr)
	{
		netManager->SendQuitCommand();
		netManager->ShutDown();
		netManager->JoinThreads();
		netManager->DestroyInstance();
		netManager->SetIsInit(false);
		ResetLevel();
	}
	threadIsInit = false;
	isServerChosen = false;
}

void OnlineLevel::ResetLevel()
{
	menuButtons->SetEnterPressed(true);
	joinButtons->SetEnterPressed(true);

	menuButtons->SetEveryIsActive(true);
	joinButtons->SetEveryIsActive(false);

	state = ONLINE_MENU;

	clock->Initialise();

	//Reset Motherships

	//Players
	int counter = 0;
	for (auto it = vecPlayers.begin(); it != vecPlayers.end(); it++)
	{
		Mothership* player = *it;
		player->SetX(earth->GetX());
		player->SetZ(earth->GetZ());
		player->ResetSpeeds();
		player->ClearVectors();
		player->SetIsNetworked(true);
		player->SetIsAlive(true);
		player->SetIsActive(false);
		player->SetTextActive(false);
		counter++;
	}

	//Enemy
	enemy->SetX(mars->GetX());
	enemy->SetZ(mars->GetZ());
	enemy->ResetAI();
	enemy->ResetSpeeds();
	enemy->ClearVectors();
	enemy->SetIsAlive(true);
	enemy->SetIsActive(false);

	//Reset Planets
	for (auto it = vecPlanets.begin(); it != vecPlanets.end(); it++)
	{
		Planet* planet = *it;

		if (planet->GetIsHomePlanet() == false)
		{
			planet->SetFaction(FAC_NONE);
			planet->ClearVectors();
			planet->SetIsDisplayText(false);
			planet->SetIsActive(false);
		}
	}

	earth->SetFaction(FAC_BLUE);

	earth->SetIsDisplayText(false);
	earth->ClearVectors();
	earth->SetIsActive(false);

	mars->SetFaction(FAC_RED);
	mars->SetIsDisplayText(false);
	mars->ClearVectors();
	mars->SetIsActive(false);

	//Reset Swarmlings
	int i = 0;
	for (auto it = vecLevelSwarm.begin(); it != vecLevelSwarm.end(); it++)
	{
		Swarmling* swarmy = *it;

		swarmy->SetVelocity(glm::vec2(0.0f, 1.0f));
		swarmy->SetIsActive(false);

		i++;
	}

	//Reset Particles
	for (auto it = vecParticles.begin(); it != vecParticles.end(); it++)
	{
		Particle* temp = *it;
		temp->SetIsActive(false);
	}
}

void OnlineLevel::InitPlanets()
{
	//Init Blue Home Planet (Earth)
	earth = new Planet("EARTH", "Assets/Earth.png", this);
	earth->SetX(120.0f);
	earth->SetZ(-120.0f);
	earth->SetVecLevelSwarm(&vecLevelSwarm);
	earth->SetUniformScale(10.0f);
	earth->SetAvoidRadius(12.0f);
	earth->SetMaxSwarm(120);
	earth->SetVecExcess(&vecExcess);
	earth->SetIsNetworked(false);
	earth->SetPlanetID(0);
	earth->Initialise();
	earth->SetIsHomePlanet(true);
	earth->SetIsDisplayText(false);
	earth->SetIsActive(false);
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
	mars->SetMaxSwarm(200);
	mars->SetVecExcess(&vecExcess);
	mars->SetIsNetworked(false);
	mars->SetPlanetID(1);
	mars->Initialise();
	mars->SetIsHomePlanet(true);
	mars->SetIsDisplayText(false);
	mars->SetIsActive(false);
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
	planet->SetMaxSwarm(20);
	planet->SetVecExcess(&vecExcess);
	planet->SetIsNetworked(false);
	planet->SetPlanetID(2);
	planet->Initialise();
	planet->SetIsHomePlanet(false);
	planet->SetIsDisplayText(false);
	planet->SetIsActive(false);
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
	planet->SetMaxSwarm(40);
	planet->SetVecExcess(&vecExcess);
	planet->SetIsNetworked(false);
	planet->SetPlanetID(3);
	planet->Initialise();
	planet->SetIsHomePlanet(false);
	planet->SetIsDisplayText(false);
	planet->SetIsActive(false);
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
	planet->SetMaxSwarm(40);
	planet->SetVecExcess(&vecExcess);
	planet->SetIsNetworked(false);
	planet->SetPlanetID(4);
	planet->Initialise();
	planet->SetIsHomePlanet(false);
	planet->SetIsDisplayText(false);
	planet->SetIsActive(false);
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
	planet->SetMaxSwarm(60);
	planet->SetVecExcess(&vecExcess);
	planet->SetIsNetworked(false);
	planet->SetPlanetID(5);
	planet->Initialise();
	planet->SetIsHomePlanet(false);
	planet->SetIsDisplayText(false);
	planet->SetIsActive(false);
	planet->SetFaction(FAC_NONE);
	vecObjects.push_back(planet);
	vecPlanets.push_back(planet);
}
