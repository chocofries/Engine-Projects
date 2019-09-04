
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

#if !defined(__CARGOSHIP_H__)
#define __CARGOSHIP_H__

// Library Includes
#include <vector>

// Local Includes
#include "entity.h"
#include "cargo.h"

// Types

// Constants

// Prototypes


class CargoShip : public CEntity
{
	// Member Functions
public:
	CargoShip();
	virtual ~CargoShip();

	virtual bool Initialise();

	virtual void Draw();
	virtual void Process(float _fDeltaTick);

	void InitiateCargo();
	void SetBulletVector(std::vector<Bullet*>* _vecBullets);

protected:

private:
	CargoShip(const CargoShip& _kr);
	CargoShip& operator= (const CargoShip& _kr);

	// Member Variables
public:

protected:
	bool m_bHit;

private:
	std::vector<Cargo*> vecCargo;
	std::vector<Bullet*>* vecBullets;
	int cargoNum; //Number of containers left (still active)

};


#endif    // __CARGOSHIP_H__

