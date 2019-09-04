#include "scalpelhand.h"
#include "frog.h"

ScalpelHand::ScalpelHand(Rhythm* _rhythm, Player* _player, Level* _level, ScoreManager* _scoreManager)
{
	rhythm = _rhythm;
	player = _player;
	level = _level;
	scoreManager = _scoreManager;
}

ScalpelHand::~ScalpelHand()
{
	delete scalpelSprite;
}

void ScalpelHand::Initialise()
{
	scalpelSprite = new Sprite(this);
	scalpelSprite->InitSprite("Assets/HandEnemy.png", "ScalpelHandEnemy", 0.00000f, 1, 1, 1, 0, 2700, 639);
	scalpelSprite->SetIsActive(true);

	attackTimer = 0;
	isAttacking = true;

	xScale = 2700.0f;
	yScale = 639.0f;
	xRot = 0.0f;
	yRot = 0.0f;
	zRot = 1.0f;
	rotationAngle = 0.0f;

	restingX = 1080.0f;
	restingY = -1000.0f;
	nextX = restingX;
	nextY = 0;

	x = restingX;
	y = 0.0f;
	z = 95.0f;
}

void ScalpelHand::Update(double _dTime)
{
	deltaTime = _dTime;
	scalpelSprite->Update(deltaTime);

	int targetTime = 4;
	int waitTime = 8;

	if (isAttacking)
	{
		rotationAngle = 0.0f;

		if (rhythm->GetIsOnBeat())
		{
			if (attackTimer > waitTime) //Striking the player
			{
				nextX = 300.0f;
				attackTimer = 0;
				for (int ouch = 0; ouch < 8; ouch++)
				{
					Tile* temp = level->GetTile(ouch, targetY);
					temp->SetIsDanger(false);
				}
				isMoving = true;

				if (targetY == player->GetGridY()) //If the player is in range
				{
					if (player->GetIFrames() == 0)
					{
						player->StartCringe();
						level->BreakHeart(); // (; _ ;)
						player->AddIFrames(4);
					}
				}

				for (unsigned int i = 0; i <  (unsigned int)level->GetDequeFrogsSize(); i++)  //If frog is in range
				{
					Frog* tempFrog = level->GetFrog(i);
					if (tempFrog->GetIsActive() && tempFrog->GetIsSpawned())
					{
						if (tempFrog->GetGridY() == targetY)
						{
							tempFrog->StartDeath();  // (x . x)
						}
					}
				}

			}
			else
			{
				if (attackTimer < targetTime) //Targeting the player
				{
					nextX = restingX;
					nextY = player->GetY();
					targetY = player->GetGridY();
					isMoving = true;
				}
				else //Waiting before striking
				{
					//nextY = level->GetTile(0, targetY)->GetY();
					for (int ouch = 0; ouch < 8; ouch++)
					{
						Tile* temp = level->GetTile(ouch,targetY);
						temp->SetIsDanger(true);
					}
				}
				attackTimer++;
			}
		}
	}

	MoveTowards(nextX, nextY);
}

void ScalpelHand::Draw()
{
	scalpelSprite->Draw();
}

void ScalpelHand::SetIsAttacking(bool _isAttacking)
{
	isAttacking = _isAttacking;
}

void ScalpelHand::MoveTowards(float _x, float _y)
{
	float buffer = 10.0f;
	float moveSpeedX = rhythm->GetTickMS()*8.0f*(float)deltaTime;
	float moveSpeedY = rhythm->GetTickMS()*2.0f*(float)deltaTime;

	if (isMoving)
	{
		if (x >= _x - buffer && x <= _x + buffer && y >= _y - buffer && y <= _y + buffer)
		{
			isMoving = false;
		}

		if (x <= _x)
		{
			x += moveSpeedX;
		}
		if (x >= _x)
		{
			x += -moveSpeedX;
		}

		if (y <= _y)
		{
			y += moveSpeedY;
		}
		if (y >= _y)
		{
			y += -moveSpeedY;
		}
	}
}
