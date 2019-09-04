
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

#pragma once

#if !defined(__PLAYERSHIP_H__)
#define __PLAYERSHIP_H__

// Library Includes
#include <vector>

// Local Includes
#include "entity.h"
#include "Sprite.h"
#include "bullet.h"

// Types

// Constants

// Prototypes
class PlayerShip : public CEntity
{
	// Member Functions
public:
	PlayerShip();
	virtual ~PlayerShip();

	virtual bool Initialise();

	virtual void Draw();
	virtual void Process(float _fDeltaTick);

	void Movement(float _fDeltaTick);
	void Combat(float _fDeltaTick);
	void SetBulletVector(std::vector<Bullet*>* _vecBullets);
	void CheckBulletCollision();
	bool GetAlive() const;
	void SetSpeeds(int _moveSpeed, int _shootSpeed);

protected:

private:
	PlayerShip(const PlayerShip& _kr);
	PlayerShip& operator= (const PlayerShip& _kr);

	// Member Variables
public:

protected:

private:
	float HalfShipW;
	float HalfShipH;
	float HSpeed;
	float VSpeed;
	float HMaxSpeed;
	float VMaxSpeed;
	float bulletSpeed;
	int health;
	int shootInterval;
	bool moveKeyHeld; //If a movement key is currently held
	bool isAlive;
	std::vector<Bullet*>* vecBullets;
	HINSTANCE gameInst;

};

#endif    // __PLAYERSHIP_H__
