#include "mothership.h"
#include "cubemap.h"
#include "swarmling.h"
#include "particle.h"
#include "netmanager.h"

Mothership::Mothership(Level * _level, AstroFaction _faction)
{
	this->level = _level;
	VBO = level->GetVBO();
	levelCamera = level->GetCamera();
	inputManager = level->GetInputManager();
	shaderLoader = level->GetShaderLoader();
	assetLoader = level->GetTextureLoader();
	inputManager = level->GetInputManager();
	audioManager = level->GetAudioManager();
	skyBoxTexture = level->GetSkyBox()->GetTexture();
	vecTexts = level->GetVecTexts();
	clock = level->GetClock();
	boundW = level->GetBoundW();
	boundH = level->GetBoundH();
	halfBW = boundW * 0.5f;
	halfBH = boundH * 0.5f;
	x = 0.0f;
	z = 0.0f;
	xRot = 0.0f;
	yRot = 1.0f;
	zRot = 0.0f;
	rotationAngle = 0.0f;
	xSpeed = 0.0f;
	zSpeed = 0.0f;
	turnSpeed = 0.0f;
	maxSpeed = 25.0f;
	maxTurnSpeed = 150.0f;
	accel = 55.0f;
	turnAccel = 600.0f;
	direction = 90.0f;
	linearDamp = 20.0f;
	angularDamp = 200.0f;
	velocity = glm::vec2(0.0f,0.0f);
	avoidRadius = 12.0f;
	shootRate = 0.2f;
	shootElapsed = shootRate;
	respawnElapsed = 0.0f;
	respawnRate = 10.0f;
	faction = _faction;
	aiState = MOT_DECIDE;
	myColonies = 0;
	isDemo = false;
	isNetworked = false;
	oldUpdateData = "OLD";
	shipName = "FLEET";
}

Mothership::~Mothership()
{
	delete currentModel;
}

void Mothership::Initialise()
{
	if (faction == FAC_BLUE)
	{
		currentModel = new Model("Assets/MothershipA/MothershipA.obj", this);
	}
	else
	{
		currentModel = new Model("Assets/MothershipB/MothershipB.obj", this);
	}
	SetUniformScale(0.6f);

	//if (isNetworked)
	//{
		textA = new Texter("", "Assets/Fonts/nasalization-rg.ttf", glm::vec3(x, 10.0f, z), shaderLoader, assetLoader, levelCamera, false, true);
		textA->SetScale(1.0f);
		textA->SetColor(glm::vec3(1.0f,1.0f,1.0f));
		vecTexts->push_back(textA);
	//}
}

void Mothership::Update(double dTime)
{
	deltaTime = (float)dTime;

	//Update text
	if (isNetworked && !isAI)
	{ 
		std::string str;
		str = shipName + " | " + to_string(vecSwarm.size());
		textA->SetText(str);
		textA->SetIsActive(true);
		textA->SetPosition(glm::vec3(x, 10.0f, z));
	}
	else
	{
		textA->SetIsActive(false);
	}

	if (shootElapsed < shootRate)
	{
		shootElapsed += deltaTime;
	}

	ProcessDamping();

	if (isAlive)
	{
		if (!isAI && !isNetworked)
		{
			rotationAngle = direction - 90.0f;
			ProcessInput();
		}
		else if (!isAI)
		{
			rotationAngle = direction - 180.0f;
		}
		else
		{
			rotationAngle = direction - 180.0f;
		}

		if (isAI && isNetworked)
		{
			rotationAngle = direction - 180.0f;
		}

	}
	else //If dead
	{
		if (respawnElapsed < respawnRate) //Wait for respawn
		{
			respawnElapsed += deltaTime;
			ResetSpeeds();
			x = homePlanet->GetX();
			z = homePlanet->GetZ();
		}
		else
		{
			isAlive = true;
			respawnElapsed = 0.0f;
		}
	}

	if (!isAI && !isNetworked) //If local input
	{
		xSpeed = Utils::Clamp(xSpeed, -maxSpeed, maxSpeed);
		zSpeed = Utils::Clamp(zSpeed, -maxSpeed, maxSpeed);
		turnSpeed = Utils::Clamp(turnSpeed, -maxTurnSpeed, maxTurnSpeed);
		velocity.x = xSpeed * deltaTime;
		velocity.y = zSpeed * deltaTime;
		direction += turnSpeed * deltaTime;
		x += velocity.x;
		z += velocity.y;
	}
	else if (!isNetworked) //If AI
	{
		x += velocity.x * deltaTime;
		z += velocity.y * deltaTime;

		ProcessAI();

		velocity.x = Utils::Clamp(velocity.x, -maxSpeed, maxSpeed);
		velocity.y = Utils::Clamp(velocity.y, -maxSpeed, maxSpeed);

		if (velocity.x != 0.0f && velocity.y != 0.0f)
		{
			direction = Utils::RadToDeg(atan2(velocity.x, velocity.y));
		}
	}
	else //If networked, process updateActionData as Inputs
	{
		if (isShooting)
		{
			ShootSwarm();
		}

		if (isGathering)
		{
			GatherSwarm();
		}

		SyncFleetSize();
	}

	if (!isNetworked)
	{
		ProcessCollisions();
	}

	currentModel->Update(deltaTime);
}

