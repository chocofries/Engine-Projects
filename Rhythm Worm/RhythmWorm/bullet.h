#pragma once
#include "sprite.h"

class Bullet : public GameObject
{
public:
	Bullet();
	virtual ~Bullet();

	virtual void Initialise();
	virtual void Update(double dTime);
	virtual void Draw();

private:
	Sprite * sprite;
};