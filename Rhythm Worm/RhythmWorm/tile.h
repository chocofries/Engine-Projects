#pragma once
#include "sprite.h"
#include "rhythm.h"
#include "level.h"

enum TileState
{
	TILE_FREE,
	TILE_PLAYER,
	TILE_ENEMY
};

class Tile : public GameObject
{
public:
	Tile(int _x, int _y, TileState _state, Rhythm* _rhythm, bool _tileColor, Level* _level);
	virtual ~Tile();

	virtual void Initialise();
	virtual void Update(double _dTime);
	virtual void Draw();

	TileState GetTileState() const;
	void SetTileState(TileState _state);
	void SetIsDanger(bool _isDanger);
	void Reset();

private:
	int tileX, tileY; //Position index of tile in the grid
	TileState state;
	Level* myLevel;
	Rhythm* rhythm;
	Sprite* spriteNormal;
	Sprite* spriteFever;
	Sprite* spriteDanger;
	Sprite* currentSprite;
	Texter* debugText;
	bool tileColor; //False = white, True = cyan
	bool isDanger; //If an enemy is about to spawn here
	bool isFever;
};