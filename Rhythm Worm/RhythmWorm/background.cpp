#include "background.h"

Background::Background()
{
	isTitleScreen = true;
	currentBG = BG_TITLE;
}

Background::~Background()
{
	for (int i = 0; i < 4; i++)
	{
		if (sprites[i] != nullptr)
		{
			delete sprites[i];
		}
	}
}

void Background::Initialise()
{
	sprites[BG_TITLE] = new Sprite(this);
	sprites[BG_TITLE]->InitSprite("Assets/Title.png", "Title", 0.0f, 1, 1, 1, 0, 3340, 1900);

	sprites[BG_GAME] = new Sprite(this);
	sprites[BG_GAME]->InitSprite("Assets/DissectionTrayBG.png", "Background", 0.0f, 1, 1, 1, 0, 3400, 2000);

	sprites[BG_LOSE] = new Sprite(this);
	sprites[BG_LOSE]->InitSprite("Assets/GameOverLose.png", "GameOverLose", 0.0f, 1, 1, 1, 0, 3340, 1900);

	sprites[BG_WIN] = new Sprite(this);
	sprites[BG_WIN]->InitSprite("Assets/GameOverWin.png", "GameOverWin", 0.0f, 1, 1, 1, 0, 3340, 1900);

	currentSprite = sprites[currentBG];
}

void Background::Update(double dTime)
{
	x = 0;
	y = 0;
	z = -10;
	xScale = 1920;
	yScale = 1080;
	zScale = 1.0f;
	zRot = 1.0f;
	currentSprite->Update(dTime);
}

void Background::Draw()
{
	currentSprite->Draw();
}

BGType Background::GetBackground() const
{
	return currentBG;
}

void Background::SetBackground(BGType _bgType)
{
	currentBG = _bgType;
	currentSprite = sprites[currentBG];
}