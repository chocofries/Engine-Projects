
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
#include <mmsystem.h>

// Local Includes
#include "resource.h"
#include "utils.h"
#include "Game.h"

// This Include
#include "playership.h"
#include "BackBuffer.h"

// Static Variables

// Static Function Prototypes

// Implementation

PlayerShip::PlayerShip()
{
}


PlayerShip::~PlayerShip()
{
	isAlive = false;
}

bool PlayerShip::Initialise()
{
	VALIDATE(CEntity::Initialise(IDB_PADDLESPRITE, IDB_PADDLEMASK));
	InitiateAnimation(8, 8, 0, 0, 0.04166f);
	HalfShipW = static_cast<float>(m_pSprite->GetFrameWidth() / 2.0);
	HalfShipH = static_cast<float>(m_pSprite->GetFrameHeight() / 2.0);
	moveKeyHeld = false;
	isAlive = true;

	CGame& game = CGame::GetInstance();

	health = 10;
	HSpeed = 0;
	VSpeed = 0;
	HMaxSpeed = 300.0f;
	VMaxSpeed = 200.0f;
	bulletSpeed = 400.0f;
	shootInterval = 30;
	vecBullets = nullptr;
	
	gameInst = game.GetAppInstance();
	return (true);
}

void PlayerShip::Draw()
{
	if (isAlive)
	{
		CEntity::Draw();

		//Draw Health Bar
		if (health < 10)
		{
			CGame& game = CGame::GetInstance();
			HWND hwnd = game.GetWindow();
			HDC hdc_b = game.GetBackBuffer()->GetBFDC();

			HBRUSH black_brush = CreateSolidBrush(RGB(20, 20, 20));
			HBRUSH bar_brush = CreateSolidBrush(RGB(250, 200, 20));

			RECT aRect = { (int)(m_fX - 41.0f), (int)(m_fY + 66.0f),  (int)(m_fX + 41.0f), (int)(m_fY + 60.0f) };
			RECT bRect = { (int)(m_fX - 40.0f), (int)(m_fY + 65.0f),  (int)(m_fX - 40.0f + (8.0f*health)), (int)(m_fY + 61.0f )};

			FillRect(hdc_b, &aRect, black_brush);
			FillRect(hdc_b, &bRect, bar_brush);

			DeleteObject(black_brush);
			DeleteObject(bar_brush);

			ReleaseDC(hwnd, hdc_b);
		}
	}
	
	/*CBackBuffer* pBackBuffer = CGame::GetInstance().GetBackBuffer();
	TCHAR text[256];
	swprintf_s(text, 256, L"MoveKeyHeld: %d", moveKeyHeld);
	TextOut(pBackBuffer->GetBFDC(), m_fX, m_fY - 30, text, wcslen(text));
	swprintf_s(text, 256, L"halfWidth: %f", HalfShipW);
	TextOut(pBackBuffer->GetBFDC(), m_fX, m_fY - 50, text, wcslen(text));
	swprintf_s(text, 256, L"halfHeight: %f", HalfShipH);
	TextOut(pBackBuffer->GetBFDC(), m_fX, m_fY - 70, text, wcslen(text));*/
}

void PlayerShip::Process(float _fDeltaTick)
{
	if (isAlive)
	{
		Movement(_fDeltaTick);
		Combat(_fDeltaTick);
		CheckBulletCollision();

		CEntity::Process(_fDeltaTick);
		if (health <= 0)
		{
			isAlive = false;
		}
	}
}

