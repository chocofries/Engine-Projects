#include "planet.h"
#include "cubemap.h"
#include "swarmling.h"
#include "netmanager.h"

Planet::Planet(std::string _name, std::string _texPath, Level * _level)
{
	planetName = _name;
	planetTex = _texPath;
	this->level = _level;
	VBO = level->GetVBO();
	levelCamera = level->GetCamera();
	inputManager = level->GetInputManager();
	shaderLoader = level->GetShaderLoader();
	assetLoader = level->GetTextureLoader();
	audioManager = level->GetAudioManager();
	vecTexts = level->GetVecTexts();
	clock = level->GetClock();
	xRot = 0.0f;
	yRot = 1.0f;
	zRot = 0.0f;
	rotationAngle = 0.0f;
	skyBoxTexture = level->GetSkyBox()->GetTexture();
	avoidRadius = 12.0f;
	isDisplayText = false;
	isHomePlanet = false;
	swarmNum = 0;
	maxSwarm = 20;
	spawnRate = 2.0f;
	spawnElapsed = 0.0f;
	netFaction = faction;
}

Planet::~Planet()
{
	delete meshSphere;
}

void Planet::Initialise()
{
	meshSphere = new MeshSphere(this, skyBoxTexture);
	meshSphere->InitMesh(planetTex.c_str(), planetName.c_str(), 0.0f, 1, 1, 1, 0, 2048, 1024);

	textA = new Texter(planetName, "Assets/Fonts/nasalization-rg.ttf", glm::vec3(x + 8.0f, 12.0f, z), shaderLoader, assetLoader, levelCamera, false, true);
	textA->SetScale(1.5f);
	textA->SetColor(glm::vec3(0.6f,0.6f,0.6f));
	vecTexts->push_back(textA);

	textB = new Texter("subtitle", "Assets/Fonts/nasalization-rg.ttf", glm::vec3(x + 6.0f, 12.0f, z + 3.0f), shaderLoader, assetLoader, levelCamera, false, true);
	textB->SetScale(0.8f);
	textB->SetColor(glm::vec3(0.6f, 0.6f, 0.6f));
	vecTexts->push_back(textB);
}

void Planet::Update(double dTime)
{
	deltaTime = (float)dTime;
	meshSphere->Update(deltaTime);

	static float planetRotate = 0.0f;
	float rotateSpeed = 5.0f;

	if (planetRotate < 360.0f)
	{
		planetRotate += rotateSpeed * (float)deltaTime;
	}
	else
	{
		planetRotate = 0.0f;
	}
	rotationAngle = planetRotate;

	swarmNum = vecSwarm.size();

	if (isNetworked)
	{
		
	}

	if (faction != FAC_NONE) //Spawning new swarmlings
	{
		if ((int)vecSwarm.size() < maxSwarm) //If the maximum hasn't been reached
		{
			if (spawnElapsed >= spawnRate)
			{
				//Find an inactive swarmling
				for (auto it = vecLevelSwarm->begin(); it != vecLevelSwarm->end(); it++)
				{
					Swarmling* temp = *it;

					if (temp->GetIsActive() == false)
					{
						temp->SetIsActive(true);		//Set active
						temp->SetFaction(faction);		//Set as planet's faction
						temp->SetState(SWA_PLANET);		//Set state
						temp->SetTargetPlanet(this);
						temp->SetX(x);
						temp->SetZ(z);
						spawnElapsed = 0.0f;
						AddToSwarm(temp);
						break;
					}
				}
			}
			else
			{
				spawnElapsed += deltaTime;
			}
		}
	}

	UpdateTexts();
}

void Planet::Draw()
{
	meshSphere->Draw();
}

void Planet::DealDamage(AstroFaction _faction)
{
	if (vecSwarm.size() > 0)
	{
		Swarmling* temp = TakeFromSwarm();
		temp->SetState(SWA_DESTROY);
		swarmNum = vecSwarm.size();
	}
	else //If it is 0, then planet is taken over
	{
		if (!isHomePlanet)
		{
			faction = _faction;
		}
		else //If it is a home planet, end the level
		{
			level->ResetLevel();	//Reset the level
			level->ChangeLevel(2);  //Change to results level;
			level->GetCurrentLevel()->ResetLevel();
			if (level->GetNetManager() != nullptr && level->GetNetManager()->IsOnline())
			{
				level->Disconnect();
			}
			level->GetCurrentLevel()->SetCurrentWinner(_faction); //Set damage dealer faction as the winner
			audioManager->PlayBGM(false);
		}
	}
}

