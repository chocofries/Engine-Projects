//
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

#include "Credits.h"

Credits::Credits()
{
}


Credits::~Credits()
{
}


bool Credits::Initialise()
{
	//VALIDATE(CEntity::Initialise(IDB_BITMAP3, IDB_BITMAP4));
	CreditsActive = true;
	//->SetX(m_fX - 34.0f + (22.0f*column));
	//->SetY(m_fY - 92.0f + (40.0f*row));
	return (true);
}

void Credits::Draw()
{
	CEntity::Draw();
}

void Credits::Process(float _fDeltaTick)
{
	//Move from side of screen across
	HSpeed = 50.0f;
	m_fX += HSpeed * _fDeltaTick;
	if (m_fX <= -500)
	{
		m_fX = 500;
		HSpeed = 0;
		CreditsActive = false;
		SetCreditsActive(CreditsActive);
	}
	CEntity::Process(_fDeltaTick);
}

void Credits::SetCreditsActive(bool)
{

}

bool Credits::GetCreditsActive()
{
	return CreditsActive;
}