void PlayerShip::Movement(float _fDeltaTick)
{
	float acceleration = 2600.0f;
	float friction = 4.0f;

	//Horizontal Movement Input
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		if (HSpeed > -HMaxSpeed)
		{
			HSpeed -= acceleration * _fDeltaTick;
		}
		moveKeyHeld = true;
	}
	else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		if (HSpeed < HMaxSpeed)
		{
			HSpeed += acceleration * _fDeltaTick;
		}
		moveKeyHeld = true;
	}

	//Vertical Movement Input
	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		if (VSpeed > -VMaxSpeed)
		{
			VSpeed -= acceleration * _fDeltaTick;
		}
		moveKeyHeld = true;
	}
	else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		if (VSpeed < VMaxSpeed)
		{
			VSpeed += acceleration * _fDeltaTick;
		}
		moveKeyHeld = true;
	}

	if (!((GetAsyncKeyState(VK_LEFT) & 0x8000) ||
		(GetAsyncKeyState(VK_RIGHT) & 0x8000) ||
		(GetAsyncKeyState(VK_UP) & 0x8000) ||
		(GetAsyncKeyState(VK_DOWN) & 0x8000)))
	{
		moveKeyHeld = false;
	}

	//Movement Processing
	if (HSpeed > HMaxSpeed)
	{
		HSpeed = HMaxSpeed;
	}
	if (HSpeed < -HMaxSpeed)
	{
		HSpeed = -HMaxSpeed;
	}

	m_fX += HSpeed * _fDeltaTick;
	m_fY += VSpeed * _fDeltaTick;

	if (!((GetAsyncKeyState(VK_LEFT) & 0x8000) ||
		(GetAsyncKeyState(VK_RIGHT) & 0x8000)))
	{
		if (HSpeed > friction)
		{
			HSpeed -= friction;
		}
		else if (HSpeed < -friction)
		{
			HSpeed += friction;
		}
		else
		{
			HSpeed = 0;
		}
	}

	if (!((GetAsyncKeyState(VK_UP) & 0x8000) ||
		(GetAsyncKeyState(VK_DOWN) & 0x8000)))
	{
		if (VSpeed > friction)
		{
			VSpeed -= friction;
		}
		else if (VSpeed < -friction)
		{
			VSpeed += friction;
		}
		else
		{
			VSpeed = 0;
		}
	}


	float hBuffer = 0.00f;
	float vBuffer = 0.5f;

	//Clamping Movement Area (Vertical)
	if (m_fY - HalfShipH <= (gameHeight*vBuffer))
	{
		m_fY = HalfShipH + (gameHeight*vBuffer);
		VSpeed = 0;
	}
	else if (m_fY + HalfShipH >= gameHeight)
	{
		m_fY = gameHeight - HalfShipH;
		VSpeed = 0;
	}

	//Clamping Movement Area (Horizontal)
	if (m_fX - (HalfShipW / 2) <= (gameWidth*hBuffer))
	{
		m_fX = (HalfShipW / 2) + (gameWidth*hBuffer);
		HSpeed = 0;
	}
	else if (m_fX + (HalfShipW / 2) >= (gameWidth - (gameWidth*hBuffer)))
	{
		m_fX = (gameWidth - (gameWidth*hBuffer)) - (HalfShipW / 2);
		HSpeed = 0;
	}
}

void PlayerShip::SetBulletVector(std::vector<Bullet*>* _vecBullets)
{
	vecBullets = _vecBullets;
}

void PlayerShip::Combat(float _fDeltaTick)
{
	Bullet* tempBullet;
	static int shootTimer = 0;

	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
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
 						tempBullet->Shoot(m_fX,m_fY,0, -bulletSpeed, true);
						//PlaySoundA(MAKEINTRESOURCEA(IDR_GUN2), gameInst, SND_RESOURCE | SND_ASYNC);
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

void PlayerShip::CheckBulletCollision()
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
					if (it->GetIsActive() == true && it->GetIsPlayerBullet() == false) //Check if a bullet is active
					{
						tempBullet = it;

						float bulletR = tempBullet->GetRadius()*0.1f;
						float bulletX = tempBullet->GetX();
						float bulletY = tempBullet->GetY();

						float playerX = m_fX;
						float playerY = m_fY;

						float playerH = 40.0f;
						float playerW = 40.0f;

						if ((bulletX + bulletR > playerX - playerW / 2) &&
							(bulletX - bulletR < playerX + playerW / 2) &&
							(bulletY + bulletR > playerY - playerH / 2) &&
							(bulletY - bulletR < playerY + playerH / 2))
						{
							CGame& game = CGame::GetInstance();
							health -= game.GetMultiplier();
							tempBullet->SetIsActive(false);
						}
					}
				}
			}
		}
	}
}

bool PlayerShip::GetAlive() const
{
	return isAlive;
}

void PlayerShip::SetSpeeds(int _moveSpeed, int _shootSpeed)
{
	switch (_moveSpeed)
	{
	case 1:
	{
		HMaxSpeed = 100.0f;
		VMaxSpeed = 50.0f;
		break;
	}
	case 2:
	{
		HMaxSpeed = 300.0f;
		VMaxSpeed = 200.0f;
		break;
	}
	case 3:
	{
		HMaxSpeed = 500.0f;
		VMaxSpeed = 400.0f;
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
		shootInterval = 40;
		break;
	}
	case 3:
	{
		shootInterval = 10;
		break;
	}
	default:
		break;
	}
}
