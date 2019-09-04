
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

#if !defined(__LEVEL_H__)
#define __LEVEL_H__

// Library Includes
#include <vector>
#include <string>

// Local Includes

// Types

// Constants

// Prototypes
class CBall;
class PlayerShip;
class CargoShip;
class Cargo;
class Bullet;
class Menu;
class WinLose;
class Button;
class Credits;
class EnemySmall;
class EnemyBoss;
class CFPSCounter;
class CBackGround;

class CLevel
{
    // Member Functions
public:
    CLevel();
    virtual ~CLevel();

    virtual bool Initialise(int _iWidth, int _iHeight);

    virtual void Draw(float _fDeltaTick);
    virtual void Process(float _fDeltaTick);

    PlayerShip* GetPaddle() const;

    int GetBricksRemaining() const;

	void SetPause(bool _pause);
	bool GetPause() const;

	int GetAliveEnemies() const; //Returns the number of enemies that are still alive

	void SetSpeeds(int _plMove, int _plShoot, int _piMove, int _piShoot);

protected:
    void UpdateScoreText();
    void DrawScore();
	void DrawFPS();


    void SetBricksRemaining(int _i);

private:
    CLevel(const CLevel& _kr);
    CLevel& operator= (const CLevel& _kr);

    // Member Variables
public:

protected:
	CBackGround* m_pBackground;
    CBall* m_pBall;
    PlayerShip* playerShip;
	EnemyBoss* enemyBoss;
	Menu* m_pMenu;
	WinLose* winLoseScreen;
	Credits* pCredits;
	std::vector<EnemySmall*> vecEnemySmall;
	std::vector<Bullet*> vecBullets;
	std::vector<CargoShip*> vecCargoShips;
	std::vector<Button*> vecButtons;
	CFPSCounter* m_fpsCounter;

    int m_iWidth;
    int m_iHeight;
	int enemyAmount;

    int m_iBricksRemaining;

    std::string m_strScore;

private:
	bool isPaused;
	bool bossActivated;
	bool gameOver;
	bool gameWon;

	int playerMoveSpeed; // 1-3
	int playerShootSpeed; // 1-3
	int pirateMoveSpeed; // 1-3
	int pirateShootSpeed; // 1-3
};

#endif    // __LEVEL_H__
