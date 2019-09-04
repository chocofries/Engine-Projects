#include "backstar.h"
#include "level.h"

BackStar::BackStar(Level * _level)
{
	this->level = _level;
	VBO = level->GetVBO();
	levelCamera = level->GetCamera();
	inputManager = level->GetInputManager();
	shaderLoader = level->GetShaderLoader();
	assetLoader = level->GetTextureLoader();
	inputManager = level->GetInputManager();
	clock = level->GetClock();
	boundW = level->GetBoundW();
	boundH = level->GetBoundH();
}

BackStar::~BackStar()
{
	delete sprite;
}

void BackStar::Initialise()
{
	sprite = new Sprite(this);
	sprite->SetIsBillboard(true);
	sprite->InitSprite("Assets/BackStar1.png", "BackStar", 0.0f, 1, 1, 1, 0, 128, 128);
}

void BackStar::Update(double dTime)
{
	deltaTime = (float)dTime;
	sprite->Update(deltaTime);
}

void BackStar::Draw()
{
	sprite->Draw();
}

void BackStar::StarScale(float _scale)
{
	sprite->SetSpriteScale(_scale);
}
