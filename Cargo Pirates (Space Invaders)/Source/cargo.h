
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

#if !defined(__CARGO_H__)
#define __CARGO_H__

// Library Includes
#include <vector>

// Local Includes
#include "entity.h"
#include "bullet.h"

// Types

// Constants

// Prototypes

class Cargo : public CEntity
{
	// Member Functions
public:
	Cargo();
	virtual ~Cargo();

	virtual bool Initialise();

	virtual void Draw();
	virtual void Process(float _fDeltaTick);

	void CheckBulletCollision();
	void SetBulletVector(std::vector<Bullet*>* _vecBullets);
	bool GetIsAlive() const;

protected:

private:
	Cargo(const Cargo& _kr);
	Cargo& operator= (const Cargo& _kr);

	// Member Variables
public:

protected:
	bool isAlive;

private:
	int health;
	std::vector<Bullet*>* vecBullets;
	float oX; //Offset X
	float oY; //Offset Y

};


#endif    // __CARGO_H__

