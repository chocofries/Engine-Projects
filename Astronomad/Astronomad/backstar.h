#pragma once
#include "sprite.h"

class BackStar : public GameObject
{
public:
	BackStar(Level* _level);
	virtual ~BackStar();

	virtual void Initialise();
	virtual void Update(double dTime);
	virtual void Draw();

	void StarScale(float _scale);

private:
	Level * level;
	Sprite * sprite;


};