void Mothership::Draw()
{
	if (isAlive)
	{
		currentModel->Draw();
	}
}

void Mothership::ProcessInput()
{
	//Movement
	if (inputManager->GetArrowState(DIR_UP) == KEY_DOWN)
	{
		float len = accel * deltaTime;
		xSpeed += Utils::LengthDirX(len, -direction);
		zSpeed += Utils::LengthDirY(len, -direction);
	}
	//if (inputManager->GetArrowState(DIR_DOWN) == KEY_DOWN)
	//{
	//	float len = -accel*0.5f * deltaTime;
	//	xSpeed += Utils::LengthDirX(len, -direction);
	//	zSpeed += Utils::LengthDirY(len, -direction);
	//}
	if (inputManager->GetArrowState(DIR_LEFT) == KEY_DOWN)
	{
		turnSpeed += turnAccel * deltaTime;
	}
	if (inputManager->GetArrowState(DIR_RIGHT) == KEY_DOWN)
	{
		turnSpeed -= turnAccel * deltaTime;
	}
	
	//Commanding Swarmlings
	if (!isDemo)
	{
		if (inputManager->GetKeyState(' ') == KEY_DOWN) //Shooting swarmlings from mothership
		{
			isShooting = true;
			ShootSwarm();
		}
		else
		{
			isShooting = false;
		}

		if ((inputManager->GetKeyState('v') == KEY_DOWN) || (inputManager->GetKeyState('V') == KEY_DOWN)) //Taking swarmlings from an owned planet
		{
			isGathering = true;
			GatherSwarm();
		}
		else
		{
			isGathering = false;
		}
	}

	direction = Utils::Wrap(direction, 0.0f, 360.0f);
}

