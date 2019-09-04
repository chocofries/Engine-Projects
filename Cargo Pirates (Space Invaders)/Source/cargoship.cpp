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
#include "cargoship.h"

// Static Variables

// Static Function Prototypes

// Implementation

CargoShip::CargoShip()
{

}

CargoShip::~CargoShip()
{
	while (vecCargo.size() > 0) //Destroy Cargo
	{
		Cargo* tempCargo = vecCargo[vecCargo.size() - 1];
		vecCargo.pop_back();
		delete tempCargo;
	}
}

bool CargoShip::Initialise()
{
	VALIDATE(CEntity::Initialise(IDB_CARGOSHIP, IDB_CARGOSHIPMASK));
	vecBullets = nullptr;
	return (true);
}

void CargoShip::InitiateCargo()
{
	//Create Cargo Containers and put them into vector
	for (int column = 0; column < 4; column++)
	{
		for (int row = 0; row < 3; row++)
		{
			if (!(row == 0 && (column == 0 || column == 3)))
			{
				Cargo* pCargo = new Cargo();
				pCargo->Initialise();
				pCargo->SetBulletVector(vecBullets);
				pCargo->SetX(m_fX - 34.0f + (22.0f*column));
				pCargo->SetY(m_fY - 92.0f + (40.0f*row));
				vecCargo.push_back(pCargo);
			}
		}
	}
}

void CargoShip::SetBulletVector(std::vector<Bullet*>* _vecBullets)
{
	vecBullets = _vecBullets;
}

void CargoShip::Draw()
{
	CEntity::Draw();

	//Draw Cargo
	for (unsigned int i = 0; i < vecCargo.size(); ++i)
	{
		vecCargo[i]->Draw();
	}
}

void CargoShip::Process(float _fDeltaTick)
{
	CEntity::Process(_fDeltaTick);

	//Process Cargo
	for (unsigned int i = 0; i < vecCargo.size(); ++i)
	{
		vecCargo[i]->Process(_fDeltaTick);
	}
}

