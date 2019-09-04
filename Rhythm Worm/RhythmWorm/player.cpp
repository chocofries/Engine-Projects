#include "player.h"

Player::Player(Rhythm* _rhythm, AudioManager* _audioManager, ScoreManager* _scoreManager, Level* _level)
{
	rhythm = _rhythm;
	myLevel = _level;
	audioManager = _audioManager;
	scoreManager = _scoreManager;
	gridX = 0;
	gridY = 0;
	nextMove = MOVE_STAY;
	isCringing = false;
	isDancing = false;
	isFever = false;
	feverTimer = 100;
	iFrames = 0;
}

Player::~Player()
{
	for (int i = 0; i < 11; i++)
	{
		if (sprites[i] != nullptr)
		{
			delete sprites[i];
		}
	}
}

void Player::Initialise()
{
	//Init Sprite
	spriteSize = 180.0f;
	yRot = 1.0f;
	z = 90.0f;
	xScale = spriteSize;
	yScale = spriteSize;
	facing = 1.0f;
	float animSpd = (float)rhythm->GetBPMTick();

	//Normal Sprites
	sprites[ANIM_HEADBANG] = new Sprite(this);
	sprites[ANIM_HEADBANG]->InitSprite("Assets/AnneHeadBang.png", "PlayerHeadBang", animSpd/16.0f, 16, 11, 2, 0, 1927, 432);

	sprites[ANIM_SCOOT_SIDE] = new Sprite(this);
	sprites[ANIM_SCOOT_SIDE]->InitSprite("Assets/Anne-FaceDefault-ScootTheBootSide.png", "PlayerScootSide", animSpd/15.0f, 15, 9, 2, 0, 1847, 444);

	sprites[ANIM_SCOOT_UP] = new Sprite(this);
	sprites[ANIM_SCOOT_UP]->InitSprite("Assets/Anne-FaceDefault-ScootTheBootUp.png", "PlayerScootUp", animSpd / 16.0f, 16, 11, 2, 0, 1949, 536);

	sprites[ANIM_SCOOT_DOWN] = new Sprite(this);
	sprites[ANIM_SCOOT_DOWN]->InitSprite("Assets/Anne-FaceDefault-ScootTheBootDown.png", "PlayerScootDown", animSpd / 16.0f, 16, 11, 2, 0, 1938, 458);

	//Fever Sprites
	sprites[ANIM_FEVER_START] = new Sprite(this);
	sprites[ANIM_FEVER_START]->InitSprite("Assets/Anne-FaceHappy-Fever!.png", "PlayerFeverStart", animSpd / 30.0f, 32, 7, 5, 0, 1976, 1622);

	sprites[ANIM_FEVERBANG] = new Sprite(this);
	sprites[ANIM_FEVERBANG]->InitSprite("Assets/Anne-FaceHappy-HeadBang.png", "PlayerFeverBang", animSpd / 16.0f, 16, 11, 2, 0, 1927, 432);

	sprites[ANIM_FEVER_SIDE] = new Sprite(this);
	sprites[ANIM_FEVER_SIDE]->InitSprite("Assets/Anne-FaceHappy-ScootTheBootSide.png", "PlayerFeverSide", animSpd / 15.0f, 15, 9, 2, 0, 1847, 444);

	sprites[ANIM_FEVER_UP] = new Sprite(this);
	sprites[ANIM_FEVER_UP]->InitSprite("Assets/Anne-FaceHappy-ScootTheBootUp.png", "PlayerFeverUp", animSpd / 16.0f, 16, 11, 2, 0, 1949, 536);

	sprites[ANIM_FEVER_DOWN] = new Sprite(this);
	sprites[ANIM_FEVER_DOWN]->InitSprite("Assets/Anne-FaceHappy-ScootTheBootDown.png", "PlayerFeverDown", animSpd / 16.0f, 16, 11, 2, 0, 1938, 458);
	
	//Cringe/Death Sprites
	sprites[ANIM_CRINGE] = new Sprite(this);
	sprites[ANIM_CRINGE]->InitSprite("Assets/AnneCringe.png", "PlayerCringe", animSpd / 12.0f, 24, 7, 4, 0, 1969, 867);

	sprites[ANIM_DEATH] = new Sprite(this);
	sprites[ANIM_DEATH]->InitSprite("Assets/Anne-FaceCringe-Death.png", "PlayerDeath", animSpd / 16.0f, 49, 7, 7, 0, 1955, 1864);

	currentSprite = sprites[ANIM_HEADBANG];

	//Init Sounds

	//Voices from samplefocus.com (public domain)
	audioManager->GetAudioSystem()->createSound("Assets/Japanese Voices/japanese-yay_C#_minor.wav", FMOD_DEFAULT, 0, &soundHappy1);
	audioManager->AddSoundToVector(soundHappy1);
	audioManager->GetAudioSystem()->createSound("Assets/Japanese Voices/japanese-awesome_A_major.wav", FMOD_DEFAULT, 0, &soundHappy2);
	audioManager->AddSoundToVector(soundHappy2);
	audioManager->GetAudioSystem()->createSound("Assets/Japanese Voices/japanese-yay_C_major.wav", FMOD_DEFAULT, 0, &soundHappy3);
	audioManager->AddSoundToVector(soundHappy3);
	audioManager->GetAudioSystem()->createSound("Assets/Japanese Voices/japanese-idiot_C_minor.wav", FMOD_DEFAULT, 0, &soundAngry1);
	audioManager->AddSoundToVector(soundAngry1);
	audioManager->GetAudioSystem()->createSound("Assets/Japanese Voices/japanese-no-way_G#_minor.wav", FMOD_DEFAULT, 0, &soundAngry2);
	audioManager->AddSoundToVector(soundAngry2);
	audioManager->GetAudioSystem()->createSound("Assets/Japanese Voices/japanese-you-re-a-real-idiot_107bpm_C_minor.wav", FMOD_DEFAULT, 0, &soundAngry3);
	audioManager->AddSoundToVector(soundAngry3);

	//Movement Sound
	audioManager->GetAudioSystem()->createSound("Assets/SlimeMove.mp3", FMOD_DEFAULT, 0, &soundMove);
	audioManager->AddSoundToVector(soundMove);

	gridX = 4;
	gridY = 4;

	x = myLevel->GetTile(gridX, gridY)->GetX();
	y = myLevel->GetTile(gridX, gridY)->GetY() + 40.0f;
	nextX = x;
	nextY = y;
}

