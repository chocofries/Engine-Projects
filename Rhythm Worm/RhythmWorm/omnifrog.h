#pragma once
#include "frog.h"

class OmniFrog : public Frog
{
public:
	OmniFrog(Rhythm* _rhythm, Player* _player, Level* _level, ScoreManager* _scoreManager, int _id);
	virtual ~OmniFrog();

	virtual void Initialise();
	virtual void Update(double dTime);
	virtual void Draw();
	virtual void Spawn();
	virtual void StartDeath();

	void AI();

private:
	ScoreManager * scoreManager;
	Sprite * sprites[6];
	Sprite * currentSprite;
	bool isChasingPlayer;
	MoveDir nextMove;
};