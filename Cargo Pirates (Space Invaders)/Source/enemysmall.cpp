
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2018 Media Design School.
//
// File Name	: Cargo Pirates (Space Invaders)
// Description	: 
// Author		: John Bu with support by Rory More
// Mail			: John.Bu@mediadesign.school.nz + ruairidh.more@mediadesign.school.nz
//

// Library Includes
#include <Windows.h>
// Local Includes
#include "resource.h"
#include "game.h"
#include "utils.h"

// This Include
#include "enemysmall.h"
#include "backbuffer.h"

// Static Variables

// Static Function Prototypes

// Implementation

EnemySmall::EnemySmall()
	: m_bHit(false)
{

}

EnemySmall::~EnemySmall()
{

}

bool EnemySmall::Initialise()
{
	VALIDATE(CEntity::Initialise(IDB_BRICKSPRITE, IDB_BRICKMASK));
	vecBullets = nullptr;
	vecEnemySmall = nullptr;
	health = 5;
	moveRight = true;
	moveDown = false;
	HSpeed = 50.0f;
	shootInterval = 15;
	bulletSpeed = 400.0f;
	newY = m_fY;

	return (true);
}

void EnemySmall::Draw()
{
	if (!m_bHit)
	{
		CEntity::Draw();

		//Draw Health Bar
		if (health < 5)
		{
			CGame& game = CGame::GetInstance();
			HWND hwnd = game.GetWindow();
			HDC hdc_b = game.GetBackBuffer()->GetBFDC();

			HBRUSH black_brush = CreateSolidBrush(RGB(20, 20, 20));
			HBRUSH red_brush = CreateSolidBrush(RGB(250, 20, 20));

			RECT aRect = { (int)(m_fX - 21.0f), (int)(m_fY - 16.0f),  (int)(m_fX + 21.0f), (int)(m_fY - 10.0f )};
			RECT bRect = { (int)(m_fX - 20.0f), (int)(m_fY - 15.0f),  (int)(m_fX - 20.0f + (8.0f*health)), (int)(m_fY - 11.0f )};

			FillRect(hdc_b, &aRect, black_brush);
			FillRect(hdc_b, &bRect, red_brush);

			DeleteObject(black_brush);
			DeleteObject(red_brush);

			ReleaseDC(hwnd, hdc_b);
		}
	}
}


void EnemySmall::Process(float _fDeltaTick)
{
	if (!m_bHit)
	{
		Movement(_fDeltaTick);
		Combat(_fDeltaTick);
		CheckBulletCollision();
		CEntity::Process(_fDeltaTick);
		if (health <= 0)
		{
			m_bHit = true;

			//Add Score
			CGame& game = CGame::GetInstance();
			game.SetScore(10*game.GetMultiplier(),true);
		}
	}
}

void EnemySmall::SetHit(bool _b)
{
	m_bHit = _b;
}

bool EnemySmall::IsHit() const
{
	return (m_bHit);
}

void EnemySmall::CheckBulletCollision()
{
	Bullet* tempBullet;

	if (!m_bHit)
	{
		if (vecBullets != nullptr)
		{
			if (!vecBullets->empty())
			{
				for (Bullet* it : *vecBullets) //Iterate through vector
				{
					if (it->GetIsActive() == true && it->GetIsPlayerBullet() == true) //Check if a bullet is active
					{
						tempBullet = it;

						float bulletR = tempBullet->GetRadius()*0.1f;
						float bulletX = tempBullet->GetX();
						float bulletY = tempBullet->GetY();

						float enemyX = m_fX;
						float enemyY = m_fY;
						
						float enemyH = this->GetHeight();
						float enemyW = this->GetWidth();

						if ((bulletX + bulletR > enemyX - enemyW / 2) &&
							(bulletX - bulletR < enemyX + enemyW / 2) &&
							(bulletY + bulletR > enemyY - enemyH / 2) &&
							(bulletY - bulletR < enemyY + enemyH / 2))
						{
							health -= 1;
							//m_bHit = true;
							tempBullet->SetIsActive(false);
						}
					}
				}
			}
		}
	}
}

void EnemySmall::SetBulletVector(std::vector<Bullet*>* _vecBullets)
{
	vecBullets = _vecBullets;
}

void EnemySmall::SetEnemySmallVector(std::vector<EnemySmall*>* _vecEnemySmall)
{
	vecEnemySmall = _vecEnemySmall;
}

void EnemySmall::SetMoving(bool _moveRight)
{
	moveRight = _moveRight;
}

void EnemySmall::Movement(float _fDeltaTick)
{
	if (m_fY >= newY)
	{
		if (moveRight) //Moving Right
		{
			if (m_fX + (this->GetWidth() / 2) < gameWidth) //If within bounds
			{
				m_fX += HSpeed * _fDeltaTick;
			}
			else
			{
				if (vecEnemySmall != nullptr)
				{
					if (!vecEnemySmall->empty())
					{
						for (EnemySmall* it : *vecEnemySmall) //Iterate through vector
						{
							it->SetMoving(false);
							it->SetMoveDown(true);
						}
					}
				}
			}
		}
		else  //Moving Left
		{
			if (m_fX - (this->GetWidth() / 2) > 0) //If within bounds
			{
				m_fX -= HSpeed * _fDeltaTick;
			}
			else
			{
				if (vecEnemySmall != nullptr)
				{
					if (!vecEnemySmall->empty())
					{
						for (EnemySmall* it : *vecEnemySmall) //Iterate through vector
						{
							it->SetMoving(true);
							it->SetMoveDown(true);
						}
					}
				}
			}
		}
	}

	//Moving Downwards
	if (moveDown)
	{
		newY = m_fY + 30;
		moveDown = false;
	}
	if (m_fY < newY)
	{
		m_fY += (HSpeed*4.0f) * _fDeltaTick;
	}
}

void EnemySmall::Combat(float _fDeltaTick)
{
	Bullet* tempBullet;
	static float shootTimer = static_cast<float>(rand() % shootInterval + 5);

	if (shootTimer > 0)
	{
		shootTimer -= 1 * _fDeltaTick;
	}
	else
	{
		if (vecBullets != nullptr && shootTimer <= 0)
		{
			if (!vecBullets->empty())
			{
				for (Bullet* it : *vecBullets) //Iterate through vector
				{
					if (it->GetIsActive() == false) //Check if a bullet is inactive
					{
						tempBullet = it;
						tempBullet->Shoot(m_fX, m_fY, 0, bulletSpeed, false);
						shootTimer = static_cast<float>(rand() % shootInterval + 5);
						return;
					}
				}
			}
		}
	}

}

void EnemySmall::SetSpeeds(int _moveSpeed, int _shootSpeed)
{
	switch (_moveSpeed)
	{
	case 1:
	{
		HSpeed = 20.0f;
		break;
	}
	case 2:
	{
		HSpeed = 50.0f;
		break;
	}
	case 3:
	{
		HSpeed = 200.0f;
		break;
	}
	default:
		break;
	}

	switch (_shootSpeed)
	{
	case 1:
	{
		shootInterval = 60;
		break;
	}
	case 2:
	{
		shootInterval = 15;
		break;
	}
	case 3:
	{
		shootInterval = 1;
		break;
	}
	default:
		break;
	}
}

void EnemySmall::SetMoveDown(bool _moveDown)
{
	moveDown = _moveDown;
}