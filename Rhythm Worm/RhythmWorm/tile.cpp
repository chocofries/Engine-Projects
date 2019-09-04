#include "tile.h"

Tile::Tile(int _x, int _y, TileState _state, Rhythm* _rhythm, bool _tileColor, Level* _level)
{
	tileX = _x;
	tileY = _y;
	state = _state;
	rhythm = _rhythm;
	tileColor = _tileColor;
	myLevel = _level;
	zRot = 1.0f;
	isDanger = false;
}

Tile::~Tile()
{
	delete spriteNormal;
	delete spriteFever;
	delete spriteDanger;
}

void Tile::Initialise()
{
	spriteNormal = new Sprite(this);
	spriteNormal->InitSprite("Assets/Tile.png", "TileNormal", 0.0f, 2, 2, 1, 1, 512, 256);
	spriteNormal->SetWidth((float)TILE_WIDTH*2);
	spriteNormal->SetHeight((float)TILE_HEIGHT*2);

	spriteFever = new Sprite(this);
	spriteFever->InitSprite("Assets/TileFever.png", "TileFever", 0.0f, 2, 2, 1, 1, 512, 256);
	spriteFever->SetWidth((float)TILE_WIDTH * 2);
	spriteFever->SetHeight((float)TILE_HEIGHT * 2);

	spriteDanger = new Sprite(this);
	spriteDanger->InitSprite("Assets/TileDanger.png", "TileDanger", 0.0f, 2, 2, 1, 1, 512, 256);
	spriteDanger->SetWidth((float)TILE_WIDTH * 2);
	spriteDanger->SetHeight((float)TILE_HEIGHT * 2);

	currentSprite = spriteNormal;
	//debugText = new Texter("","Assets/Fonts/arial.ttf", glm::vec3(x, y, 20.0f), shaderLoader, textureLoader, gameCamera, false);
	//debugText->SetScale(80.0f);
	//vecTexts->push_back(debugText);
}

void Tile::Update(double _dTime)
{
	currentSprite->Update(_dTime);

	if (rhythm->GetIsOnBeat())
	{
		if (isDanger)
		{
			currentSprite = spriteDanger;
		}
		else if (myLevel->GetIsFever())
		{
			currentSprite = spriteFever;
		}
		else
		{
			currentSprite = spriteNormal;
		}

		if (tileColor)
		{
			tileColor = false;
			currentSprite->SetCurrentFrameX(0);
			currentSprite->SetCurrentFrameY(0);
		}
		else
		{
			tileColor = true;
			currentSprite->SetCurrentFrameX(1);
			currentSprite->SetCurrentFrameY(0);
		}
	}
}

void Tile::Draw()
{
	currentSprite->Draw();
}

TileState Tile::GetTileState() const
{
	return state;
}

void Tile::SetTileState(TileState _state)
{
	state = _state;
}

void Tile::SetIsDanger(bool _isDanger)
{
	isDanger = _isDanger;
}

void Tile::Reset()
{
	state = TILE_FREE;
	isDanger = false;
	currentSprite = spriteNormal;
}
