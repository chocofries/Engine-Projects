#pragma once
#include <vector>
#include "model.h"

class Planet;
class Mothership;
class Particle;

enum SwarmState
{
	SWA_DEMO,		//Demo state for AI sandbox
	SWA_MENU,		//No specific state, used for Menu and Results
	SWA_PLANET,		//Orbit assigned faction-owned planet that has free space
	SWA_MOTHER,		//Pursue the player mothership
	SWA_SHOOT,		//Shooting from the mothership
	SWA_RETURN,		//Returning to the mothership
	SWA_ATTACK,		//Pursuing enemy mothership to attack
	SWA_INVADE,		//Seeking enemy planet to invade
	SWA_DESTROY		//Getting destroyed
};

enum DemoState
{
	DEMO_SEEK, 		//Demo Seeking/Fleeing
	DEMO_PURSUE,	//Demo Pursue/Evade
	DEMO_ARRIVAL,	//Demo Pursue & Arrival
	DEMO_WANDER,	//Demo Wandering & Containment
	DEMO_FLOCK,		//Demo Flocking & Wandering
	DEMO_FOLLOW,	//Demo Leader Following
	DEMO_ORBIT		//Demo Orbiting
};

class Swarmling : public GameObject
{
public:
	Swarmling(Level* _level);
	virtual ~Swarmling();

	virtual void Initialise();
	virtual void Update(double dTime);
	virtual void Draw();

	void ProcessCollisions();
	void ProcessAI();
	void ProcessDemoAI();

	void SetVecParticles(std::vector<Particle*>* _vecParticles);
	void SetVecPlanets(std::vector<Planet*>* _vecPlanets);
	void SetVecOther(std::vector<GameObject*>* _vecOther);
	void SetTarget(GameObject* _target);
	void SetFaction(AstroFaction _faction);
	void SetState(SwarmState _state);
	void SetDemoState(DemoState _state);
	void SetTargetPlanet(Planet* _target);
	void SetTargetMother(Mothership* _target);
	void SetFactionMotherships(Mothership* blue, Mothership* red);
	void SetShootX(float _x);
	void SetShootZ(float _z);

	void SetIsInvisible(bool _isInvisible);

	AstroFaction GetFaction() const;
	SwarmState GetState() const;

private:

	Level * level;
	Model * currentModel;
	Model * blueModel; //Blue faction model
	Model * redModel; //Red faction model
	Texter* text;
	AstroFaction faction;
	SwarmState state;
	Planet* targetPlanet;
	Mothership* targetMother;
	Mothership* blueMother;
	Mothership* redMother;

	std::vector<Planet*>* vecPlanets;
	std::vector<GameObject*>* vecOther; //Vector used for separation;
	std::vector<Particle*>* vecParticles;
	GameObject* target;
	float targetX, targetZ;
	float shootX, shootZ;
	float maxSpeed;
	float maxForce;

	bool isInvisible;

	DemoState demoState; //State of the AI demo
};