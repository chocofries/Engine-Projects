#pragma once
#include "level.h"

class Particle;
class BackStar;
class Swarmling;
class Planet;
class Mothership;
class ButtonSet;

class AISandboxLevel : public Level
{
public:
	AISandboxLevel();
	virtual ~AISandboxLevel();

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
	Planet* sun;
	std::vector<Planet*> vecPlanets;
	std::vector<GameObject*> vecBoids;
	std::vector<Swarmling*> vecLevelSwarm;	//All swarmlings in the level
	std::vector<Particle*> vecParticles;	//Vector of particles in the level

	ButtonSet* sandboxButtons;
};