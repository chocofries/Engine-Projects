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
#include "winlose.h"

// Static Variables

// Static Function Prototypes

// Implementation

WinLose::WinLose()
{

}

WinLose::~WinLose()
{

}

bool WinLose::Initialise()
{
	VALIDATE(CEntity::Initialise(IDB_WINLOSE, IDB_WINLOSEMASK));
	InitiateAnimation(2, 1, 0, 0, 0.0f);

	return (true);
}

void WinLose::Draw()
{
	CEntity::Draw();
}

void WinLose::Process(float _fDeltaTick)
{
	CEntity::Process(_fDeltaTick);
}

void WinLose::SetActive(bool _active)
{
	isActive = _active;
}

bool WinLose::GetActive() const
{
	return isActive;
}
