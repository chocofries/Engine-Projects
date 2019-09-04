#include <string>
#include "swarmling.h"
#include "cubemap.h"
#include "planet.h"
#include "mothership.h"
#include "particle.h"

Swarmling::Swarmling(Level * _level)
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
	xRot = 0.0f;
	yRot = 1.0f;
	zRot = 0.0f;
	rotationAngle = 0.0f;
	maxSpeed = 30.0f;
	maxForce = 20.0f;
	velocity = glm::vec2((float(rand()%100)*0.001f) - 0.05f, (float(rand() % 100)*0.001f) - 0.05f);
	direction = 0.0f;
	arrivalDist = 10.0f;
	avoidRadius = 1.0f;
	faction = FAC_BLUE;
	demoState = DEMO_SEEK;
}

Swarmling::~Swarmling()
{
	delete blueModel;
	delete redModel;
}

void Swarmling::Initialise()
{
	blueModel = new Model("Assets/MothershipA/MothershipA.obj", this);
	redModel = new Model("Assets/MothershipB/MothershipB.obj", this);

	if (faction == FAC_BLUE)
	{
		currentModel = blueModel;
	}
	else
	{
		currentModel = redModel;
	}

	SetUniformScale(0.15f);

	//Init Text
	//text = new Texter("Glurg", "Assets/Fonts/nasalization-rg.ttf", glm::vec3(x, 40.0f, z), shaderLoader, textureLoader, gameCamera);
	//text->SetColor(glm::vec3(1.0f, 1.0f, 1.0f));
	//vecTexts->push_back(text);
}

void Swarmling::Update(double dTime)
{
	if (faction == FAC_BLUE)
	{
		currentModel = blueModel;
	}
	else
	{
		currentModel = redModel;
	}

	deltaTime = (float)dTime;
	currentModel->parent = this;
	currentModel->Update(deltaTime);

	x += velocity.x * deltaTime;
	z += velocity.y * deltaTime;

	ProcessAI();

	velocity.x = Utils::Clamp(velocity.x, -maxSpeed, maxSpeed);
	velocity.y = Utils::Clamp(velocity.y, -maxSpeed, maxSpeed);

	direction = Utils::RadToDeg(atan2(velocity.x, velocity.y));

	rotationAngle = direction + 180.0f;

	ProcessCollisions();

	//std::string str = "X: " + to_string(x) + " Z: " + to_string(z);
	//text->SetPosition(glm::vec3(x,40.0f,z));
	//text->SetText(str);
}

void Swarmling::Draw()
{
	if (!isInvisible)
	{
		currentModel->Draw();
	}
}

