
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

#if !defined(__ENEMYSMALL_H__)
#define __ENEMYSMALL_H__

// Library Includes
#include <vector>

// Local Includes
#include "entity.h"
#include "bullet.h"

// Types

// Constants

// Prototypes

class EnemySmall : public CEntity
{
	// Member Functions
public:
	EnemySmall();
	virtual ~EnemySmall();

	virtual bool Initialise();

	virtual void Draw();
	virtual void Process(float _fDeltaTick);

	void SetHit(bool _b);
	bool IsHit() const;
	void CheckBulletCollision();
	void SetBulletVector(std::vector<Bullet*>* _vecBullets);
	void SetEnemySmallVector(std::vector<EnemySmall*>* _vecEnemySmall);
	void SetMoving(bool _moveRight);
	void SetMoveDown(bool _moveDown);
	void Movement(float _fDeltaTick);
	void Combat(float _fDeltaTick);
	void SetSpeeds(int _moveSpeed, int _shootSpeed);

protected:

private:
	EnemySmall(const EnemySmall& _kr);
	EnemySmall& operator= (const EnemySmall& _kr);

	// Member Variables
public:

protected:
	bool m_bHit;

private:
	std::vector<Bullet*>* vecBullets;
	std::vector<EnemySmall*>* vecEnemySmall;
	float HSpeed;
	float bulletSpeed;
	float newY;
	int health;
	int shootInterval;
	bool moveRight;
	bool moveDown;

	int playerMoveSpeed; // 1-3
	int playerShootSpeed; // 1-3
	int pirateMoveSpeed; // 1-3
	int pirateShootSpeed; // 1-3

};


#endif    // __ENEMYSMALL_H__

