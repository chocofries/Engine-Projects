
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

#if !defined(__SPRITE_H__)
#define __SPRITE_H__

// Library Includes
#include "windows.h"

// Local Includes

// Types

// Constants

// Prototypes
class CSprite
{
    // Member Functions
public:
    CSprite();
    ~CSprite();

    bool Initialise(int _iResourceID, int _iMaskResourceID);

    void Draw();
    void Process(float _fDeltaTick);

    int GetWidth() const;
    int GetHeight() const;

    int GetX() const;
    int GetY() const;
	int GetFrameWidth() const;
	int GetFrameHeight() const;
    void SetX(int _i);
    void SetY(int _i);

	void SetHFrames(int _hNum);
	void SetVFrames(int _vNum);
	void SetAnimSpeed(float _animSpeed);
	void SetCurrentFrame(int _H, int _V);

    void TranslateRelative(int _iX, int _iY);
    void TranslateAbsolute(int _iX, int _iY);

protected:


private:
    CSprite(const CSprite& _kr);
    CSprite& operator= (const CSprite& _kr);

    // Member Variables
public:

protected:
    //Center handle
    int m_iX;
    int m_iY;

    HBITMAP m_hSprite;
    HBITMAP m_hMask;

    BITMAP m_bitmapSprite;
    BITMAP m_bitmapMask;

    static HDC s_hSharedSpriteDC;
    static int s_iRefCount;

private:
	int frameWidth;		//Width of a frame in a sprite-sheet
	int frameHeight;	//Height of a frame in a sprite-sheet
	int HFramesNum;		//Number of frames in a sprite-sheet horizontally
	int VFramesNum;		//Number of framse in a sprite-sheet vertically
	int HCurrent;		//Current frame's horizontal position
	int VCurrent;		//Current frame's vertical position
	int totalFrames;	//Total number of frames
	float animSpeed;	//The speed of the animation, 0 is not animated
	float timeElapsed;
};

#endif    // __SPRITE_H__
