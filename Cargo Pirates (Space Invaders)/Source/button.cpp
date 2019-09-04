//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2018 Media Design School.
//
// File Name	: 
// Description	: 
// Author		: Your Name
// Mail			: your.name@mediadesign.school.nz
//

// Library Includes

// Local Includes
#include "resource.h"
#include "utils.h"
#include "windows.h"
#include "backbuffer.h"

// This Include
#include "button.h"
#include "level.h"
#include "game.h"

// Static Variables

// Static Function Prototypes

// Implementation

Button::Button()
{

}

Button::~Button()
{

}

bool Button::Initialise()
{
	VALIDATE(CEntity::Initialise(IDB_MENUBUTTON, IDB_MENUBUTTONMASK));
	InitiateAnimation(2, 2, 0, 0, 0.0f);
	currentButton = 1;
	myButtonIndex = 1;
	return (true);
}

void Button::Draw()
{
	CEntity::Draw();

	//CBackBuffer* pBackBuffer = CGame::GetInstance().GetBackBuffer();
	//TCHAR text[256];
	//swprintf_s(text, 256, L"CurrentButton: %d", currentButton);
	//TextOut(pBackBuffer->GetBFDC(), m_fX, m_fY - 30, text, wcslen(text));

}

void Button::SetIndex(int _index)
{
	myButtonIndex = _index;
}

void Button::SetCurrentButton(int _cb)
{
	currentButton = _cb;
}

void Button::Process(float _fDeltaTick)
{
	
	if (currentButton == myButtonIndex)
	{
		if (myButtonIndex == 1)
		{
			SetCurrentFrame(1, 0);
		}
		else
		{
			SetCurrentFrame(1, 1);
		}
	}
	else
	{
		if (myButtonIndex == 1)
		{
			SetCurrentFrame(0, 0);
		}
		else
		{
			SetCurrentFrame(0, 1);
		}
	}

	CEntity::Process(_fDeltaTick);
}