void Mothership::ProcessAI()
{
	//static int myColonies = 0;
	static int enemyColonies = 0;
	static int planetNum = vecPlanets->size();
	static Mothership* targetMother = nullptr;
	static Planet* targetPlanet = homePlanet;
	static Planet* enemyHome = FindHomePlanet(FAC_BLUE);
	static int gatherAmount = 10; //Amount to gather
	static float thinkElapsed = 0.0f;
	static float thinkSpeed = 1.0f;

	if (isAI)
	{
		if (level->GetLevelDifficulty() == 2) //If Insane Mode
		{
			maxSpeed = 30.0f;
			thinkSpeed = 0.0f;
		}
		if (level->GetLevelDifficulty() == 1) //If Hard Mode
		{
			maxSpeed = 30.0f;
			thinkSpeed = 0.025f;
		}
		else //If Easy Mode
		{
			maxSpeed = 25.0f;
			thinkSpeed = 1.0f;
		}
	}
	else
	{
		maxSpeed = 25.0f;
		thinkSpeed = 1.0f;
	}

	//Decision making
	switch (aiState)
	{
	case MOT_DECIDE:
	{
		myColonies = CountColonies(faction);
		enemyColonies = planetNum - myColonies;

		if (isAlive)
		{
			if (thinkElapsed < thinkSpeed)
			{
				thinkElapsed += deltaTime;
				break;
			}
			else
			{
				thinkElapsed = 0.0f;
			}
		}
		else
		{
			thinkElapsed = thinkSpeed;
		}

		//Position
		glm::vec2 pos = glm::vec2(x, z);

		if (vecSwarm.size() < 10) //If not enough swarmlings in fleet
		{
			//Decide to gather more from colonies

			//Try to find nearest owned planet
			targetPlanet = NearestPlanet(faction);
			gatherAmount = 5 + rand()%50;
			aiState = MOT_GATHER; 
		}
		else if (enemyHome->GetSwarmNum() + 5 < (int)vecSwarm.size()) //If player homeplanet is badly defended
		{
			targetPlanet = enemyHome;
			aiState = MOT_INVADE; //Invade player territory
		}
		else if (myColonies < planetNum) //If not enough planets are mine
		{
			//Decide to create more colonies

			//Try to find nearest uninhabited planet
			targetPlanet = NearestPlanet(FAC_NONE);

			if (targetPlanet == nullptr) //If not found
			{
				targetPlanet = NearestPlanet(FAC_BLUE); //Find nearest blue planet
			}

			aiState = MOT_COLONY;
		}
		else //If everything else is satisfied
		{
			targetPlanet = NearestPlanet(FAC_BLUE); //Find nearest blue planet
			gatherAmount = 100;
			aiState = MOT_INVADE; //Invade player territory
		}

		break;
	}
	case MOT_GATHER:
	{
		//Check if the planet is still mine
		if (targetPlanet->GetFaction() != faction)
		{
			aiState = MOT_DECIDE;
		}

		//Position
		glm::vec2 pos = glm::vec2(x, z);
		glm::vec2 p = glm::vec2(targetPlanet->GetX(), targetPlanet->GetZ());
		float dist = glm::distance(pos, p);

		//Go to the target planet
		if (dist > targetPlanet->GetAvoidRadius() + 10.0f)
		{
			glm::vec2 seek = Pursue(targetPlanet, maxSpeed, accel, true, false) * deltaTime;
			velocity += seek;
		}
		else if (targetPlanet != homePlanet) //If in range
		{
			velocity = glm::normalize(velocity);
			if (vecSwarm.size() < (unsigned int)gatherAmount && targetPlanet->GetSwarmNum() > 5)
			{
				isGathering = true;
				GatherSwarm();
			}
			else if (targetPlanet->GetSwarmNum() <= 10) //Find a random owned planet
			{
				isGathering = false;
				targetPlanet = RandomColony();
			}
			else
			{
				isGathering = false;
				aiState = MOT_DECIDE;
			}
		}
		else //If it is the home planet
		{
			if (vecSwarm.size() < (unsigned int)gatherAmount && targetPlanet->GetSwarmNum() > 30)
			{
				isGathering = true;
				GatherSwarm();
			}
			else if (vecSwarm.size() >= (unsigned int)gatherAmount)
			{
				isGathering = false;
				aiState = MOT_DECIDE;
			}
			else
			{
				isGathering = false;
				targetPlanet = RandomColony();
			}
		}

		break;
	}
	case MOT_COLONY:
	{
		//Check if the planet is still uninhabited
		if (targetPlanet->GetFaction() == FAC_BLUE)
		{
			aiState = MOT_INVADE;
		}

		//Position
		glm::vec2 pos = glm::vec2(x, z);
		glm::vec2 p = glm::vec2(targetPlanet->GetX(), targetPlanet->GetZ());
		float dist = glm::distance(pos, p);

		//Go to the target planet
		if (dist > targetPlanet->GetAvoidRadius() + 30.0f)
		{
			glm::vec2 seek = Pursue(targetPlanet, maxSpeed, accel, true, false) * deltaTime;
			velocity += seek;
		}
		else if(targetPlanet->GetSwarmNum() < 5) //If planet is in range, shoot swarmlings at planet
		{
			velocity = glm::normalize(velocity);
			//ResetSpeeds();

			if (vecSwarm.size() > 1)
			{
				isShooting = true;
				ShootSwarm();
			}
			else
			{
				isShooting = false;
				aiState = MOT_GATHER;
			}
		}
		else
		{
			aiState = MOT_DECIDE;
		}

		break;
	}
	case MOT_ATTACK:
	{
		//Position
		glm::vec2 pos = glm::vec2(x, z);
		glm::vec2 p = glm::vec2(targetMother->GetX(), targetMother->GetZ());
		p += targetMother->GetVelocity();
		float dist = glm::distance(pos, p);

		//Go to the target mothership
		if (dist > targetMother->GetAvoidRadius() + 80.0f)
		{
			glm::vec2 seek = Pursue(targetMother, maxSpeed, accel, true, false) * deltaTime;
			velocity += seek;
		}
		else if (targetMother->GetIsAlive()) //If mother is in range, shoot swarmlings
		{
			if (vecSwarm.size() > 5)
			{
				isShooting = true;
				ShootSwarm();
			}
			else
			{
				isShooting = false;
				aiState = MOT_GATHER;
			}
		}
		else
		{
			aiState = MOT_DECIDE;
		}

		break;
	}
	case MOT_INVADE:
	{
		//Position
		glm::vec2 pos = glm::vec2(x, z);
		glm::vec2 p = glm::vec2(targetPlanet->GetX(), targetPlanet->GetZ());
		float dist = glm::distance(pos, p);

		//Go to the target planet
		if (dist > targetPlanet->GetAvoidRadius() + 30.0f)
		{
			glm::vec2 seek = Pursue(targetPlanet, maxSpeed, accel, true, false) * deltaTime;
			velocity += seek;
		}
		else if (targetPlanet->GetSwarmNum() >= 0) //If planet is in range, shoot swarmlings at planet
		{
			velocity = glm::normalize(velocity);
			if (vecSwarm.size() > 1)
			{
				isShooting = true;
				ShootSwarm();
			}
			else
			{
				isShooting = false;
				aiState = MOT_GATHER;
			}
		}
		else
		{
			aiState = MOT_DECIDE;
		}

		break;
	}
	default:
		break;
	}
}

