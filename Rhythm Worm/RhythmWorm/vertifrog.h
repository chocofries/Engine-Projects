#pragma once
#include "frog.h"

class VertiFrog : public Frog
{
public:
	VertiFrog(Rhythm* _rhythm, Player* _player, Level* _level, ScoreManager* _scoreManager, int _id);
	virtual ~VertiFrog();

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
	bool isMovingDown;
};