void Swarmling::ProcessCollisions()
{
	glm::vec2 v = glm::vec2(x + velocity.x* deltaTime, z + velocity.y* deltaTime);

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

void Swarmling::ProcessAI()
{
	//Path tempPath;
	//glm::vec2 followPath = FollowPath(maxSpeed,maxForce,tempPath) * deltaTime;

	glm::vec2 cohesion = Cohesion(maxSpeed, maxForce, 5.0f, vecOther) * deltaTime;
	glm::vec2 align = Alignment(maxSpeed, maxForce, 5.0f, vecOther) * deltaTime;
	glm::vec2 contain = Containment(maxSpeed, maxForce, 20.0f, level->GetBoundW()*0.5f, level->GetBoundH()*0.5f) * deltaTime;

	switch (state)
	{
	case SWA_DEMO:
	{
		ProcessDemoAI();
		break;
	}
	case SWA_MENU:
	{
		glm::vec2 separate = Separation(maxSpeed, maxForce, 5.0f, vecOther) * deltaTime;
		glm::vec2 wander = Wander(maxSpeed, maxForce, 100.0f, 100.0f, 0.2f) * deltaTime;
		velocity += align + wander + separate + cohesion + contain;
		break;
	}
	case SWA_PLANET:
	{
		if (targetPlanet != nullptr && targetPlanet->GetSwarmNum() <= targetPlanet->GetMaxSwarm())
		{
			target = targetPlanet;
			vecOther = targetPlanet->GetVecBoids();
			float orbitDist = targetPlanet->GetAvoidRadius() + 2.0f;
			glm::vec2 separate = Separation(maxSpeed, maxForce, 1.0f, vecOther) * deltaTime;
			glm::vec2 orbit = Orbit(maxSpeed, maxForce, orbitDist, 1.0f, vecOther, target) * deltaTime;
			//glm::vec2 seek = Seek(targetPlanet->GetX(), targetPlanet->GetZ(), maxSpeed, maxForce, true, false) * deltaTime;
			velocity += align + orbit + cohesion + separate;
		}
		else
		{
			state = SWA_MOTHER;
		}
		break;
	}
	case SWA_MOTHER:
	{
		target = targetMother;
		vecOther = targetMother->GetVecBoids();
		glm::vec2 separate = Separation(maxSpeed* 1.5f, maxForce* 1.5f, 2.0f, vecOther) * deltaTime;

		if (!targetMother->GetIsAI())
		{
			glm::vec2 pursue = Pursue(target, maxSpeed* 2.5f, maxForce* 2.5f, true, false) * deltaTime;
			velocity += pursue + separate + cohesion;
		}
		else
		{
			glm::vec2 pursue = Pursue(target, maxSpeed* 1.0f, maxForce* 1.0f, true, false);
			velocity += pursue + separate + cohesion;
		}
		break;
	}
	case SWA_SHOOT:
	{
		float reachBuffer = 5.0f;
		float detectRange = 10.0f; //Range to detect enemies
		glm::vec2 pos = glm::vec2(x, z);
		glm::vec2 shootPos = glm::vec2(shootX, shootZ);
		float dist = glm::distance(pos, shootPos);

		if (dist > reachBuffer) //On the way to the shoot pos
		{
			//Seek Towards the target
			glm::vec2 seek = Seek(shootX, shootZ, maxSpeed*2.0f, maxForce*2.0f, false, false) * deltaTime;
			velocity += seek;
			glm::vec2 v = glm::vec2(x + velocity.x* deltaTime, z + velocity.y* deltaTime);

			//Check if enemy mothership is near
			/*glm::vec2 enemyPos = glm::vec2(0.0f,0.0f);
			Mothership* enemy = nullptr;

			if (faction == FAC_BLUE)
			{
				enemy = redMother;
			}
			else if (faction == FAC_RED)
			{
				enemy = blueMother;
			}*/

			/*enemyPos = glm::vec2(enemy->GetX(), enemy->GetZ());
			float enemyDist = glm::distance(v,enemyPos);

			if (enemyDist < 20.0f)
			{
				targetMother = enemy;
				state = SWA_ATTACK;
				break;
			}*/

			//Check if a planet is near
			for (auto it = vecPlanets->begin(); it != vecPlanets->end(); it++)
			{
				Planet* planet = *it;
				glm::vec2 p = glm::vec2(planet->GetX(), planet->GetZ());
				float pDist = glm::distance(v, p);
				float angle = Utils::RadToDeg(atan2((z - p.y), (x - p.x)));
				float pLX = p.x + Utils::LengthDirX(planet->GetAvoidRadius(), angle);
				float pLZ = p.y + Utils::LengthDirY(planet->GetAvoidRadius(), angle);

				if (pDist < planet->GetAvoidRadius() + 5.0f) //If a planet is near
				{
					if ((planet->GetFaction() == faction || planet->GetFaction() == FAC_NONE))
					{
						//Owned or uninhabited planet
						if (planet->GetSwarmNum() < planet->GetMaxSwarm()) //If there is space to deposit swarmling
						{
							targetPlanet = planet;
							velocity = glm::normalize(velocity);
							state = SWA_PLANET;
							planet->AddToSwarm(this);
							if (planet->GetFaction() == FAC_NONE)
							{
								planet->SetFaction(faction);
							}
						}
						else
						{
							state = SWA_RETURN; //Return to the mothership
						}
					}
					else if (planet->GetFaction() != faction && planet->GetFaction() != FAC_NONE)
					{
						//Enemy planet
						targetPlanet = planet;
						state = SWA_INVADE;
					}
				}
			}
		}
		else //If reached the shoot pos without incident
		{
			state = SWA_RETURN;
		}
		break;
	}
	case SWA_RETURN:
	{
		target = targetMother;
		glm::vec2 pos = glm::vec2(x, z);
		glm::vec2 motherPos = glm::vec2(targetMother->GetX(), targetMother->GetZ());
		float dist = glm::distance(pos, motherPos);
		
		vecOther = targetMother->GetVecBoids();
		glm::vec2 separate = Separation(maxSpeed, maxForce, 1.0f, vecOther) * deltaTime;
		glm::vec2 pursue = Pursue(target, maxSpeed*1.5f, maxForce*1.5f, false, false) * deltaTime;
		velocity += pursue + separate + cohesion;

		if (dist < 10.0f)
		{
			state = SWA_MOTHER;
			targetMother->AddToSwarm(this);
		}
		break;
	}
	case SWA_ATTACK:
	{
		target = targetMother;
		if (targetMother->GetIsAlive() == false)
		{
			if (faction == FAC_BLUE)
			{
				targetMother = blueMother;
			}
			else if (faction == FAC_RED)
			{
				targetMother = redMother;
			}
			state = SWA_RETURN;
		}
		glm::vec2 pos = glm::vec2(x, z);
		pos += targetMother->GetVelocity() * deltaTime;
		glm::vec2 MotherPos = glm::vec2(targetMother->GetX(), targetMother->GetZ());
		float dist = glm::distance(pos, MotherPos);

		glm::vec2 separate = Separation(maxSpeed* 1.5f, maxForce* 1.5f, 1.0f, vecOther) * deltaTime;
		glm::vec2 pursue = Pursue(target, maxSpeed* 2.5f, maxForce* 2.5f, false, false) * deltaTime;
		velocity += pursue + separate + cohesion;

		if (dist <= targetMother->GetAvoidRadius() + 1.0f)
		{
			targetMother->DealDamage();
			state = SWA_DESTROY;
		}

		break;
	}
	case SWA_INVADE:
	{
		target = targetPlanet;
		glm::vec2 pos = glm::vec2(x, z);
		glm::vec2 planetPos = glm::vec2(targetPlanet->GetX(), targetPlanet->GetZ());
		float dist = glm::distance(pos, planetPos);

		glm::vec2 separate = Separation(maxSpeed* 1.5f, maxForce* 1.5f, 1.0f, vecOther) * deltaTime;
		glm::vec2 pursue = Pursue(target, maxSpeed* 2.5f, maxForce* 2.5f, true, false) * deltaTime;
		velocity += pursue + separate + cohesion;

		if (dist <= targetPlanet->GetAvoidRadius() + 1.0f)
		{
			targetPlanet->DealDamage(faction);
			state = SWA_DESTROY;
		}
		break;
	}
	case SWA_DESTROY:
	{
		//Activate an explosion particle
		for (auto it = vecParticles->begin(); it != vecParticles->end(); it++)
		{
			Particle* part = *it;
			if (part != nullptr)
			{
				if (part->GetIsActive() == false)
				{
					part->Activate(glm::vec3(x, y, z), 0.1f);
					break;
				}
			}
		}

		//Deactivate
		isActive = false;
		break;
	}
	default:
		break;
	}
}

void Swarmling::ProcessDemoAI()
{
	switch (demoState)
	{
	case DEMO_SEEK:
	{
		if (faction == FAC_RED)
		{
			glm::vec2 seek = Seek(targetMother->GetX(), targetMother->GetZ(), maxSpeed, maxForce, false, false) * deltaTime;
			velocity += seek;
		}
		else if (faction == FAC_BLUE)
		{
			if ((x < level->GetBoundW()*0.5f && x > level->GetBoundW()*-0.5f)&&
				(z < level->GetBoundH()*0.5f && z > level->GetBoundH()*-0.5f))
			{
				glm::vec2 flee = Seek(targetMother->GetX(), targetMother->GetZ(), maxSpeed, maxForce, false, true) * deltaTime;
				velocity += flee;
			}
			else
			{
				glm::vec2 seek = Seek(targetMother->GetX(), targetMother->GetZ(), maxSpeed, maxForce, false, false) * deltaTime;
				velocity += seek;
			}
		}
		break;
	}
	case DEMO_PURSUE:
	{
		if (faction == FAC_RED)
		{
			glm::vec2 pursue = Pursue(targetMother, maxSpeed, maxForce, false, false) * deltaTime;
			velocity += pursue;
		}
		else if (faction == FAC_BLUE)
		{
			if ((x < level->GetBoundW()*0.5f && x > level->GetBoundW()*-0.5f) &&
				(z < level->GetBoundH()*0.5f && z > level->GetBoundH()*-0.5f))
			{
				glm::vec2 evade = Pursue(targetMother, maxSpeed, maxForce, false, true) * deltaTime;
				velocity += evade;
			}
			else
			{
				glm::vec2 pursue = Pursue(targetMother, maxSpeed, maxForce, false, false) * deltaTime;
				velocity += pursue;
			}
		}
		break;
	}
	case DEMO_ARRIVAL:
	{
		if (faction == FAC_RED)
		{
			glm::vec2 pursue = Pursue(targetMother, maxSpeed, maxForce, true, false) * deltaTime;
			velocity += pursue;
		}
		else if (faction == FAC_BLUE)
		{
			glm::vec2 wander = Wander(maxSpeed, maxForce, 100.0f, 100.0f, 0.2f) * deltaTime;
			glm::vec2 contain = Containment(maxSpeed, maxForce, 5.0f, level->GetBoundW()*0.5f, level->GetBoundH()*0.5f) * deltaTime;
			velocity += wander + contain;
		}
		break;
	}
	case DEMO_WANDER:
	{
		glm::vec2 wander = Wander(maxSpeed, maxForce, 100.0f, 100.0f, 0.2f) * deltaTime;
		glm::vec2 contain = Containment(maxSpeed, maxForce, 5.0f, level->GetBoundW()*0.5f, level->GetBoundH()*0.5f) * deltaTime;
		velocity += wander + contain;
		break;
	}
	case DEMO_FLOCK:
	{
		glm::vec2 wander = Wander(maxSpeed, maxForce, 100.0f, 100.0f, 0.2f) * deltaTime;
		glm::vec2 cohesion = Cohesion(maxSpeed, maxForce, 8.0f, vecOther) * deltaTime;
		glm::vec2 align = Alignment(maxSpeed, maxForce, 10.0f, vecOther) * deltaTime;
		glm::vec2 separate = Separation(maxSpeed, maxForce, 15.0f, vecOther) * deltaTime;
		glm::vec2 contain = Containment(maxSpeed, maxForce, 10.0f, level->GetBoundW()*0.5f, level->GetBoundH()*0.5f) * deltaTime;

		velocity += wander + cohesion + align + separate + contain;
		break;
	}
	case DEMO_FOLLOW:
	{
		glm::vec2 cohesion = Cohesion(maxSpeed, maxForce, 5.0f, vecOther) * deltaTime;
		glm::vec2 align = Alignment(maxSpeed, maxForce, 5.0f, vecOther) * deltaTime;
		glm::vec2 separate = Separation(maxSpeed, maxForce, 8.0f, vecOther) * deltaTime;

		Path pathA;
		pathA.start = glm::vec2( -10.0f, 200.0f);
		pathA.end = glm::vec2( -10.0f, -200.0f);
		pathA.radius = 2.0f;

		Path pathB;
		pathB.start = glm::vec2( 200.0f, -10.0f);
		pathB.end = glm::vec2( -200.0f, -10.0f);
		pathB.radius = 4.0f;
		
		if (faction == FAC_BLUE)
		{
			glm::vec2 followPath = FollowPath(maxSpeed, maxForce, pathA) * deltaTime;
			velocity += followPath + align + separate + cohesion;
		}
		else
		{
			glm::vec2 followPath = FollowPath(maxSpeed, maxForce, pathB) * deltaTime;
			velocity += followPath + align + separate + cohesion;
		}

		x = Utils::Wrap(x, level->GetBoundW()*-0.5f, level->GetBoundW()*0.5f);
		z = Utils::Wrap(z, level->GetBoundH()*-0.5f, level->GetBoundH()*0.5f);

		break;
	}
	default:
		break;
	}
}

void Swarmling::SetVecParticles(std::vector<Particle*>* _vecParticles)
{
	vecParticles = _vecParticles;
}

void Swarmling::SetVecPlanets(std::vector<Planet*>* _vecPlanets)
{
	vecPlanets = _vecPlanets;
}

void Swarmling::SetVecOther(std::vector<GameObject*>* _vecOther)
{
	vecOther = _vecOther;
}

void Swarmling::SetTarget(GameObject * _target)
{
	target = _target;
}

void Swarmling::SetFaction(AstroFaction _faction)
{
	faction = _faction;
}

void Swarmling::SetState(SwarmState _state)
{
	state = _state;
}

void Swarmling::SetDemoState(DemoState _state)
{
	demoState = _state;
}

void Swarmling::SetTargetPlanet(Planet* _target)
{
	targetPlanet = _target;
}

void Swarmling::SetTargetMother(Mothership* _target)
{
	targetMother = _target;
}

void Swarmling::SetFactionMotherships(Mothership * blue, Mothership * red)
{
	blueMother = blue;
	redMother = red;
}

void Swarmling::SetShootX(float _x)
{
	shootX = _x;
}

void Swarmling::SetShootZ(float _z)
{
	shootZ = _z;
}

void Swarmling::SetIsInvisible(bool _isInvisible)
{
	isInvisible = _isInvisible;
}

AstroFaction Swarmling::GetFaction() const
{
	return faction;
}

SwarmState Swarmling::GetState() const
{
	return state;
}
