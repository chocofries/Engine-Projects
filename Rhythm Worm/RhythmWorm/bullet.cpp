#include "bullet.h"

Bullet::Bullet()
{
}

Bullet::~Bullet()
{
	delete sprite;
}

void Bullet::Initialise()
{
	//Init Sprite
	yRot = 1.0f;
	xScale = 20.0f;
	yScale = 20.0f;

	sprite = new Sprite(this);
	//sprite->InitSprite("Assets/BallSprite.bmp", "Bullet", 0.0f, 1, 1, 1, 0, 64, 64);
}

void Bullet::Update(double dTime)
{
}

void Bullet::Draw()
{
}