void Mothership::ProcessDamping()
{
	//Damping Movement
	if (xSpeed > 0.1f)
	{
		xSpeed -= linearDamp * deltaTime;
	}
	else if (xSpeed < -0.1f)
	{
		xSpeed += linearDamp * deltaTime;
	}
	else
	{
		xSpeed = 0.0f;
	}

	if (zSpeed > 0.1f)
	{
		zSpeed -= linearDamp * deltaTime;
	}
	else if (zSpeed <  -0.1f)
	{
		zSpeed += linearDamp * deltaTime;
	}
	else
	{
		zSpeed = 0.0f;
	}

	//Damping Turning
	if (turnSpeed > 0.1f)
	{
		turnSpeed -= angularDamp * deltaTime;
	}
	else if (turnSpeed < -0.1f)
	{
		turnSpeed += angularDamp * deltaTime;
	}
	else
	{
		turnSpeed = 0.0f;
	}
}

void Mothership::ProcessCollisions()
{
	glm::vec2 v = glm::vec2(0.0f,0.0f);
	if (!isAI)
	{
		v = glm::vec2(x + velocity.x, z + velocity.y);

		if (v.x < -halfBW)
		{
			x = -halfBW;
		}
		if (v.x > halfBW)
		{
			x = halfBW;
		}
		if (v.y < -halfBH)
		{
			z = -halfBH;
		}
		if (v.y > halfBH)
		{
			z = halfBH;
		}
	}
	else
	{
		v = glm::vec2(x + velocity.x * deltaTime, z + velocity.y * deltaTime);
	}

	//Check planet collisions
	for (auto it = vecPlanets->begin(); it != vecPlanets->end(); it++)
	{
		Planet* temp = *it;
		glm::vec2 p = glm::vec2(temp->GetX(), temp->GetZ());
		float dist = glm::distance(v, p);
		float angle = Utils::RadToDeg(atan2((z - p.y), (x - p.x)));
		float pLX = p.x + Utils::LengthDirX(temp->GetAvoidRadius(), angle);
		float pLZ = p.y + Utils::LengthDirY(temp->GetAvoidRadius(), angle);

		if (dist < temp->GetAvoidRadius())
		{
			x = pLX;
			z = pLZ;
		}
	}
}

