
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

#if !defined(__ENEMYBOSS_H__)
#define __ENEMYBOSS_H__

// Library Includes
#include <vector>

// Local Includes
#include "entity.h"
#include "bullet.h"

// Types

// Constants

// Prototypes

class EnemyBoss : public CEntity
{
	// Member Functions
public:
	EnemyBoss();
	virtual ~EnemyBoss();

	virtual bool Initialise();

	virtual void Draw();
	virtual void Process(float _fDeltaTick);

	void SetAlive(bool _b);
	bool IsAlive() const;
	void CheckBulletCollision();
	void SetBulletVector(std::vector<Bullet*>* _vecBullets);
	void Movement(float _fDeltaTick);
	void Combat(float _fDeltaTick);
	void Activate(); //Activate the boss
	void SetSpeeds(int _moveSpeed, int _shootSpeed);

protected:

private:
	EnemyBoss(const EnemyBoss& _kr);
	EnemyBoss& operator= (const EnemyBoss& _kr);

	// Member Variables
public:

protected:

private:
	std::vector<Bullet*>* vecBullets;
	float HSpeed;
	float bulletSpeed;
	float timeElapsed;
	int health;
	int shootInterval;
	bool isAlive;
	bool isShooting;
	bool bossActive;

};


#endif    // __ENEMYBOSS_H__

