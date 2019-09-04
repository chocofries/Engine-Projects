//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2018 Media Design School
//
// File Name	: 
// Description	: 
// Author		: Your Name
// Mail			: your.name@mediadesign.school.nz
//

#pragma once

#if !defined(__BULLET_H__)
#define __BULLET_H__

// Library Includes

// Local Includes
#include "entity.h"

// Types

// Constants

// Prototypes
class CSprite;

class Bullet : public CEntity
{
	// Member Functions
public:
	Bullet();
	virtual ~Bullet();

	virtual bool Initialise(float _fPosX, float _fPosY, float _fVelocityX, float _fVelocityY);

	virtual void Draw();
	virtual void Process(float _fDeltaTick);

	float GetVelocityX() const;
	float GetVelocityY() const;
	void SetVelocityX(float _fX);
	void SetVelocityY(float _fY);

	float GetRadius() const;

	void SetIsActive(bool _active);
	bool GetIsActive() const;
	bool GetIsPlayerBullet() const;

	void Shoot(float _fPosX, float _fPosY, float _fVelocityX, float _fVelocityY, bool _playerBullet);

protected:

private:
	Bullet(const Bullet& _kr);
	Bullet& operator= (const Bullet& _kr);

	// Member Variables
public:

protected:
	float m_fVelocityX;
	float m_fVelocityY;

private:
	bool isActive; //if the bullet is active or not
	bool playerBullet; //if the bullet belongs to the player
};


#endif    // __BULLET_H__
