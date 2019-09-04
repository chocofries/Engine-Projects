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

// This Include
#include "cargo.h"

// Static Variables

// Static Function Prototypes

// Implementation

Cargo::Cargo()
{

}

Cargo::~Cargo()
{

}

bool Cargo::Initialise()
{
	VALIDATE(CEntity::Initialise(IDB_CARGO, IDB_CARGOMASK));
	InitiateAnimation(6,1,0,0,0.0f);
	isAlive = true;
	health = 6;
	vecBullets = nullptr;

	return (true);
}

void Cargo::Draw()
{
	if (isAlive)
	{
		CEntity::Draw();
	}
}

void Cargo::Process(float _fDeltaTick)
{
	if (isAlive)
	{
		if (health > 0)
		{
			CEntity::Process(_fDeltaTick);
			CheckBulletCollision();
			SetCurrentFrame(6-health, 0);
		}
		else
		{
			isAlive = false;
		}
	}
}

bool Cargo::GetIsAlive() const
{
	return isAlive;
}

void Cargo::SetBulletVector(std::vector<Bullet*>* _vecBullets)
{
	vecBullets = _vecBullets;
}

void Cargo::CheckBulletCollision()
{
	Bullet* tempBullet;

	if (isAlive)
	{
		if (vecBullets != nullptr)
		{
			if (!vecBullets->empty())
			{
				for (Bullet* it : *vecBullets) //Iterate through vector
				{
					if (it->GetIsActive() == true && it->GetIsPlayerBullet() == false) //Check if a bullet is active
					{
						tempBullet = it;

						float bulletR = tempBullet->GetRadius()*0.1f;
						float bulletX = tempBullet->GetX();
						float bulletY = tempBullet->GetY();

						float cargoX = m_fX;
						float cargoY = m_fY;

						float cargoH = 40.0f;
						float cargoW = 22.0f;

						if ((bulletX + bulletR > cargoX - cargoW / 2) &&
							(bulletX - bulletR < cargoX + cargoW / 2) &&
							(bulletY + bulletR > cargoY - cargoH / 2) &&
							(bulletY - bulletR < cargoY + cargoH / 2))
						{
							health -= 1; //-1 to health
							tempBullet->SetIsActive(false);
						}
					}
				}
			}
		}
	}
}