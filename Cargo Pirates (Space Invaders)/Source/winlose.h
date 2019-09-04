
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

#if !defined(__WINLOSE_H__)
#define __WINLOSE_H__

// Library Includes
#include <vector>

// Local Includes
#include "entity.h"
#include "button.h"

// Types

// Constants

// Prototypes

class WinLose : public CEntity
{
	// Member Functions
public:
	WinLose();
	virtual ~WinLose();

	virtual bool Initialise();

	virtual void Draw();
	virtual void Process(float _fDeltaTick);

	void SetActive(bool _active);
	bool GetActive() const;

protected:

private:
	WinLose(const WinLose& _kr);
	WinLose& operator= (const WinLose& _kr);

	// Member Variables
public:

protected:


private:
	bool isActive;
};
// NOTE: The window size should be fixed at 1600 x 900, make the menu BMP that size

#endif    // __WINLOSE_H__

