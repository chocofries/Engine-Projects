#include "omnifrog.h"

OmniFrog::OmniFrog(Rhythm * _rhythm, Player * _player, Level * _level, ScoreManager * _scoreManager, int _id)
{
	rhythm = _rhythm;
	player = _player;
	myLevel = _level;
	scoreManager = _scoreManager;
	frogID = _id;
}

OmniFrog::~OmniFrog()
{
	for (int i = 0; i < 6; i++)
	{
		if (sprites[i] != nullptr)
		{
			delete sprites[i];
		}
	}
}

void OmniFrog::Initialise()
{
	//Init Sprite
	spawnX = rand() % TILES_MAX_X;
	spawnY = rand() % TILES_MAX_Y;
	gridX = spawnX;
	gridY = spawnY;
	xScale = 80.0f;
	yScale = 160.0f;
	yRot = 1.0f;
	float facing = 1.0f;
	float animSpd = (float)rhythm->GetBPMTick();

	sprites[ANIM_CROAK] = new Sprite(this);
	sprites[ANIM_CROAK]->InitSprite("Assets/Omni-CroakBang.png", "OmniCroakBang", animSpd / 16.0f, 16, 12, 2, 0, 1910, 272);

	sprites[ANIM_SCOOT_SIDE] = new Sprite(this);
	sprites[ANIM_SCOOT_SIDE]->InitSprite("Assets/Omni-ScootSide.png", "OmniScootSide", animSpd / 16.0f, 16, 9, 2, 0, 2000, 348);

	sprites[ANIM_SCOOT_UP] = new Sprite(this);
	sprites[ANIM_SCOOT_UP]->InitSprite("Assets/Omni-ScootUp.png", "OmniScootUp", animSpd / 16.0f, 16, 10, 2, 0, 2002, 388);

	sprites[ANIM_SCOOT_DOWN] = new Sprite(this);
	sprites[ANIM_SCOOT_DOWN]->InitSprite("Assets/Omni-ScootDown.png", "OmniScootDown", animSpd / 16.0f, 16, 8, 2, 0, 2034, 458);

	sprites[ANIM_JUMP] = new Sprite(this);
	sprites[ANIM_JUMP]->InitSprite("Assets/Omni-Jump.png", "OmniJump", animSpd / 16.0f, 16, 11, 2, 0, 1927, 562);

	sprites[ANIM_DEATH] = new Sprite(this);
	sprites[ANIM_DEATH]->InitSprite("Assets/Omni-Death.png", "OmniDeath", animSpd / 24.0f, 24, 7, 4, 0, 1864, 1022);

	currentSprite = sprites[ANIM_CROAK];

	Tile* startTile = myLevel->GetTile(gridX, gridY);
	x = startTile->GetX();
	y = startTile->GetY() + 10.0f;
	startTile->SetTileState(TILE_FREE);

	//Init
	spawnTimer = 0;
	isChasingPlayer = true;
	scootCount = 0;
	facing = 1.0f;
	isDying = false;
	isSpawned = false;
}

void OmniFrog::Update(double dTime)
{
	deltaTime = dTime;

	if (rhythm->GetIsOnBeat() && isSpawned == false)
	{
		if (spawnTimer < 3)
		{
			spawnTimer++;
		}
		else //Respawn
		{
			gridX = spawnX;
			gridY = spawnY;
			scootCount = 0;
			currentSprite = sprites[ANIM_CROAK];
			currentSprite->SetX(x);
			currentSprite->SetY(y);
			isDying = false;
			isSpawned = true;

			Tile* temp = myLevel->GetTile(gridX, gridY);
			x = temp->GetX();
			y = temp->GetY() + 10.0f;
			temp->SetIsDanger(false);
		}
	}

	if (rhythm->GetIsOnBeat() && isDying == false)
	{
		currentSprite->SetCurrentFrame(0);
		currentSprite->SetCurrentFrameX(0);
		currentSprite->SetCurrentFrameY(0);
	}
	else if (isDying && isSpawned == true)
	{
		if (currentSprite->GetCurrentFrame() >= 15)
		{
			spawnX = rand() % TILES_MAX_X;
			spawnY = rand() % TILES_MAX_Y;
			gridX = spawnX;
			gridY = spawnY;
			Tile* temp = myLevel->GetTile(gridX, gridY);
			x = temp->GetX();
			y = 10000.0f;
			temp->SetIsDanger(false);
			isSpawned = false;
			isActive = false;
		}
	}

	AI();
	currentSprite->SetFacing(facing);
	currentSprite->Update(deltaTime);
}

void OmniFrog::Draw()
{
	currentSprite->Draw();
}

void OmniFrog::Spawn()
{
	spawnTimer = 0;
	Tile* temp = myLevel->GetTile(spawnX, spawnY);
	temp->SetIsDanger(true);
	isActive = true;
	currentSprite->SetCurrentFrame(0);
	currentSprite->SetCurrentFrameX(0);
	currentSprite->SetCurrentFrameY(0);
}

void OmniFrog::StartDeath()
{
	myLevel->GetTile(gridX, gridY)->SetTileState(TILE_FREE);
	myLevel->GetTile(gridX, gridY)->SetIsDanger(false);
	currentSprite = sprites[ANIM_DEATH];
	currentSprite->SetCurrentFrame(0);
	currentSprite->SetCurrentFrameX(0);
	currentSprite->SetCurrentFrameY(0);
	isDying = true;
}

