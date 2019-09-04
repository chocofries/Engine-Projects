#include "heart.h"

Heart::Heart(Rhythm* _rhythm)
{
	rhythm = _rhythm;
}

Heart::~Heart()
{
	delete sprite;
}

void Heart::Initialise()
{
	float animSpd = (float)rhythm->GetBPMTick();
	sprite = new Sprite(this);
	sprite->InitSprite("Assets/Heart-Beat.png", "HeartBeat", animSpd/16.0f, 16, 16, 1, 0, 1842, 162);
}

void Heart::Update(double _dTime)
{
	z = 80.0f;
	zRot = 1.0f;
	if (rhythm->GetIsOnBeat())
	{
		sprite->SetCurrentFrame(0);
		sprite->SetCurrentFrameX(0);
		sprite->SetCurrentFrameY(0);
	}
	sprite->Update(_dTime);
}

void Heart::Draw()
{
	sprite->Draw();
}
