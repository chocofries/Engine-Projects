#pragma once
#include "sprite.h"
#include "rhythm.h"

class Heart : public GameObject
{
public:
	Heart(Rhythm* _rhythm);
	virtual ~Heart();

	virtual void Initialise();
	virtual void Update(double _dTime);
	virtual void Draw();

private:
	Sprite* sprite;
	Rhythm* rhythm;
};