#pragma once
#include "level.h"

class Particle;
class BackStar;
class Swarmling;
class Planet;
class Mothership;

class SceneSolarSystem : public Level
{
public:
	SceneSolarSystem();
	virtual ~SceneSolarSystem();

	virtual void Initialise(Game* _myGame, ShaderLoader* _shaderloader, AssetLoader* _assetloader, InputManager* _inputManager);
	virtual void Update();
	virtual void Draw();

	virtual void ProcessKeyInput();
	virtual void ResetLevel();

	void InitPlanets();

private:
	Texter * textA;
	Texter * hintTexter;

	Mothership* player;
	Mothership* enemy;
	Planet* earth;	//Blue Home Planet
	Planet* mars;	//Red Home Planet
	std::vector<Planet*> vecPlanets;
	std::vector<GameObject*> vecBoids;
	std::vector<Swarmling*> vecLevelSwarm;	//All swarmlings in the level
	std::vector<Particle*> vecParticles;	//Vector of particles in the level
};