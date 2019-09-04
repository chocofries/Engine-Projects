
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

#if !defined(__MENU_H__)
#define __MENU_H__

// Library Includes
#include <vector>

// Local Includes
#include "entity.h"
#include "button.h"

// Types

// Constants

// Prototypes

class Menu : public CEntity
{
	// Member Functions
public:
	Menu();
	virtual ~Menu();

	virtual bool Initialise();

	virtual void Draw();
	virtual void Process(float _fDeltaTick);

protected:

private:
	Menu(const Menu& _kr);
	Menu& operator= (const Menu& _kr);

	// Member Variables
public:

protected:


private:
	bool menuActive;
	int menuID; //0 = main menu, 1 = win screen, 2 = game over
	int currentButton; //Index of the currently selected button
	std::vector<Button*> vecButtons;

};
// NOTE: The window size should be fixed at 1600 x 900, make the menu BMP that size

#endif    // __MENU_H__

