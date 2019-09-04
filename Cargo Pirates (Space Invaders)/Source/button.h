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

#if !defined(__BUTTON_H__)
#define __BUTTON_H__

// Library Includes
#include <vector>

// Local Includes
#include "entity.h"
//#include "bullet.h"

// Types

// Constants

// Prototypes

class Button : public CEntity
{
	// Member Functions
public:
	Button();
	virtual ~Button();

	virtual bool Initialise();

	virtual void Draw();
	virtual void Process(float _fDeltaTick);

	void SetIndex(int _index);
	void SetCurrentButton(int _cb);

protected:

private:
	Button(const Button& _kr);
	Button& operator= (const Button& _kr);

	// Member Variables
public:

protected:


private:
	bool menuActive;
	int currentButton; //Index of the currently selected button
	int myButtonIndex; //Index of this button

};


#endif    // __BUTTON_H__