void Planet::UpdateTexts()
{
	//Update text
	textA->SetPosition(glm::vec3(x + 8.0f, y + avoidRadius + 1.0f, z));
	textB->SetPosition(glm::vec3(x + 6.0f, y + avoidRadius + 1.0f, z + 3.0f));
	//textA->SetIsActive(isDisplayText);
	//textB->SetIsActive(isDisplayText);

	std::string str;
	str = planetName + " | " + to_string(vecSwarm.size()) + "/" + to_string(maxSwarm);
	textA->SetText(str);

	//Text color based on faction
	glm::vec3 factionColor = glm::vec3(0.6f, 0.6f, 0.6f);
	switch (faction)
	{
	case FAC_NONE:
	{
		factionColor = glm::vec3(0.6f, 0.6f, 0.6f);
		textB->SetText("UNINHABITED");
		break;
	}
	case FAC_BLUE:
	{
		factionColor = glm::vec3(0.4f, 0.9f, 1.0f);
		if (isHomePlanet)
		{
			textB->SetText("BLUE HOMEPLANET");
		}
		else
		{
			textB->SetText("BLUE COLONY");
		}
		break;
	}
	case FAC_RED:
	{
		factionColor = glm::vec3(1.0f, 0.4f, 0.4f);
		if (isHomePlanet)
		{
			textB->SetText("RED HOMEPLANET");
		}
		else
		{
			textB->SetText("RED COLONY");
		}
		break;
	}
	case FAC_GREEN:
	{
		factionColor = glm::vec3(0.4f, 1.0f, 0.4f);
		if (isHomePlanet)
		{
			textB->SetText("GREEN HOMEPLANET");
		}
		else
		{
			textB->SetText("GREEN COLONY");
		}
		break;
	}
	case FAC_PURPLE:
	{
		factionColor = glm::vec3(1.0f, 0.4f, 1.0f);
		if (isHomePlanet)
		{
			textB->SetText("PURPLE HOMEPLANET");
		}
		else
		{
			textB->SetText("PURPLE COLONY");
		}
		break;
	}
	default:
		break;
	}
	textA->SetColor(factionColor);
	textB->SetColor(factionColor);
}

void Planet::ClearVectors()
{
	vecBoids.clear();
	vecSwarm.clear();
}

void Planet::SetIsDisplayText(bool _display)
{
	textA->SetIsActive(_display);
	textB->SetIsActive(_display);
	//isDisplayText = _display;
}

void Planet::SetIsHomePlanet(bool _home)
{
	isHomePlanet = _home;
}

void Planet::SetFaction(AstroFaction _faction)
{
	faction = _faction;
}

void Planet::SetVecLevelSwarm(std::vector<Swarmling*>* _vec)
{
	vecLevelSwarm = _vec;
}

void Planet::SetVecExcess(std::vector<Swarmling*>* _vecExcess)
{
	vecExcess = _vecExcess;
}

void Planet::SetMaxSwarm(int _maxNum)
{
	maxSwarm = _maxNum;
}

Swarmling* Planet::TakeFromSwarm()
{
	Swarmling* temp = nullptr;

	if (!vecSwarm.empty())
	{
		temp = vecSwarm.back();
		vecSwarm.pop_back();
		vecBoids.pop_back();
	}

	return temp;
}

void Planet::AddToSwarm(Swarmling * _add)
{
	if (_add != nullptr)
	{
		vecSwarm.push_back(_add);
		vecBoids.push_back(_add);
	}
}

int Planet::GetSwarmNum() const
{
	return swarmNum;
}

int Planet::GetMaxSwarm() const
{
	return maxSwarm;
}

bool Planet::GetIsHomePlanet() const
{
	return isHomePlanet;
}

AstroFaction Planet::GetFaction() const
{
	return faction;
}

