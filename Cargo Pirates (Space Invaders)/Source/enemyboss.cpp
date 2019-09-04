
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
#include "EnemyBoss.h"
#include "backbuffer.h"

// Static Variables

// Static Function Prototypes

// Implementation

EnemyBoss::EnemyBoss()
{

}

EnemyBoss::~EnemyBoss()
{

}

bool EnemyBoss::Initialise()
{
	VALIDATE(CEntity::Initialise(IDB_BOSS, IDB_BOSSMASK));
	InitiateAnimation(8, 8, 0, 0, 0.04166f);
	isAlive = true;
	vecBullets = nullptr;
	health = 40;
	HSpeed = 200.0f;
	shootInterval = 10;
	bulletSpeed = 400.0f;
	isShooting = false;
	bossActive = false;
	timeElapsed = 0;

	return (true);
}

void EnemyBoss::Draw()
{
	if (bossActive)
	{
		if (isAlive)
		{
			CEntity::Draw();

			//Draw Health Bar
			if (health < 40)
			{
				CGame& game = CGame::GetInstance();
				HWND hwnd = game.GetWindow();
				HDC hdc_b = game.GetBackBuffer()->GetBFDC();

				HBRUSH black_brush = CreateSolidBrush(RGB(20, 20, 20));
				HBRUSH red_brush = CreateSolidBrush(RGB(250, 20, 20));

				RECT aRect = { (int)(m_fX - 51.0f), (int)(m_fY - 16.0f),  (int)(m_fX + 51.0f), (int)(m_fY - 10.0f) };
				RECT bRect = { (int)(m_fX - 50.0f), (int)(m_fY - 15.0f),  (int)(m_fX - 50.0f + (2.5f*health)), (int)(m_fY - 11.0f) };

				FillRect(hdc_b, &aRect, black_brush);
				FillRect(hdc_b, &bRect, red_brush);

				DeleteObject(black_brush);
				DeleteObject(red_brush);

				ReleaseDC(hwnd, hdc_b);
			}
		}
	}
}

void EnemyBoss::Activate()
{
	bossActive = true;
}

void EnemyBoss::SetSpeeds(int _moveSpeed, int _shootSpeed)
{
	switch (_moveSpeed)
	{
	case 1:
	{
		HSpeed = 50.0f;
		break;
	}
	case 2:
	{
		HSpeed = 200.0f;
		break;
	}
	case 3:
	{
		HSpeed = 500.0f;
		break;
	}
	default:
		break;
	}

	switch (_shootSpeed)
	{
	case 1:
	{
		shootInterval = 30;
		break;
	}
	case 2:
	{
		shootInterval = 10;
		break;
	}
	case 3:
	{
		shootInterval = 5;
		break;
	}
	default:
		break;
	}
}

void EnemyBoss::Process(float _fDeltaTick)
{
	if (isAlive)
	{
		if (bossActive)
		{
			Movement(_fDeltaTick);
			Combat(_fDeltaTick);
			CheckBulletCollision();
			CEntity::Process(_fDeltaTick);
		
			if (health <= 0)
			{
				isAlive = false;

				//Add Score
				CGame& game = CGame::GetInstance();
				game.SetScore(100 * game.GetMultiplier(), true);
			}
		}
	}
}

void EnemyBoss::SetAlive(bool _b)
{
	isAlive = _b;
}

bool EnemyBoss::IsAlive() const
{
	return (isAlive);
}

void EnemyBoss::CheckBulletCollision()
{
	Bullet* tempBullet;

	if (isAlive)
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

						float enemyH = 100.0f;
						float enemyW = 80.0f;

						if ((bulletX + bulletR > enemyX - enemyW / 2) &&
							(bulletX - bulletR < enemyX + enemyW / 2) &&
							(bulletY + bulletR > enemyY - enemyH / 2) &&
							(bulletY - bulletR < enemyY + enemyH / 2))
						{
							health -= 1;
							tempBullet->SetIsActive(false);
						}
					}
				}
			}
		}
	}
}

void EnemyBoss::SetBulletVector(std::vector<Bullet*>* _vecBullets)
{
	vecBullets = _vecBullets;
}

void EnemyBoss::Movement(float _fDeltaTick)
{
	static bool moveRight = false;

	if (moveRight) //Moving Right
	{
		if (m_fX + (80.0f / 2) < gameWidth) //If within bounds
		{
			m_fX += HSpeed * _fDeltaTick;
		}
		else
		{
			moveRight = false;
		}
	}
	else  //Moving Left
	{
		if (m_fX - (80.0f / 2) > 0) //If within bounds
		{
			m_fX -= HSpeed * _fDeltaTick;
		}
		else
		{
			moveRight = true;
		}
	}
}

void EnemyBoss::Combat(float _fDeltaTick)
{
	Bullet* tempBullet;
	static int shootTimer = 0;
	timeElapsed += _fDeltaTick;

	if (timeElapsed >= 1.0f)
	{
		if (isShooting == true)
		{
			isShooting = false;
		}
		else
		{
			isShooting = true;
		}
		timeElapsed = 0;
	}

	if (shootTimer <= 0.0f && isShooting == true)
	{
		if (vecBullets != nullptr)
		{
			if (!vecBullets->empty() && shootTimer <= 0)
			{
				for (Bullet* it : *vecBullets) //Iterate through vector
				{
					if (it->GetIsActive() == false) //Check if a bullet is inactive
					{
						float ranDir = static_cast<float>((rand() % (int)bulletSpeed) - (bulletSpeed/2));
						tempBullet = it;
						tempBullet->Shoot(m_fX, m_fY, ranDir, bulletSpeed, false);
						shootTimer = shootInterval;
						return;
					}
				}
			}
		}
	}

	if (shootTimer > 0)
	{
		float f = static_cast<float>(shootTimer);
		f -= 1.0f*_fDeltaTick;
		shootTimer = static_cast<int>(f);
	}
}