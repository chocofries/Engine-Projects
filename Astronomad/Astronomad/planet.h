#pragma once
#include "level.h"
#include "meshsphere.h"
#include "sprite.h"

class Swarmling;

class Planet : public GameObject
{
public:
	Planet(std::string _name, std::string _texPath, Level* _level);
	virtual ~Planet();

	virtual void Initialise();
	virtual void Update(double dTime);
	virtual void Draw();

	void DealDamage(AstroFaction _faction);
	void UpdateTexts();

	void ClearVectors();

	void SetIsDisplayText(bool _display);
	void SetIsHomePlanet(bool _home);
	void SetFaction(AstroFaction _faction);
	void SetVecLevelSwarm(std::vector<Swarmling*>* _vec);
	void SetVecExcess(std::vector<Swarmling*>* _vecExcess);
	void SetMaxSwarm(int _maxNum);

	Swarmling* TakeFromSwarm();
	void AddToSwarm(Swarmling* _add);

	int GetSwarmNum() const;
	int GetMaxSwarm() const;
	bool GetIsHomePlanet() const;
	AstroFaction GetFaction() const;
	std::vector<GameObject*>* GetVecBoids();

	//Networking functions
	void Serialize();
	void Deserialize(const char * _updateData);
	void SyncSwarmSize();
	void SyncFaction();	//Syncs the faction as the netFaction

	std::string GetGameUpdateData();
	bool GetIsNetworked() const;
	bool GetIsChanged() const;
	void SetIsNetworked(bool _isNet);
	void SetPlanetID(int _id);

private:
	Level * level;
	MeshSphere* meshSphere;
	Texter* textA;
	Texter* textB;

	float spawnRate;
	float spawnElapsed;
	AstroFaction faction;	//The faction of this planet
	int swarmNum;			//Current number of swarmlings on the planet
	int maxSwarm;			//Maximum number of swarmlings on the planet
	std::string planetName; //Name of the planet
	std::string planetTex;	//Texture path of the planet
	bool isDisplayText;
	bool isHomePlanet;
	std::vector<Swarmling*>* vecLevelSwarm;	//Vector of all swarmlings in the level
	std::vector<Swarmling*> vecSwarm;		//Vector of swarmlings on the planet
	std::vector<GameObject*> vecBoids;		//Vector of boids on the planet
	std::vector<Swarmling*>* vecExcess;		//Vector of excess swarmlings in the level

	//Networking variables
	std::string gameUpdateData; //Data of the planet to be sent
	int planetID;
	int netFleetSize;
	AstroFaction netFaction;
	bool isNetworked;
	bool isChanged;

};