void Player::Update(double dTime)
{
	deltaTime = dTime;
	ProcessInput();
	static int missedBeat = 0;

	if (rhythm->GetIsOnBeat() && isFever)
	{
		if (feverTimer < 5)
		{
			feverTimer++;
		}
		else
		{
			isFever = 0;
		}
	}

	if (rhythm->GetIsOnBeat())
	{
		if (iFrames > 0)
		{
			iFrames -= 1;
		}
	}

	if (isMoving == false && rhythm->GetIsOnBeat() && isCringing == false && isDancing == false )
	{
		if (!isFever)
		{
			currentSprite = sprites[ANIM_HEADBANG];
		}
		else
		{
			currentSprite = sprites[ANIM_FEVERBANG];
		}
	
		currentSprite->SetCurrentFrame(2);
		currentSprite->SetCurrentFrameX(2);
		currentSprite->SetCurrentFrameY(0);

		if (!moveQueue.empty())
		{
			if (missedBeat < 2)
			{
				missedBeat += 1;
			}
			else
			{
				moveQueue.clear();
				missedBeat = 0;
			}
		}
	}
	
	if (isCringing)
	{
		isFever = false;
		feverTimer = 0;
		if (currentSprite->GetCurrentFrame() >= 12)
		{
			isCringing = false;
		}
	}
	else if (isDancing)
	{
		missedBeat = 0;
		if (currentSprite->GetCurrentFrame() >= 12)
		{
			isDancing = false;
		}
	}
	else if (isFeverBegin)
	{
		if (currentSprite->GetCurrentFrame() >= 24)
		{
			isFeverBegin = false;
		}
	}

	CheckCombos(); //Check the moves in the move queue and see if a combo is triggered

	myLevel->SetIsFever(isFever);
	xScale = spriteSize;
	currentSprite->Update(deltaTime);

	MoveTowards(nextX, nextY);
}

void Player::Draw()
{
	currentSprite->Draw();
}

