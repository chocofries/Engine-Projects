#pragma once
#include "model.h"
#include "level.h"
#include "planet.h"

class Particle;
class Swarmling;
class NetManager;

enum MothershipState //Enums for mothership AI
{
	MOT_DECIDE,		//Take time to decide on an action
	MOT_DEFEND,		//Returning to defend a planet
	MOT_GATHER,		//Gather a fleet
	MOT_COLONY,		//Find uninhabited planets for colonies
	MOT_ATTACK,		//Attacking an enemy mothership
	MOT_INVADE,		//Invade an enemy's planet
	MOT_DESTROY		//Getting destroyed
};

class Mothership : public GameObject
{
public:
	Mothership(Level* _level, AstroFaction _faction);
	virtual ~Mothership();

	virtual void Initialise();
	virtual void Update(double dTime);
	virtual void Draw();

	void ProcessInput();
	void ProcessAI();
	void ProcessDamping();
	void ProcessCollisions();
	void ShootSwarm();
	void GatherSwarm();
	Swarmling* TakeFromSwarm();
	void AddToSwarm(Swarmling* _add);

	void DealDamage();
	void ResetSpeeds();
	void ResetAI();
	void ClearVectors();

	void SetHomePlanet(Planet* _planet);
	void SetVecPlanets(std::vector<Planet*>* _vecPlanets);
	void SetVecParticles(std::vector<Particle*>* _vecParticles);
	void SetVecExcess(std::vector<Swarmling*>* _vecExcess);
	void SetFaction(AstroFaction _faction);
	void SetFactionMotherships(Mothership* blue, Mothership* red);
	void SetIsAI(bool _isAI);
	void SetIsAlive(bool _isAlive);
	void SetIsDemo(bool _isDemo);
	void SetIsNetworked(bool _isNetworked);
	void SetHintTexter(Texter* _texter);

	bool GetIsAI() const;
	bool GetIsAlive() const;
	bool GetIsNetworked() const;
	bool GetIsChanged() const;
	int GetSwarmNum() const;
	AstroFaction GetFaction() const;
	MothershipState GetAIState() const;
	std::vector<Swarmling*>* GetVecSwarm();
	std::vector<GameObject*>* GetVecBoids();

	float GetRespawnElapsed() const;

	//Networking functions
	void Serialize();								//Serializes current position/direction into a string
	void Deserialize(const char* _updateData);		//Deserialize network game update data
	void SyncFleetSize();
	void SetShipID(int _id);
	void SetTextActive(bool _tex);
	void SetShipName(std::string _name);
	std::string GetGameUpdateData();

private:
	Level * level;
	Model * currentModel;
	Planet * homePlanet;
	Texter * textA;
	Texter * textB;
	Texter * hintTexter;

	std::vector<Planet*>* vecPlanets;
	std::vector<Swarmling*> vecSwarm;
	std::vector<Swarmling*>* vecExcess; //Vector of excess swarmlings in the level (caused by descrepancy with networking)
	std::vector<GameObject*> vecBoids;
	std::vector<Particle*>* vecParticles;

	Mothership* blueMother;
	Mothership* redMother;

	MothershipState aiState;
	AstroFaction faction;
	bool isAI;
	bool isAlive;
	bool isDemo;
	bool isNetworked;	//If this mothership is taking in network update instead of local input.
	bool isShooting;	//If the mothership is shooting right now (used for networking)
	bool isGathering;	//If the mothership is gathering right now (used for networking)
	bool isChanged;		//If the gameUpdateData has changed from the previous

	std::string gameUpdateData; //Data of the mothership to be sent
	std::string oldUpdateData;
	std::string shipName;

	float halfBW, halfBH;
	float xSpeed;
	float zSpeed;
	float angularDamp;
	float linearDamp;
	float accel;
	float turnSpeed;
	float turnAccel;
	float maxTurnSpeed;
	float shootElapsed;
	float shootRate;
	float respawnElapsed;
	float respawnRate;

	int maxHealth;
	int health;
	int myColonies;
	int shipID;			//Unique ID of the ship, used for networking
	int netFleetSize;	//Size of the fleet from the actual version of this ship

private:
	//------------------ AI Functions -----------------------
	int CountColonies(AstroFaction faction);		//Counts a faction's current colonies
	Planet* NearestPlanet(AstroFaction faction);	//Finds nearest planet of a faction
	Planet* RandomColony();							//Finds random owned colony
	Planet* FindHomePlanet(AstroFaction faction);	//Finds home planet of a faction
};