std::vector<GameObject*>* Planet::GetVecBoids()
{
	return &vecBoids;
}

//---------------------------------------------------------------- Planet Networking functions --------------------------------------------------

void Planet::Serialize()
{
	std::string str = "PLA|";						//Update of planet tag

	str += to_string(planetID) + "|";				//ID of planet	
	str += to_string(faction) + "|";				//Faction of planet
	str += to_string(vecSwarm.size()) + "!";		//Swarm size of planet

	if (str != gameUpdateData) //Check if the data is different than before
	{
		gameUpdateData = str;
		isChanged = true; //The data has changed
	}
	else
	{
		isChanged = false; //The data is still the same
	}
}

void Planet::Deserialize(const char * _updateData)
{
	std::string str = _updateData;

	int tagIndex = 0;			//The current tag being parsed
	std::string checkID = "";	//Checking if the ID is correct

	std::string factionStr, fleetStr;

	//Iterate through string
	for (auto it = str.begin(); it != str.end(); it++)
	{
		if (*it == '|') //If reached end of tag
		{
			tagIndex++;
			continue;
		}
		else if (*it == '!') //If reached end of data
		{
			//Setting the variables to updated settings
			netFaction = static_cast<AstroFaction>(atoi(factionStr.c_str()));
			netFleetSize = atoi(fleetStr.c_str());

			SyncFaction();
			SyncSwarmSize();
			break;
		}

		if (tagIndex == 1) //Planet ID Tag
		{
			checkID += *it;
		}
		else if (tagIndex == 2) //Position Tag
		{
			//Ensure checkID is correct
			if (checkID != to_string(planetID))
			{
				return; //If it isn't, then return out of function;
			}
			else //If correct ID, Get the faction
			{
				factionStr += *it;
			}
		}
		else if (tagIndex == 3) //Get the fleet size
		{
			fleetStr += *it;
		}
	}
}

void Planet::SyncSwarmSize()
{
	//While the fleet size is more than the correct size -----------------------------------------
	while ((int)vecSwarm.size() > netFleetSize)
	{
		if (!vecSwarm.empty()) //If there is at least one
		{
			//Take a swarmling from the fleet
			Swarmling* temp = TakeFromSwarm();
			temp->SetVelocity(glm::vec2(0.0f, 0.0f));
			temp->SetState(SWA_MENU);
			temp->SetIsInvisible(true);	//Set swarmling to invisible
			vecExcess->push_back(temp); //Push it to the vector of excess in the level
		}
		else
		{
			break;
		}
	}

	//While the fleet size is less than the correct size -----------------------------------------
	while ((int)vecSwarm.size() < netFleetSize)
	{
		if (!vecExcess->empty()) //If there is an excess
		{
			//Take a swarmling from the excess vector
			Swarmling* temp = vecExcess->back();

			vecExcess->pop_back();
			temp->SetVelocity(glm::vec2(0.0f, 0.0f));
			temp->SetFaction(faction);
			temp->SetState(SWA_PLANET);
			temp->SetTargetPlanet(this);
			temp->SetTarget(this);
			temp->SetX(x);
			temp->SetZ(z);
			temp->SetIsInvisible(false);	//Set swarmling to not invisible
			temp->SetIsActive(true);

			AddToSwarm(temp); //Add to the fleet
		}
		else
		{
			break;
		}
	}
}

void Planet::SyncFaction()
{
	if (faction != netFaction)
	{
		//Excess all in the swarm
		while (!vecSwarm.empty())
		{
			Swarmling* temp = TakeFromSwarm();
			temp->SetVelocity(glm::vec2(0.0f, 0.0f));
			temp->SetState(SWA_MENU);
			temp->SetIsInvisible(true);	//Set swarmling to invisible
			vecExcess->push_back(temp);
		}

		faction = netFaction;
	}
}

std::string Planet::GetGameUpdateData()
{
	return gameUpdateData;
}

bool Planet::GetIsNetworked() const
{
	return isNetworked;
}

bool Planet::GetIsChanged() const
{
	return isChanged;
}

void Planet::SetIsNetworked(bool _isNet)
{
	isNetworked = _isNet;
}

void Planet::SetPlanetID(int _id)
{
	planetID = _id;
}

