#pragma once
#include "sprite.h"
#include "rhythm.h"
#include "tile.h"
#include "level.h"

class Player : public GameObject
{
public:
	Player(Rhythm* _rhythm, AudioManager* _audioManager, ScoreManager* _scoreManager, Level* _level);
	virtual ~Player();

	virtual void Initialise();
	virtual void Update(double dTime);
	virtual void Draw();

	void ProcessInput();
	void StartFever();
	void StartCringe();
	void Reset();
	void AddIFrames(int _inv);

	int GetIFrames() const;
	int GetGridX() const;
	int GetGridY() const;

private:
	enum MoveDir
	{
		MOVE_STAY,
		MOVE_LEFT,
		MOVE_RIGHT,
		MOVE_UP,
		MOVE_DOWN
	};

	enum AnimState
	{
		ANIM_HEADBANG,
		ANIM_FEVERBANG,
		ANIM_SCOOT_SIDE,
		ANIM_SCOOT_UP,
		ANIM_SCOOT_DOWN,
		ANIM_FEVER_SIDE,
		ANIM_FEVER_UP,
		ANIM_FEVER_DOWN,
		ANIM_FEVER_START,
		ANIM_CRINGE,
		ANIM_DEATH
	};

	bool isMoving;
	bool isCringing;
	bool isDancing;
	bool isFever;
	bool isFeverBegin;
	int iFrames; //number of current invincibility frames (goes down by one each beat until 0)

	void MoveTowards(float _x, float _y);
	void GridMove(MoveDir _move);
	void CheckCombos();
	void PlaySoundRandom(int _type); //type 0 = happy, type 1 = sad

	std::deque<MoveDir> moveQueue; //A double-ended queue of moves, determines dance move combos
	MoveDir nextMove;
	Level* myLevel;
	Tile* myTile;
	int gridX;
	int gridY;
	int feverTimer; //How many turns since getting fever
	float nextX, nextY;

	Rhythm* rhythm;
	AudioManager* audioManager;
	ScoreManager* scoreManager;
	Sprite* sprites[11];
	Sprite* currentSprite;

	FMOD::Sound* soundHappy1;
	FMOD::Sound* soundHappy2;
	FMOD::Sound* soundHappy3;
	FMOD::Sound* soundAngry1;
	FMOD::Sound* soundAngry2;
	FMOD::Sound* soundAngry3;

	FMOD::Sound* soundMove;

	float spriteSize;
	float facing; //Which X direction the player sprite is facing, 1 = right, -1 = left

	float xSpeed;
	float ySpeed;
	float maxSpeed;
	float friction;
};