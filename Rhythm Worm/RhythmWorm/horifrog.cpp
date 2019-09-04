#include "horifrog.h"
#include "tile.h"

HoriFrog::HoriFrog(Rhythm * _rhythm, Player * _player, Level* _level, ScoreManager* _scoreManager, int _id)
{
	rhythm = _rhythm;
	player = _player;
	myLevel = _level;
	scoreManager = _scoreManager;
	frogID = _id;
}

HoriFrog::~HoriFrog()
{
	for (int i = 0; i < 6; i++)
	{
		if (sprites[i] != nullptr)
		{
			delete sprites[i];
		}
	}
}

void HoriFrog::Initialise()
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
	sprites[ANIM_CROAK]->InitSprite("Assets/Hori-CroakBang.png", "HoriCroakBang", animSpd / 16.0f, 16, 12, 2, 0, 1910, 272);

	sprites[ANIM_SCOOT_SIDE] = new Sprite(this);
	sprites[ANIM_SCOOT_SIDE]->InitSprite("Assets/Hori-ScootSide.png", "HoriScootSide", animSpd / 16.0f, 16, 9, 2, 0, 2000, 348);

	sprites[ANIM_JUMP] = new Sprite(this);
	sprites[ANIM_JUMP]->InitSprite("Assets/Hori-Jump.png", "HoriJump", animSpd / 16.0f, 16, 11, 2, 0, 1927, 562);

	sprites[ANIM_DEATH] = new Sprite(this);
	sprites[ANIM_DEATH]->InitSprite("Assets/Hori-Death.png", "HoriDeath", animSpd / 24.0f, 24, 7, 4, 0, 1864, 1022);


	currentSprite = sprites[ANIM_CROAK];

	Tile* startTile = myLevel->GetTile(gridX, gridY);
	x = startTile->GetX();
	y = startTile->GetY() + 10.0f;
	startTile->SetTileState(TILE_FREE);

	//Init
	spawnTimer = 0;
	isMovingLeft = false;
	scootCount = 0;
	facing = 1.0f;
	isDying = false;
	isSpawned = false;
}

void HoriFrog::Update(double dTime)
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

void HoriFrog::Draw()
{
	currentSprite->Draw();
}

void HoriFrog::Spawn()
{
	spawnTimer = 0;
	Tile* temp = myLevel->GetTile(spawnX, spawnY);
	temp->SetIsDanger(true);
	isActive = true;
	currentSprite->SetCurrentFrame(0);
	currentSprite->SetCurrentFrameX(0);
	currentSprite->SetCurrentFrameY(0);
}

void HoriFrog::StartDeath()
{
	myLevel->GetTile(gridX, gridY)->SetTileState(TILE_FREE);
	myLevel->GetTile(gridX, gridY)->SetIsDanger(false);
	currentSprite = sprites[ANIM_DEATH];
	currentSprite->SetCurrentFrame(0);
	currentSprite->SetCurrentFrameX(0);
	currentSprite->SetCurrentFrameY(0);
	isDying = true;
}

void HoriFrog::AI()
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
				player->AddIFrames(4);
			}
		}
	}

	//Moving horizontally
	if (rhythm->GetIsOnBeat() && isDying == false && isSpawned == true)
	{
		scootCount++;
		if (scootCount >= 2)
		{
			if (isMovingLeft)
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
					}
					else
					{
						facing = -1.0f;
						isMovingLeft = false;
					}
				}
				else
				{
					facing = -1.0f;
					isMovingLeft = false;
				}
			}
			else
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
					}
					else
					{
						facing = 1.0f;
						isMovingLeft = true;
					}
				}
				else
				{
					facing = 1.0f;
					isMovingLeft = true;
				}
			}
			scootCount = 0;
		}

		if (isMoving)
		{
			currentSprite = sprites[ANIM_SCOOT_SIDE];
		}
		else
		{
			currentSprite = sprites[ANIM_CROAK];
		}
	}

	//Doing the move
	float nextX = myLevel->GetTile(gridX, gridY)->GetX();
	float nextY = myLevel->GetTile(gridX, gridY)->GetY() + 10.0f;

	MoveTowards(nextX, nextY);
}