void Player::ProcessInput()
{
	currentSprite->SetFacing(facing);

	if (rhythm->GetIsInWindow() && isCringing == false)
	{
		if (inputManager->GetArrowState(DIR_LEFT) == KEY_DOWN)
		{
			facing = 1.0f;
			moveQueue.push_back(MOVE_LEFT);
			if (!isFever)
			{
				currentSprite = sprites[ANIM_SCOOT_SIDE];
			}
			else
			{
				currentSprite = sprites[ANIM_FEVER_SIDE];
			}
			currentSprite->SetCurrentFrame(2);
			currentSprite->SetCurrentFrameX(2);
			currentSprite->SetCurrentFrameY(0);
			isDancing = true;
		}
		else if (inputManager->GetArrowState(DIR_RIGHT) == KEY_DOWN)
		{
			facing = -1.0f;
			moveQueue.push_back(MOVE_RIGHT);
			if (!isFever)
			{
				currentSprite = sprites[ANIM_SCOOT_SIDE];
			}
			else
			{
				currentSprite = sprites[ANIM_FEVER_SIDE];
			}
			currentSprite->SetCurrentFrame(2);
			currentSprite->SetCurrentFrameX(2);
			currentSprite->SetCurrentFrameY(0);
			isDancing = true;
		}
		else if (inputManager->GetArrowState(DIR_UP) == KEY_DOWN)
		{
			moveQueue.push_back(MOVE_UP);
			if (!isFever)
			{
				currentSprite = sprites[ANIM_SCOOT_UP];
			}
			else
			{
				currentSprite = sprites[ANIM_FEVER_UP];
			}
			currentSprite->SetCurrentFrame(2);
			currentSprite->SetCurrentFrameX(2);
			currentSprite->SetCurrentFrameY(0);
			isDancing = true;
		}
		else if (inputManager->GetArrowState(DIR_DOWN) == KEY_DOWN)
		{
			moveQueue.push_back(MOVE_DOWN);
			if (!isFever)
			{
				currentSprite = sprites[ANIM_SCOOT_DOWN];
			}
			else
			{
				currentSprite = sprites[ANIM_FEVER_DOWN];
			}
			currentSprite->SetCurrentFrame(2);
			currentSprite->SetCurrentFrameX(2);
			currentSprite->SetCurrentFrameY(0);
			isDancing = true;
		}
	}
	else if (isCringing == false)
	{
		if ((inputManager->GetArrowState(DIR_LEFT) == KEY_DOWN) ||
			(inputManager->GetArrowState(DIR_RIGHT) == KEY_DOWN) ||
			(inputManager->GetArrowState(DIR_UP) == KEY_DOWN) ||
			(inputManager->GetArrowState(DIR_DOWN) == KEY_DOWN))
		{
			StartCringe();
		}
	}
}

void Player::StartFever()
{
	isFeverBegin = true;
	currentSprite = sprites[ANIM_FEVER_START];
	currentSprite->SetCurrentFrame(2);
	currentSprite->SetCurrentFrameX(2);
	currentSprite->SetCurrentFrameY(0);
	isFever = true;
	feverTimer = 0;
	std::cout << "FEVER!!" << std::endl;
	scoreManager->AddScore(BEST_SCORE);
	PlaySoundRandom(0);
}

void Player::StartCringe()
{
	if (isCringing == false)
	{
		moveQueue.clear();
		isCringing = true;
		currentSprite = sprites[ANIM_CRINGE];
		currentSprite->SetCurrentFrame(2);
		currentSprite->SetCurrentFrameX(2);
		currentSprite->SetCurrentFrameY(0);
		scoreManager->AddScore(BAD_SCORE);
		feverTimer = 0;
		PlaySoundRandom(1);
	}
}

void Player::Reset()
{
	isCringing = false;
	isFever = false;
	currentSprite = sprites[ANIM_HEADBANG];
	currentSprite->SetCurrentFrame(2);
	currentSprite->SetCurrentFrameX(2);
	currentSprite->SetCurrentFrameY(0);
	nextMove = MOVE_STAY;
	isMoving = false;
	gridX = 4;
	gridY = 4;
	x = myLevel->GetTile(gridX, gridY)->GetX();
	y = myLevel->GetTile(gridX, gridY)->GetY() + 40.0f;
	nextX = x;
	nextY = y;
}

void Player::AddIFrames(int _inv)
{
	iFrames += _inv;
}

int Player::GetIFrames() const
{
	return iFrames;
}

int Player::GetGridX() const
{
	return gridX;
}

