#pragma once


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

#if !defined(__Credits_H__)
#define __Credits_H__

// Library Includes

// Local Includes
#include "entity.h"

// Types

// Constants

// Prototypes

class Credits : public CEntity
{
	// Member Functions
public:
	Credits();
	virtual ~Credits();

	virtual bool Initialise();

	virtual void Draw();
	virtual void Process(float _fDeltaTick);
	void SetCreditsActive(bool);
	bool GetCreditsActive();

protected:

private:
	Credits(const Credits& _kr);
	Credits& operator= (const Credits& _kr);

	// Member Variables
public:

protected:


private:
	bool CreditsActive;
	float HSpeed = 50.0f;

};
#endif    // __Credits_H__