void OmniFrog::AI()
{
	//Check if a fevered player is on the frog
	if (myLevel->GetIsFever() == true && isDying == false && isSpawned == true)
	{
		if (gridX == player->GetGridX() && gridY == player->GetGridY())
		{
			StartDeath();
			player->StartFever();
			return;
		}
	}
	else if (myLevel->GetIsFever() == false && isDying == false)
	{
		if (gridX == player->GetGridX() && gridY == player->GetGridY())
		{
			if (player->GetIFrames() == 0)
			{
				player->StartCringe();
				myLevel->BreakHeart(); // (; _ ;)
				player->AddIFrames(6);
			}
		}
	}

	if (myLevel->GetIsFever() == true)
	{
		isChasingPlayer = false;
	}
	else
	{
		isChasingPlayer = true;
	}

	//Decide which direction to move in
	if (isChasingPlayer)
	{
		if (player->GetGridX() > gridX)
		{
			nextMove = MOVE_RIGHT;
		}
		else if (player->GetGridX() < gridX)
		{
			nextMove = MOVE_LEFT;
		}

		if (player->GetGridY() > gridY)
		{
			nextMove = MOVE_UP;
		}
		else if (player->GetGridY() < gridY)
		{
			nextMove = MOVE_DOWN;
		}

		//If on the player, then chose a random direction
		if (player->GetGridX() == gridX && player->GetGridY() == gridY)
		{
			int rng = rand() % 4;
			switch (rng)
			{
			case 0:
				nextMove = MOVE_RIGHT;
				break;
			case 1:
				nextMove = MOVE_LEFT;
				break;
			case 2:
				nextMove = MOVE_UP;
				break;
			case 3:
				nextMove = MOVE_DOWN;
				break;
			}
			scootCount = 2;
		}
	}
	else
	{
		if (player->GetGridX() > gridX)
		{
			nextMove = MOVE_LEFT;
		}
		else if (player->GetGridX() < gridX)
		{
			nextMove = MOVE_RIGHT;
		}

		if (player->GetGridY() > gridY)
		{
			nextMove = MOVE_DOWN;
		}
		else if (player->GetGridY() < gridY)
		{
			nextMove = MOVE_UP;
		}
	}

	//Moving Omni-directionally
	if (rhythm->GetIsOnBeat() && isDying == false && isSpawned == true)
	{
		scootCount++;
		if (scootCount >= 3)
		{
			switch (nextMove)
			{
			case MOVE_LEFT:
			{
				if (gridX > 0)
				{
					if (myLevel->GetTile(gridX - 1, gridY)->GetTileState() != TILE_ENEMY) //If there isn't another enemy there
					{
						myLevel->GetTile(gridX, gridY)->SetTileState(TILE_FREE);
						myLevel->GetTile(gridX, gridY)->SetIsDanger(false);
						gridX -= 1;
						myLevel->GetTile(gridX, gridY)->SetTileState(TILE_ENEMY);
						facing = 1.0f;
						isMoving = true;
						currentSprite = sprites[ANIM_SCOOT_SIDE];
					}
				}
				break;
			}
			case MOVE_RIGHT:
			{
				if (gridX < TILES_MAX_X - 1)
				{
					if (myLevel->GetTile(gridX + 1, gridY)->GetTileState() != TILE_ENEMY) //If there isn't another enemy there
					{
						myLevel->GetTile(gridX, gridY)->SetTileState(TILE_FREE);
						myLevel->GetTile(gridX, gridY)->SetIsDanger(false);
						gridX += 1;
						myLevel->GetTile(gridX, gridY)->SetTileState(TILE_ENEMY);
						facing = -1.0f;
						isMoving = true;
						currentSprite = sprites[ANIM_SCOOT_SIDE];
					}
				}
				break;
			}
			case MOVE_UP:
			{
				if (gridY < TILES_MAX_Y - 1)
				{
					if (myLevel->GetTile(gridX, gridY + 1)->GetTileState() != TILE_ENEMY) //If there isn't another enemy there
					{
						myLevel->GetTile(gridX, gridY)->SetTileState(TILE_FREE);
						myLevel->GetTile(gridX, gridY)->SetIsDanger(false);
						gridY += 1;
						myLevel->GetTile(gridX, gridY)->SetTileState(TILE_ENEMY);
						isMoving = true;
						currentSprite = sprites[ANIM_SCOOT_UP];
					}
				}
				break;
			}
			case MOVE_DOWN:
			{
				if (gridY > 0)
				{
					if (myLevel->GetTile(gridX, gridY - 1)->GetTileState() != TILE_ENEMY) //If there isn't another enemy there
					{
						myLevel->GetTile(gridX, gridY)->SetTileState(TILE_FREE);
						myLevel->GetTile(gridX, gridY)->SetIsDanger(false);
						gridY -= 1;
						myLevel->GetTile(gridX, gridY)->SetTileState(TILE_ENEMY);
						isMoving = true;
						currentSprite = sprites[ANIM_SCOOT_DOWN];
					}
				}
				break;
			}
			}
			scootCount = 0;
		}

		if (isMoving == false)
		{
			currentSprite = sprites[ANIM_CROAK];
		}
	}

	//Doing the move
	float nextX = myLevel->GetTile(gridX, gridY)->GetX();
	float nextY = myLevel->GetTile(gridX, gridY)->GetY() + 10.0f;

	MoveTowards(nextX, nextY);
}
