
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

// Library Includes

// Local Includes
#include "resource.h"
#include "utils.h"

// This Include
#include "menu.h"

// Static Variables

// Static Function Prototypes

// Implementation

Menu::Menu()
{

}

Menu::~Menu()
{

}

bool Menu::Initialise()
{
	VALIDATE(CEntity::Initialise(IDB_TITLE, IDB_TITLEMASK));

	return (true);
}

void Menu::Draw()
{
	CEntity::Draw();
}

void Menu::Process(float _fDeltaTick)
{
	CEntity::Process(_fDeltaTick);
}

