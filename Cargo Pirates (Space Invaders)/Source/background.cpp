
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
#include "backbuffer.h"
#include "game.h"

// This Include
#include "background.h"

// Static Variables

// Static Function Prototypes

// Implementation

CBackGround::CBackGround()
{

}

CBackGround::~CBackGround()
{

}

bool CBackGround::Initialise()
{
	VALIDATE(CEntity::Initialise(IDB_BACKGROUND, IDB_BACKGROUNDMASK));

	hInstance = CGame::GetInstance().GetAppInstance();
	bgHBMP = (HBITMAP)LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BACKGROUND));
	pBackBuffer = CGame::GetInstance().GetBackBuffer();
	hwnd = pBackBuffer->GetHWND();
	bX = 0;
	bY = 0;
	bW = static_cast<float>(m_pSprite->GetWidth());
	bH = static_cast<float>(m_pSprite->GetHeight());
	scrollY = 0;

	RECT rect;
	if (GetWindowRect(hwnd, &rect))
	{
		winW = (float)(rect.right - rect.left);
		winH = (float)(rect.bottom - rect.top);
	}

	return (true);
}

void CBackGround::Draw()
{
	//CEntity::Draw();
}

void CBackGround::DrawTiled(float _scrollSpeed, float _fDeltaTick)
{
	HDC windowDC = pBackBuffer->GetBFDC();
	HDC bitDC = CreateCompatibleDC(windowDC);
	HBITMAP oldBM;

	GetObject(bgHBMP, sizeof(BITMAP), &bgBMP);
	oldBM = static_cast<HBITMAP>(SelectObject(bitDC, bgHBMP));

	for (int ix = -1; ix < (int)((winW / bW) + 2); ix++)
	{
		for (int iy = -1; iy < (int)((winH / bH) + 2); iy++)
		{
			StretchBlt(windowDC, (int)(bX + (ix*bW)), (int)((bY + (iy*bH)) + scrollY), (int)bW, (int)bH, bitDC, 0, 0, (int)bW, (int)bH, SRCCOPY);
		}
	}

	if (scrollY < bH)
	{
		scrollY += _scrollSpeed *_fDeltaTick;
	}
	else
	{
		scrollY = 0;
	}

	SelectObject(bitDC, oldBM);
	DeleteDC(bitDC);
	ReleaseDC(hwnd, windowDC);
}

void CBackGround::Process(float _fDeltaTick)
{
	CEntity::Process(_fDeltaTick);
}
