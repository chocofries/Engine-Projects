#include "frog.h"

Frog::Frog()
{
}

Frog::~Frog()
{
}

void Frog::Initialise()
{
}

void Frog::Update(double dTime)
{
}

void Frog::Draw()
{

}

void Frog::Spawn()
{
}

void Frog::StartDeath()
{
}

void Frog::SetGridX(int _x)
{
	gridX = _x;
}

void Frog::SetGridY(int _y)
{
	gridY = _y;
}

void Frog::SetIsSpawned(bool _isSpawned)
{
	isSpawned = _isSpawned;
}

int Frog::GetGridX() const
{
	return gridX;
}

int Frog::GetGridY() const
{
	return gridY;
}

int Frog::GetID() const
{
	return frogID;
}

bool Frog::GetIsSpawned() const
{
	return isSpawned;
}

void Frog::MoveTowards(float _x, float _y)
{
	float buffer = 10.0f;
	float moveSpeed = rhythm->GetTickMS() * (float)deltaTime;

	if (isMoving)
	{
		if (x >= _x - buffer && x <= _x + buffer && y >= _y - buffer && y <= _y + buffer)
		{
			//nextMove = MOVE_STAY;
			isMoving = false;
		}

		if (x <= _x)
		{
			x += moveSpeed;
		}
		if (x >= _x)
		{
			x += -moveSpeed;
		}

		if (y <= _y)
		{
			y += moveSpeed;
		}
		if (y >= _y)
		{
			y += -moveSpeed;
		}
	}
}