int Player::GetGridY() const
{
	return gridY;
}

void Player::MoveTowards(float _x, float _y)
{
	float buffer = 10.0f;
	float moveSpeed = 600.0f * (float)deltaTime;

	if (isMoving)
	{
		if (x >= _x - buffer && x <= _x + buffer && y >= _y - buffer && y <= _y + buffer)
		{
			nextMove = MOVE_STAY;
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

void Player::GridMove(MoveDir _move)
{
	moveQueue.clear();
	audioManager->GetAudioSystem()->playSound(soundMove, 0, false, 0);

	switch (_move)
	{
	case MOVE_LEFT:
		if (gridX > 0)
		{
			myLevel->GetTile(gridX, gridY)->SetTileState(TILE_FREE);
			gridX -= 1;
			myLevel->GetTile(gridX, gridY)->SetTileState(TILE_PLAYER);
			isMoving = true;
		}
		break;
	case MOVE_RIGHT:
		if (gridX < TILES_MAX_X - 1)
		{
			myLevel->GetTile(gridX, gridY)->SetTileState(TILE_FREE);
			gridX += 1;
			myLevel->GetTile(gridX, gridY)->SetTileState(TILE_PLAYER);
			isMoving = true;
		}
		break;
	case MOVE_UP:
		if (gridY < TILES_MAX_Y - 1)
		{
			myLevel->GetTile(gridX, gridY)->SetTileState(TILE_FREE);
			gridY += 1;
			myLevel->GetTile(gridX, gridY)->SetTileState(TILE_PLAYER);
			isMoving = true;
		}
		break;
	case MOVE_DOWN:
		if (gridY > 0)
		{
			myLevel->GetTile(gridX, gridY)->SetTileState(TILE_FREE);
			gridY -= 1;
			myLevel->GetTile(gridX, gridY)->SetTileState(TILE_PLAYER);
			isMoving = true;
		}

		break;
	}

	nextX = myLevel->GetTile(gridX, gridY)->GetX();
	nextY = myLevel->GetTile(gridX, gridY)->GetY() + 40.0f;
}

void Player::CheckCombos()
{
	if (!moveQueue.empty())
	{
		std::deque<MoveDir>::reverse_iterator  rit;

		int moveOrder = 0;

		bool dirFound[5]; //Whether a direction has been found

		MoveDir moveCheck = MOVE_STAY;

		for (rit = moveQueue.rbegin(); rit != moveQueue.rend(); rit++) //Iterate through the move queue (in reverse) to see if there is a combo
		{
			MoveDir temp = *rit;
			
			//If in fever mode
			if (isFever)
			{
				scoreManager->AddScore(GOOD_SCORE);
				GridMove(temp);
				break;
			}

			//Check for two identical directions in a row (movement)
			if (moveCheck == temp)
			{
				scoreManager->AddScore(GOOD_SCORE);
				GridMove(temp);
				break;
			}
			else
			{
				moveCheck = temp;
			}

			dirFound[temp] = true;

			if (!isFever)
			{
				//If all four directions are in queue, start fever mode
				if ((dirFound[MOVE_LEFT] == true) &&
					(dirFound[MOVE_RIGHT] == true) &&
					(dirFound[MOVE_UP] == true) &&
					(dirFound[MOVE_DOWN] == true))
				{
					StartFever();
					break;
				}
			}

			moveOrder++;
		}
	}
}

void Player::PlaySoundRandom(int _type)
{
	int chosen = rand() % 3;

	if (_type == 0) //Happy Sounds
	{
		switch (chosen)
		{
		case 0:
			audioManager->GetAudioSystem()->playSound(soundHappy1, 0, false, 0);
			break;
		case 1:
			audioManager->GetAudioSystem()->playSound(soundHappy2, 0, false, 0);
			break;
		case 2:
			audioManager->GetAudioSystem()->playSound(soundHappy3, 0, false, 0);
			break;
		}
	}
	else //Angry Sounds
	{
		switch (chosen)
		{
		case 0:
			audioManager->GetAudioSystem()->playSound(soundAngry1, 0, false, 0);
			break;
		case 1:
			audioManager->GetAudioSystem()->playSound(soundAngry2, 0, false, 0);
			break;
		case 2:
			audioManager->GetAudioSystem()->playSound(soundAngry3, 0, false, 0);
			break;
		}
	}
}
