//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2018 Media Design School.
//
// File Name	: 
// Description	: 
// Author		: Your Name
// Mail			: your.name@mediadesign.school.nz
//

// Library Includes

// Local Includes
#include "resource.h"
#include "utils.h"

// This Includes
#include "bullet.h"

// Static Variables

// Static Function Prototypes

// Implementation

Bullet::Bullet()
	: m_fVelocityX(0.0f)
	, m_fVelocityY(0.0f)
{

}

Bullet::~Bullet()
{

}

bool Bullet::Initialise(float _fPosX, float _fPosY, float _fVelocityX, float _fVelocityY)
{
	VALIDATE(CEntity::Initialise(IDB_BALLSPRITE, IDB_BALLMASK));
	playerBullet = true;
	isActive = false;

	m_fX = _fPosX;
	m_fY = _fPosY;

	m_fVelocityX = _fVelocityX;
	m_fVelocityY = _fVelocityY;

	return (true);
}

void Bullet::Draw()
{
	if (isActive)
	{
		CEntity::Draw();
	}
}

void Bullet::Process(float _fDeltaTick)
{
	if (isActive)
	{
		m_fX += m_fVelocityX * _fDeltaTick;
		m_fY += m_fVelocityY * _fDeltaTick;
		if (m_fY < 0 || m_fY > gameHeight) //If outside of the window
		{
			isActive = false;
		}
	}
	else
	{
		m_fVelocityX = 0;
		m_fVelocityY = 0;
		m_fX = -1000.0f;
		m_fY = -1000.0f;
	}

	CEntity::Process(_fDeltaTick);
}

void Bullet::Shoot(float _fPosX, float _fPosY, float _fVelocityX, float _fVelocityY, bool _playerBullet)
{
	m_fX = _fPosX;
	m_fY = _fPosY;
	m_fVelocityX = _fVelocityX;
	m_fVelocityY = _fVelocityY;
	playerBullet = _playerBullet;
	isActive = true;
}

float Bullet::GetVelocityX() const
{
	return (m_fVelocityX);
}

float Bullet::GetVelocityY() const
{
	return (m_fVelocityY);
}

void Bullet::SetVelocityX(float _fX)
{
	m_fVelocityX = _fX;
}

void Bullet::SetVelocityY(float _fY)
{
	m_fVelocityY = _fY;
}

float Bullet::GetRadius() const
{
	return (GetWidth() / 2.0f);
}

void Bullet::SetIsActive(bool _active)
{
	isActive = _active;
}

bool Bullet::GetIsActive() const
{
	return isActive;
}

bool Bullet::GetIsPlayerBullet() const
{
	return playerBullet;
}