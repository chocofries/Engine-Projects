#include "vertifrog.h"

VertiFrog::VertiFrog(Rhythm * _rhythm, Player * _player, Level * _level, ScoreManager * _scoreManager, int _id)
{
	rhythm = _rhythm;
	player = _player;
	myLevel = _level;
	scoreManager = _scoreManager;
	frogID = _id;
}

VertiFrog::~VertiFrog()
{
	for (int i = 0; i < 6; i++)
	{
		if (sprites[i] != nullptr)
		{
			delete sprites[i];
		}
	}
}

void VertiFrog::Initialise()
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
	sprites[ANIM_CROAK]->InitSprite("Assets/Verti-CroakBang.png", "VertiCroakBang", animSpd / 16.0f, 16, 12, 2, 0, 1910, 272);

	sprites[ANIM_SCOOT_UP] = new Sprite(this);
	sprites[ANIM_SCOOT_UP]->InitSprite("Assets/Verti-ScootUp.png", "VertiScootUp", animSpd / 16.0f, 16, 10, 2, 0, 2002, 388);

	sprites[ANIM_SCOOT_DOWN] = new Sprite(this);
	sprites[ANIM_SCOOT_DOWN]->InitSprite("Assets/Verti-ScootDown.png", "VertiScootDown", animSpd / 16.0f, 16, 8, 2, 0, 2034, 458);

	sprites[ANIM_JUMP] = new Sprite(this);
	sprites[ANIM_JUMP]->InitSprite("Assets/Verti-Jump.png", "VertiJump", animSpd / 16.0f, 16, 11, 2, 0, 1927, 562);

	sprites[ANIM_DEATH] = new Sprite(this);
	sprites[ANIM_DEATH]->InitSprite("Assets/Verti-Death.png", "VertiDeath", animSpd / 24.0f, 24, 7, 4, 0, 1864, 1022);


	currentSprite = sprites[ANIM_CROAK];

	Tile* startTile = myLevel->GetTile(gridX, gridY);
	x = startTile->GetX();
	y = startTile->GetY() + 10.0f;
	startTile->SetTileState(TILE_FREE);

	//Init
	spawnTimer = 0;
	isMovingDown = false;
	scootCount = 0;
	facing = 1.0f;
	isDying = false;
	isSpawned = false;
}

void VertiFrog::Update(double dTime)
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

void VertiFrog::Draw()
{
	currentSprite->Draw();
}

void VertiFrog::Spawn()
{
	spawnTimer = 0;
	Tile* temp = myLevel->GetTile(spawnX, spawnY);
	temp->SetIsDanger(true);
	isActive = true;
	currentSprite->SetCurrentFrame(0);
	currentSprite->SetCurrentFrameX(0);
	currentSprite->SetCurrentFrameY(0);
}

void VertiFrog::StartDeath()
{
	myLevel->GetTile(gridX, gridY)->SetTileState(TILE_FREE);
	myLevel->GetTile(gridX, gridY)->SetIsDanger(false);
	currentSprite = sprites[ANIM_DEATH];
	currentSprite->SetCurrentFrame(0);
	currentSprite->SetCurrentFrameX(0);
	currentSprite->SetCurrentFrameY(0);
	isDying = true;
}

void VertiFrog::AI()
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

	//Moving vertically
	if (rhythm->GetIsOnBeat() && isDying == false && isSpawned == true)
	{
		scootCount++;
		if (scootCount >= 2)
		{
			if (isMovingDown)
			{
				if (gridY > 0)
				{
					if (myLevel->GetTile(gridX, gridY - 1)->GetTileState() != TILE_ENEMY) //If there isn't another enemy there
					{
						myLevel->GetTile(gridX, gridY)->SetTileState(TILE_FREE);
						myLevel->GetTile(gridX, gridY)->SetIsDanger(false);
						gridY -= 1;
						myLevel->GetTile(gridX, gridY)->SetTileState(TILE_ENEMY);
						facing = 1.0f;
						isMoving = true;
					}
					else
					{
						facing = -1.0f;
						isMovingDown = false;
					}
				}
				else
				{
					facing = -1.0f;
					isMovingDown = false;
				}
			}
			else
			{
				if (gridY < TILES_MAX_Y - 1)
				{
					if (myLevel->GetTile(gridX, gridY + 1)->GetTileState() != TILE_ENEMY) //If there isn't another enemy there
					{
						myLevel->GetTile(gridX, gridY)->SetTileState(TILE_FREE);
						myLevel->GetTile(gridX, gridY)->SetIsDanger(false);
						gridY += 1;
						myLevel->GetTile(gridX, gridY)->SetTileState(TILE_ENEMY);
						facing = -1.0f;
						isMoving = true;
					}
					else
					{
						facing = 1.0f;
						isMovingDown = true;
					}
				}
				else
				{
					facing = 1.0f;
					isMovingDown = true;
				}
			}
			scootCount = 0;
		}

		if (isMoving)
		{
			if (isMovingDown)
			{
				currentSprite = sprites[ANIM_SCOOT_DOWN];
			}
			else
			{
				currentSprite = sprites[ANIM_SCOOT_UP];
			}
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