void Mothership::ShootSwarm()
{
	float shootRange = 60.0f;

	if (!vecSwarm.empty())
	{
		if (shootElapsed >= shootRate)
		{
			glm::vec2 shoot = glm::vec2(0.0f, 0.0f);
			Swarmling* swarmy = TakeFromSwarm();

			if (!isAI && !isNetworked)
			{
				shoot.x = Utils::LengthDirX(shootRange, -direction);
				shoot.y = Utils::LengthDirY(shootRange, -direction);
				swarmy->SetX(x + velocity.x);
				swarmy->SetZ(z + velocity.y);
			}
			else
			{
				shoot.x = Utils::LengthDirX(shootRange, -direction + 90.0f);
				shoot.y = Utils::LengthDirY(shootRange, -direction + 90.0f);
				swarmy->SetX(x + (velocity.x * deltaTime));
				swarmy->SetZ(z + (velocity.y * deltaTime));
			}

			swarmy->SetState(SWA_SHOOT);

			swarmy->SetVelocity(glm::vec2(shoot.x, shoot.y));
			swarmy->SetShootX(x + shoot.x);
			swarmy->SetShootZ(z + shoot.y);

			shootElapsed = 0.0f;
		}
	}
}

void Mothership::GatherSwarm()
{
	float gatherRange = 15.0f;
	float gatherRate = 0.2f;
	static float gatherElapsed = 0.1f;

	for (auto it = vecPlanets->begin(); it != vecPlanets->end(); it++)
	{
		Planet* planet = *it;
		glm::vec2 pPos = glm::vec2(planet->GetX(), planet->GetZ());
		glm::vec2 mPos = glm::vec2(x, z);
		float dist = glm::distance(mPos, pPos);

		if (planet->GetFaction() == faction && dist < planet->GetAvoidRadius() + gatherRange) //If planet is my faction and is in range
		{
			if (planet->GetSwarmNum() > 1) //If the planet has more than one swarmling
			{
				if (gatherElapsed >= gatherRate)
				{
					Swarmling* taken = planet->TakeFromSwarm();
					AddToSwarm(taken);
					taken->SetTargetMother(this);
					taken->SetState(SWA_MOTHER);
					gatherElapsed = 0.0f;
				}
				else
				{
					gatherElapsed += deltaTime;
				}
			}
		}
	}
}

Swarmling * Mothership::TakeFromSwarm()
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

void Mothership::AddToSwarm(Swarmling * _add)
{
	if (_add != nullptr)
	{
		vecSwarm.push_back(_add);
		vecBoids.push_back(_add);
	}
}

void Mothership::DealDamage()
{
	if (vecSwarm.size() > 0)
	{
		Swarmling* temp = TakeFromSwarm();
		temp->SetState(SWA_DESTROY);
	}
	else //If it is 0, then the player dies
	{
		//Activate an explosion particle
		for (auto it = vecParticles->begin(); it != vecParticles->end(); it++)
		{
			Particle* part = *it;
			if (part != nullptr)
			{
				if (part->GetIsActive() == false)
				{
					part->Activate(glm::vec3(x, y, z), 0.5f);
					break;
				}
			}
		}

		isAlive = false;
	}
}

