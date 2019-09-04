
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
#include "Sprite.h"
#include "utils.h"

// This Include
#include "entity.h"

// Static Variables

// Static Function Prototypes

// Implementation

CEntity::CEntity()
: m_fX(0.0f)
, m_fY(0.0f)
{
}

CEntity::~CEntity()
{
    delete m_pSprite;
    m_pSprite = 0;
}

bool CEntity::Initialise(const int _kiSpriteID, const int _kiMaskID)
{
    m_pSprite = new CSprite();
    VALIDATE(m_pSprite->Initialise(_kiSpriteID, _kiMaskID));

    return (true);
}

void CEntity::Draw()
{
    m_pSprite->Draw();
}

void CEntity::Process(float _fDeltaTick)
{
    m_pSprite->SetX(static_cast<int>(m_fX));
    m_pSprite->SetY(static_cast<int>(m_fY));

    m_pSprite->Process(_fDeltaTick);
}

float CEntity::GetX() const
{
    return (m_fX);
}

float  CEntity::GetY() const
{
    return (m_fY);
}

float CEntity::GetWidth() const
{
    return (static_cast<float>(m_pSprite->GetWidth()));
}

float CEntity::GetHeight() const
{
    return (static_cast<float>(m_pSprite->GetHeight()));
}

void CEntity::SetX(float _f)
{
    m_fX = _f;
}

void CEntity::SetY(float _f)
{
    m_fY = _f;
}

void CEntity::SetGameArea(float _gameWidth, float _gameHeight)
{
	gameWidth = _gameWidth;
	gameHeight = _gameHeight;
}

void CEntity::InitiateAnimation(int _HFrames, int _VFrames, int _HCurrent, int _VCurrent, float _animSpeed)
{
	m_pSprite->SetHFrames(_HFrames);
	m_pSprite->SetVFrames(_VFrames);
	m_pSprite->SetCurrentFrame(_HCurrent,_VCurrent);
	m_pSprite->SetAnimSpeed(_animSpeed);
}

void CEntity::SetCurrentFrame(int _H, int _V)
{
	m_pSprite->SetCurrentFrame( _H, _V);
}