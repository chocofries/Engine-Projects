#pragma once
#include "gameobject.h"
#include "rhythm.h"
#include "level.h"
#include "player.h"
#include "scoremanager.h"

class ScalpelHand : public GameObject
{
public:
	ScalpelHand(Rhythm* _rhythm, Player* _player, Level* _level, ScoreManager* _scoreManager);
	virtual ~ScalpelHand();

	virtual void Initialise();
	virtual void Update(double _dTime);
	virtual void Draw();

	void SetIsAttacking(bool _isAttacking);
	void MoveTowards(float _x, float _y);

private:
	Sprite* scalpelSprite;
	Rhythm* rhythm;
	Player* player;
	Level* level;
	ScoreManager* scoreManager;
	bool isMoving;
	bool isAttacking;
	int attackTimer; //How many beats to wait for before striking
	int targetX, targetY; //The initial grid X and Y of the target
	float restingX, restingY;
	float nextX, nextY;
};