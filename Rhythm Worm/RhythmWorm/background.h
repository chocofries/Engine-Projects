#pragma once
#include "gameobject.h"
#include "sprite.h"

enum BGType
{
	BG_TITLE,
	BG_GAME,
	BG_LOSE,
	BG_WIN
};

class Background : public GameObject
{
public:
	Background();
	virtual ~Background();

	virtual void Initialise();
	virtual void Update(double dTime);
	virtual void Draw();

private:
	bool isTitleScreen;
	BGType currentBG;
	Sprite * sprites[4];
	Sprite * currentSprite;

public:
	BGType GetBackground() const;
	void SetBackground(BGType _bgType);
};