void Mothership::ResetSpeeds()
{
	xSpeed = 0.0f;
	zSpeed = 0.0f;
	velocity = glm::vec2(0.0f, 0.0f);
}

void Mothership::ResetAI()
{
	aiState = MOT_DECIDE;
}

void Mothership::ClearVectors()
{
	vecBoids.clear();
	vecSwarm.clear();
}

void Mothership::SetHomePlanet(Planet * _planet)
{
	homePlanet = _planet;
}

void Mothership::SetVecPlanets(std::vector<Planet*>* _vecPlanets)
{
	vecPlanets = _vecPlanets;
}

void Mothership::SetVecParticles(std::vector<Particle*>* _vecParticles)
{
	vecParticles = _vecParticles;
}

void Mothership::SetVecExcess(std::vector<Swarmling*>* _vecExcess)
{
	vecExcess = _vecExcess;
}

void Mothership::SetFaction(AstroFaction _faction)
{
	faction = _faction;
}

void Mothership::SetFactionMotherships(Mothership * blue, Mothership * red)
{
	blueMother = blue;
	redMother = red;
}

void Mothership::SetIsAI(bool _isAI)
{
	isAI = _isAI;
}

void Mothership::SetIsAlive(bool _isAlive)
{
	isAlive = _isAlive;
}

void Mothership::SetIsDemo(bool _isDemo)
{
	isDemo = _isDemo;
}

void Mothership::SetIsNetworked(bool _isNetworked)
{
	isNetworked = _isNetworked;
}

void Mothership::SetHintTexter(Texter * _texter)
{
	hintTexter = _texter;
}

bool Mothership::GetIsAI() const
{
	return isAI;
}

bool Mothership::GetIsAlive() const
{
	return isAlive;
}

bool Mothership::GetIsNetworked() const
{
	return isNetworked;
}

bool Mothership::GetIsChanged() const
{
	return isChanged;
}

int Mothership::GetSwarmNum() const
{
	return (vecSwarm.size());
}

AstroFaction Mothership::GetFaction() const
{
	return faction;
}

MothershipState Mothership::GetAIState() const
{
	return aiState;
}

std::vector<Swarmling*>* Mothership::GetVecSwarm()
{
	return &vecSwarm;
}

std::vector<GameObject*>* Mothership::GetVecBoids()
{
	return &vecBoids;
}

float Mothership::GetRespawnElapsed() const
{
	return respawnElapsed;
}

//---------------------------------- Mothership Networking Functions ----------------------------------------------

void Mothership::Serialize()
{
	std::string str = "MOT|";					//Update of mothership tag
	float xx = x;
	float zz = z;

	str += to_string(shipID) + "|";							//ID of ship	
	str += to_string(xx) + "," + to_string(zz) + ",|";		//X & Z position
	str += to_string(direction) + "|";						//Direction angle
	str += to_string(isShooting) + "|";						//If it's shooting
	str += to_string(isGathering) + "|";					//If it's gathering
	str += to_string(vecSwarm.size()) + "|";				//Fleet size
	str += shipName + "!";									//Ship Name

	if (str != oldUpdateData) //Check if the data is different than before
	{
		gameUpdateData = str;
		oldUpdateData = gameUpdateData;
		isChanged = true; //The data has changed
	}
	else
	{
		isChanged = false; //The data is still the same
	}
}

