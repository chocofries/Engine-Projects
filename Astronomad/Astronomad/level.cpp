#include "level.h"
#include "planet.h"
#include "sprite.h"
#include "cubemap.h"
#include "model.h"
#include "mothership.h"
#include "Swarmling.h"
#include "netmanager.h"

Level::Level()
{
	//isInit = false;
}

Level::~Level()
{
	//while (vecObjects.size() > 0)
	//{
	//	GameObject* temp = vecObjects[vecObjects.size() - 1];
	//	vecObjects.pop_back();
	//	delete temp;
	//}

	//while (vecTexts.size() > 0)
	//{
	//	Texter* temp = vecTexts[vecTexts.size() - 1];
	//	vecTexts.pop_back();
	//	delete temp;
	//}
	//delete clock;
	//delete levelCamera;
}

void Level::Initialise(Game* _myGame, ShaderLoader* _shaderloader, AssetLoader* _textureloader, InputManager* _inputManager)
{
	//shaderLoader = _shaderloader;
	//assetLoader = _textureloader;
	//inputManager = _inputManager;
	//levelCamera = new Camera();
	//levelCamera->SetCamPos(glm::vec3(0.0f, 40.0f, 0.0f));

	//glEnable(GL_TEXTURE_2D);
	//glGenBuffers(1, &levelVBO);
	//glBindBuffer(GL_ARRAY_BUFFER, levelVBO);

	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	////Init Clock
	//clock = new Clock();
	//clock->Initialise();

	////Init Text
	//Texter* title = new Texter("ASTRONOMAD", "Assets/Fonts/nasalization-rg.ttf", glm::vec3(-500.0f, -600.0f, 2.0f), shaderLoader, assetLoader, levelCamera);
	//title->SetColor(glm::vec3(1.0f, 1.0f, 1.0f));
	//vecTexts.push_back(title);

	////Init Sky Box
	//std::vector<std::string> vecCubeMapPaths;
	//vecCubeMapPaths.push_back("bkg3_right1.png");
	//vecCubeMapPaths.push_back("bkg3_left2.png");
	//vecCubeMapPaths.push_back("bkg3_top3.png");
	//vecCubeMapPaths.push_back("bkg3_bottom4.png");
	//vecCubeMapPaths.push_back("bkg3_front5.png");
	//vecCubeMapPaths.push_back("bkg3_back6.png");
	//skyBox = new CubeMap(this, &vecCubeMapPaths, "Assets/CubeMap/");
	//skyBox->Initialise();
	//vecObjects.push_back(skyBox);

	////Init Planet
	//Planet* testGlobe = new Planet("Earth", "Assets/Earth.png",this);
	//testGlobe->Initialise();
	//testGlobe->SetX(10.0f);
	//testGlobe->SetZ(10.0f);
	//vecObjects.push_back(testGlobe);

	////Init Mothership
	//Mothership* player = new Mothership(this, FAC_BLUE);
	//player->Initialise();
	//vecObjects.push_back(player);

	////Set camera to follow
	//levelCamera->SetFollowTarget(player);
	//levelCamera->SetIsFollowing(true);

	////Init Swarmling
	//Swarmling* swarmy = new Swarmling(this);
	//swarmy->SetTarget(player);
	//swarmy->Initialise();
	//swarmy->SetX(0.0f);
	//swarmy->SetZ(0.0f);
	//vecObjects.push_back(swarmy);

	//isInit = true;
}

void Level::Update()
{
	//Update time
	clock->Process();
	GLfloat currentTime = (GLfloat)glutGet(GLUT_ELAPSED_TIME);

	float orbitSpeed = 1.0f;
	float orbitDistance = 10.0f;
	currentTime = (currentTime / 1000.0f)*orbitSpeed;
	double deltaTime = clock->GetDeltaTick();

	//Camera
	levelCamera->SetCamUpDir(glm::vec3(0.0f, 1.0f, 0.0f));
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

	glutPostRedisplay();
}

void Level::Draw()
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

	//Draw texts
	for (unsigned int i = 0; i < vecTexts.size(); i++)
	{
		if (vecTexts[i]->GetIsActive())
		{
			vecTexts[i]->Render();
		}
	}

	glutSwapBuffers();
}

void Level::ProcessKeyInput()
{
	
}

void Level::ResetLevel()
{
}

void Level::ChangeLevel(int _index)
{
	myGame->SetCurrentLevel(_index);
}

std::vector<Texter*>* Level::GetVecTexts()
{
	return &vecTexts;
}

GLuint Level::GetVBO() const
{
	return levelVBO;
}

ShaderLoader * Level::GetShaderLoader() const
{
	return shaderLoader;
}

AssetLoader * Level::GetTextureLoader() const
{
	return assetLoader;
}

InputManager * Level::GetInputManager() const
{
	return inputManager;
}

AudioManager * Level::GetAudioManager() const
{
	return audioManager;
}

NetManager * Level::GetNetManager() const
{
	return netManager;
}

Camera * Level::GetCamera() const
{
	return levelCamera;
}

Clock * Level::GetClock() const
{
	return clock;
}

CubeMap * Level::GetSkyBox() const
{
	return skyBox;
}

bool Level::GetIsInit() const
{
	return isInit;
}

float Level::GetBoundW() const
{
	return boundW;
}

float Level::GetBoundH() const
{
	return boundH;
}

int Level::GetLevelDifficulty() const
{
	return levelDifficulty;
}

void Level::SetAudioManager(AudioManager * _audio)
{
	audioManager = _audio;
}

void Level::SetNetManager(NetManager * _netmanager)
{
	netManager = _netmanager;
}

void Level::SetLevelDifficulty(int _diff)
{
	levelDifficulty = _diff;
}

void Level::SetCurrentWinner(AstroFaction _winner)
{
	currentWinner = _winner;
}

void Level::SetScreenMouseX(int _x)
{
	screenMouseX = _x;
}


void Level::SetScreenMouseY(int _y)
{
	screenMouseY = _y;
}

std::string Level::ParseTag(const char * _updateData, int _tagNum)
{
	std::string str = _updateData;
	str.erase(std::remove(str.begin(), str.end(), ' '), str.end()); //Moves data (spaces) to the back, then erases from there to the end of the line
	std::string tag;
	int tagCount = 0;

	//Iterate through string
	for (auto it = str.begin(); it != str.end(); it++)
	{
		if (*it == '|')
		{
			tagCount++;
			continue;
		}

		if (tagCount > _tagNum)
		{
			break;
		}

		if (tagCount == _tagNum)
		{
			tag += *it;
		}
	}

	return tag;
}

int Level::GetEntityID() const
{
	return entityID;
}

void Level::SetEntityID(int _id)
{
	entityID = _id;
}

std::string Level::GetUsername()
{
	return myGame->GetUsername();
}

std::vector<std::string>* Level::GetVecClientName()
{
	return (netManager->GetVecClientName());
}

void Level::NetworkUpdate()
{
}

void Level::ProcessNetworkData(const char* _gameUpdateStr)
{
}

void Level::Disconnect()
{

}

Level * Level::GetCurrentLevel() const
{
	return myGame->GetCurrentLevel();
}
