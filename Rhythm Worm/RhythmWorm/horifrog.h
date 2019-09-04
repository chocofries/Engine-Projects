#pragma once
#include "frog.h"

class HoriFrog : public Frog
{
public:
	HoriFrog(Rhythm* _rhythm, Player* _player, Level* _level, ScoreManager* _scoreManager, int _id);
	virtual ~HoriFrog();

	virtual void Initialise();
	virtual void Update(double dTime);
	virtual void Draw();
	virtual void Spawn();
	virtual void StartDeath();

	void AI();

private:
	ScoreManager* scoreManager;
	Sprite * sprites[6];
	Sprite * currentSprite;
	bool isMovingLeft;
};