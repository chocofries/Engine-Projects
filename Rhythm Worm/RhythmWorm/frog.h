#pragma once
#include "sprite.h"
#include "rhythm.h"
#include "level.h"
#include "player.h"

class Frog : public GameObject
{
public:
	Frog();
	virtual ~Frog();

	virtual void Initialise();
	virtual void Update(double dTime);
	virtual void Draw();
	virtual void Spawn();
	virtual void StartDeath();

	void SetGridX(int _x);
	void SetGridY(int _y);
	void SetIsSpawned(bool _isSpawned);

	int GetGridX() const;
	int GetGridY() const;
	int GetID() const;
	bool GetIsSpawned() const;

protected:
	enum MoveDir
	{
		MOVE_STAY,
		MOVE_LEFT,
		MOVE_RIGHT,
		MOVE_UP,
		MOVE_DOWN
	};

	enum AnimState
	{
		ANIM_CROAK,
		ANIM_SCOOT_SIDE,
		ANIM_SCOOT_UP,
		ANIM_SCOOT_DOWN,
		ANIM_JUMP,
		ANIM_DEATH
	};

	void MoveTowards(float _x, float _y);

	Rhythm* rhythm;
	Player* player;
	Level* myLevel;

	float facing;
	int frogID; //The unique id to this frog instance
	int spawnTimer;
	int scootCount;
	int spawnX, spawnY;
	int gridX, gridY;
	bool isMoving;
	bool isDying;
	bool isSpawned;
};