void Mothership::Deserialize(const char * _updateData)
{
	std::string str = _updateData;

	int tagIndex = 0;			//The current tag being parsed
	std::string checkID = "";	//Checking if the ID is correct

	int commaNum = 0;
	std::string xStr, zStr, dirStr, shootStr, gatherStr, fleetStr, nameStr;

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
			//Set x & z from string
			x = (float)atof(xStr.c_str());
			z = (float)atof(zStr.c_str());

			//Set direction from string
			if (!isAI)
			{
				direction = (float)atof(dirStr.c_str()) + 90.0f;
			}
			else
			{
				direction = (float)atof(dirStr.c_str());
			}

			//Set isShooting & isGathering from string
			isShooting = (bool)atoi(shootStr.c_str());
			isGathering = (bool)atoi(gatherStr.c_str());

			//Set netFleetNum as actual fleet size
			netFleetSize = atoi(fleetStr.c_str());

			//Set name as networked name
			shipName = nameStr;

			break;
		}

		if (tagIndex == 1) //Ship ID Tag
		{
			checkID += *it;
		}
		else if (tagIndex == 2) //Position Tag
		{
			//Ensure checkID is correct
			if (checkID != to_string(shipID))
			{
				return; //If it isn't, then return out of function;
			}
			else //If correct ID
			{
				if (*it == ',')
				{
					commaNum++;
					continue;
				}

				if (commaNum == 0) //X value
				{
					xStr += *it;
				}
				else if (commaNum == 1)  //Z value
				{
					zStr += *it;
				}
			}
		}
		else if (tagIndex == 3) //Direction Tag
		{
			dirStr += *it;
		}
		else if (tagIndex == 4) //IsShooting Tag
		{
			shootStr += *it;
		}
		else if (tagIndex == 5) //IsGathering Tag
		{
			gatherStr += *it;
		}
		else if (tagIndex == 6) //Fleet Size Tag
		{
			fleetStr += *it;
		}
		else if (tagIndex == 7) //Ship Name Tag
		{
			nameStr += *it;
		}
	}
}

void Mothership::SyncFleetSize()
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
			temp->SetState(SWA_MOTHER);
			temp->SetTargetMother(this);
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

void Mothership::SetShipID(int _id)
{
	shipID = _id;
}

void Mothership::SetTextActive(bool _tex)
{
	textA->SetIsActive(_tex);
}

void Mothership::SetShipName(std::string _name)
{
	shipName = _name;
}

std::string Mothership::GetGameUpdateData()
{
	return gameUpdateData;
}

//---------------------------------- Mothership AI Functions ----------------------------------------------

int Mothership::CountColonies(AstroFaction faction)
{
	int counter = 0;

	for (auto it = vecPlanets->begin(); it != vecPlanets->end(); it++)
	{
		Planet* temp = *it;
		if (temp->GetFaction() == faction)
		{
			counter++;
		}
	}

	return counter;
}

Planet* Mothership::NearestPlanet(AstroFaction _faction)
{
	Planet* nearest = nullptr;

	glm::vec2 pos = glm::vec2(x, z);
	float bestDist = 100000.0f;

	//Find the nearest planet of a faction
	for (auto it = vecPlanets->begin(); it != vecPlanets->end(); it++)
	{
		Planet* planet = *it;

		if (planet->GetFaction() == _faction)
		{
			glm::vec2 p = glm::vec2(planet->GetX(), planet->GetZ());
			float dist = glm::distance(pos, p);

			if (dist < bestDist)
			{
				bestDist = dist;
				nearest = planet;
			}
		}
	}

	return nearest;
}

Planet * Mothership::RandomColony()
{
	int index = rand() % myColonies;
	int counter = 0;
	Planet* random = homePlanet;

	for (auto it = vecPlanets->begin(); it != vecPlanets->end(); it++)
	{
		Planet* planet = *it;

		if (planet->GetFaction() == faction)
		{
			random = planet;

			if (counter < index)
			{
				counter++;
			}
			else
			{
				break;
			}
		}
	}

	return random;
}

Planet * Mothership::FindHomePlanet(AstroFaction _faction)
{
	Planet* home = homePlanet;
	for (auto it = vecPlanets->begin(); it != vecPlanets->end(); it++)
	{
		Planet* planet = *it;

		if (planet->GetFaction() == _faction && planet->GetIsHomePlanet())
		{
			home = planet;
		}
	}

	return home;
}


