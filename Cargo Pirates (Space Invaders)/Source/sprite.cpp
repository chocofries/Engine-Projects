
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
#include "Game.h"
#include "BackBuffer.h"
#include "utils.h"

// This include
#include "Sprite.h"

// Static Variables
HDC CSprite::s_hSharedSpriteDC = 0;
int CSprite::s_iRefCount = 0;

// Static Function Prototypes

// Implementation

CSprite::CSprite()
: m_iX(0)
, m_iY(0)
{
    ++s_iRefCount;
}

CSprite::~CSprite()
{
    DeleteObject(m_hSprite);
    DeleteObject(m_hMask);

    --s_iRefCount;

    if (s_iRefCount == 0)
    {
        DeleteDC(s_hSharedSpriteDC);
        s_hSharedSpriteDC =  0;
    }
}

bool CSprite::Initialise(int _iSpriteResourceID, int _iMaskResourceID)
{
    HINSTANCE hInstance = CGame::GetInstance().GetAppInstance();

    if (!s_hSharedSpriteDC)
    {
        s_hSharedSpriteDC = CreateCompatibleDC(NULL);
    }

    m_hSprite = LoadBitmap(hInstance, MAKEINTRESOURCE(_iSpriteResourceID));
    VALIDATE(m_hSprite);
    m_hMask = LoadBitmap(hInstance, MAKEINTRESOURCE(_iMaskResourceID));
    VALIDATE(m_hMask);

    GetObject(m_hSprite, sizeof(BITMAP), &m_bitmapSprite);
    GetObject(m_hMask, sizeof(BITMAP), &m_bitmapMask);

	frameWidth = this->GetWidth();
	frameHeight = this->GetHeight();
	HFramesNum = 1;
	VFramesNum = 1;
	HCurrent = 0;
	VCurrent = 0;
	animSpeed = 0;
	timeElapsed = 0;
	totalFrames = 1;

    return (true);
}

void CSprite::Draw()
{
	int iW = GetWidth();
	int iH = GetHeight();

	int iX = m_iX - (frameWidth / 2);
	int iY = m_iY - (frameHeight / 2);

	int sX = frameWidth * HCurrent;
	int sY = 0;
	if (animSpeed == 0)
	{
		sY = frameHeight * VCurrent;
	}

    CBackBuffer* pBackBuffer = CGame::GetInstance().GetBackBuffer();

    HGDIOBJ hOldObj = SelectObject(s_hSharedSpriteDC, m_hMask);

    BitBlt(pBackBuffer->GetBFDC(), iX, iY, frameWidth, frameHeight, s_hSharedSpriteDC, sX, sY, SRCAND);

    SelectObject(s_hSharedSpriteDC, m_hSprite);

    BitBlt(pBackBuffer->GetBFDC(), iX, iY, frameWidth, frameHeight, s_hSharedSpriteDC, sX, sY, SRCPAINT);

    SelectObject(s_hSharedSpriteDC, hOldObj);
}

void CSprite::Process(float _fDeltaTick)
{
	timeElapsed += _fDeltaTick;

	if (timeElapsed >= animSpeed && animSpeed > 0.000000f)
	{
		if (HCurrent < HFramesNum - 1)
		{
			HCurrent += 1;
		}
		else
		{
			HCurrent = 0;
			if (VCurrent < VFramesNum - 1)
			{
				VCurrent += 1;
			}
			else
			{
				VCurrent = 0;
			}
		}
		timeElapsed = 0;
	}
}

int CSprite::GetWidth() const
{
    return (m_bitmapSprite.bmWidth);
}

int CSprite::GetHeight() const
{
    return (m_bitmapSprite.bmHeight);
}

int CSprite::GetX() const
{
    return (m_iX);
}

int CSprite::GetY() const
{
    return (m_iY);
}

int CSprite::GetFrameWidth() const
{
	return (frameWidth);
}

int CSprite::GetFrameHeight() const
{
	return (frameHeight);
}

void CSprite::SetX(int _i)
{
    m_iX = _i;
}

void CSprite::SetY(int _i)
{
    m_iY = _i;
}

void CSprite::TranslateRelative(int _iX, int _iY)
{
    m_iX += _iX;
    m_iY += _iY;
}

void CSprite::TranslateAbsolute(int _iX, int _iY)
{
    m_iX = _iX;
    m_iY = _iY;
}

void CSprite::SetHFrames(int _hNum)
{
	HFramesNum = _hNum;
	frameWidth = static_cast<int>(m_bitmapSprite.bmWidth / _hNum);

	totalFrames = HFramesNum * VFramesNum;
}

void CSprite::SetVFrames(int _vNum)
{
	VFramesNum = _vNum;
	frameHeight = static_cast<int>(m_bitmapSprite.bmHeight / _vNum);

	totalFrames = HFramesNum * VFramesNum;
}

void CSprite::SetAnimSpeed(float _animSpeed)
{
	animSpeed = _animSpeed;
}

void CSprite::SetCurrentFrame(int _H, int _V)
{
	HCurrent = _H;
	VCurrent = _V;
}