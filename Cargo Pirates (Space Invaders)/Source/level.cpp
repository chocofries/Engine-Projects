
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
#include <Windows.h>
#include <mmsystem.h>
#include <vector>
// Local Includes
#include "resource.h"
#include "Game.h"
#include "playership.h"
//#include "Brick.h"
//#include "Ball.h"
#include "cargoship.h"
#include "enemysmall.h"
#include "enemyboss.h"
#include "bullet.h"
#include "utils.h"
#include "backbuffer.h"
#include "framecounter.h"
#include "background.h"
#include "menu.h"
#include "winlose.h"
#include "Credits.h"

// This Include
#include "Level.h"

// Static Variables

// Static Function Prototypes

// Implementation

#define CHEAT_BOUNCE_ON_BACK_WALL

CLevel::CLevel()
: m_iBricksRemaining(0)
, playerShip(0)
, enemyBoss(0)
, m_pBall(0)
, m_iWidth(0)
, m_iHeight(0)
, m_fpsCounter(0)
, enemyAmount(63)
, isPaused(true)
, gameOver(false)
, gameWon(false)
, playerMoveSpeed(2)
, playerShootSpeed(2)
, pirateMoveSpeed(2)
, pirateShootSpeed(2)
{
}

CLevel::~CLevel()
{
    while (vecEnemySmall.size() > 0) //Destroy Enemies
    {
        EnemySmall* pEnemySmall = vecEnemySmall[vecEnemySmall.size() - 1];
        vecEnemySmall.pop_back();
        delete pEnemySmall;
    }

	while (vecBullets.size() > 0) //Destroy Bullets
	{
		Bullet* tempBullet = vecBullets[vecBullets.size() - 1];
		vecBullets.pop_back();
		delete tempBullet;
	}

	while (vecCargoShips.size() > 0) //Destroy CargoShips
	{
		CargoShip* tempCargoShip = vecCargoShips[vecCargoShips.size() - 1];
		vecCargoShips.pop_back();
		delete tempCargoShip;
	}

	while (vecButtons.size() > 0) //Destroy Buttons
	{
		Button* tempButton = vecButtons[vecButtons.size() - 1];
		vecButtons.pop_back();
		delete tempButton;
	}

	delete enemyBoss;
	enemyBoss = 0;

    delete playerShip; //Delete Player
    playerShip = 0;

    m_pBall = 0;

	delete m_fpsCounter;
	m_fpsCounter = 0;

	delete m_pBackground;
	m_pBackground = 0;

	delete m_pMenu;
	m_pMenu = 0;

	delete winLoseScreen;
	winLoseScreen = 0;
}

bool CLevel::Initialise(int _iWidth, int _iHeight)
{
    m_iWidth = _iWidth;
    m_iHeight = _iHeight;
	bossActivated = false;

    const float fBallVelX = 200.0f;
    const float fBallVelY = 75.0f;

	for (int i = 0; i < 2; ++i) //Create Buttons
	{
		Button* pButton = new Button();
		VALIDATE(pButton->Initialise());
		pButton->SetX((float)m_iWidth / 2);
		pButton->SetY((float)m_iHeight*0.65 + (120 * i));
		pButton->SetIndex(i + 1);
		vecButtons.push_back(pButton);
	}

	//Menu Options and initialisation
	m_pMenu = new Menu();
	VALIDATE(m_pMenu->Initialise());
	m_pMenu->SetX((float)m_iWidth / 2);
	m_pMenu->SetY((float)m_iHeight*0.3);

	//Create Win/Lose Screen
	winLoseScreen = new WinLose();
	VALIDATE(winLoseScreen->Initialise());
	winLoseScreen->SetX((float)m_iWidth / 2);
	winLoseScreen->SetY((float)m_iHeight / 2);

	//Play BGM
	CGame& game = CGame::GetInstance();
	HINSTANCE gameInst = game.GetAppInstance();
	PlaySound(MAKEINTRESOURCE(IDR_BGM), gameInst, SND_RESOURCE | SND_ASYNC | SND_LOOP);

	m_pBackground = new CBackGround();
	VALIDATE(m_pBackground->Initialise());
	//Set the background position to start from {0,0}
	m_pBackground->SetX((float)m_iWidth / 2);
	m_pBackground->SetY((float)m_iHeight / 2);

	//m_pBall = new CBall();
    //VALIDATE(m_pBall->Initialise(m_iWidth / 2.0f, m_iHeight / 2.0f, fBallVelX, fBallVelY));

    playerShip = new PlayerShip();
    VALIDATE(playerShip->Initialise());

    // Set the player's position to be centered on the x, 
    // and a little bit up from the bottom of the window.
	//playerShip->InitiateAnimation(8,8,0,1.0f);
    playerShip->SetX(_iWidth / 2.0f);
    playerShip->SetY(_iHeight - ( 1.5f * playerShip->GetHeight()));
	playerShip->SetBulletVector(&vecBullets);
	playerShip->SetGameArea((float)m_iWidth,(float)m_iHeight);

    const int enemyNum = enemyAmount;
    const int kiStartX = 40;
    const int kiGap = 5;

    int iCurrentX = kiStartX;
    int iCurrentY = 80;

    for (int i = 0; i < enemyNum; ++i) //Create Small Enemies and put in Small Enemies vector
    {
        EnemySmall* pEnemySmall = new EnemySmall();
        VALIDATE(pEnemySmall->Initialise());

		pEnemySmall->SetBulletVector(&vecBullets);
		pEnemySmall->SetEnemySmallVector(&vecEnemySmall);
		pEnemySmall->SetGameArea((float)m_iWidth, (float)m_iHeight);
        pEnemySmall->SetX(static_cast<float>(iCurrentX));
        pEnemySmall->SetY(static_cast<float>(iCurrentY));

        iCurrentX += static_cast<int>(pEnemySmall->GetWidth()) + kiGap;

        if (iCurrentX > _iWidth*0.6f)
        {
            iCurrentX = kiStartX;
            iCurrentY += static_cast<int>(pEnemySmall->GetHeight()) + 5;;
        }

        vecEnemySmall.push_back(pEnemySmall);
    }

	for (int i = 0; i < 50; ++i) //Create Bullets and put into bullet vector
	{
		Bullet* pBullet = new Bullet();
		pBullet->SetGameArea((float)m_iWidth, (float)m_iHeight);
		VALIDATE(pBullet->Initialise(100,100,0,0));

		vecBullets.push_back(pBullet);
	}

	//Create Cargo Ships
	for (int i = 0; i < 4; i++)
	{
		CargoShip* pCargoShip = new CargoShip();
		VALIDATE(pCargoShip->Initialise());
		float shipGap = 300;

		if (i < 2)
		{
			pCargoShip->SetX((float)(m_iWidth / 2) - (shipGap/2) - (shipGap*i));
		}
		else
		{
			pCargoShip->SetX((float)(m_iWidth / 2) + (shipGap/2) + (shipGap*(i-2)));
		}

		pCargoShip->SetY(640.0f);
		pCargoShip->SetBulletVector(&vecBullets);
		pCargoShip->InitiateCargo();
		vecCargoShips.push_back(pCargoShip);
	}

	//Create Enemy Boss
	enemyBoss = new EnemyBoss();
	VALIDATE(enemyBoss->Initialise());
	enemyBoss->SetX(_iWidth + 100.0f);
	enemyBoss->SetY(100.0f);
	enemyBoss->SetBulletVector(&vecBullets);
	enemyBoss->SetGameArea((float)m_iWidth, (float)m_iHeight);

    SetBricksRemaining(enemyNum);
	m_fpsCounter = new CFPSCounter();
	VALIDATE(m_fpsCounter->Initialise());

    return (true);
}

void CLevel::Draw(float _fDeltaTick)
{
	m_pBackground->DrawTiled(100.0f, _fDeltaTick);

	if (!gameOver)
	{
		if (!isPaused)
		{
			for (unsigned int i = 0; i < vecEnemySmall.size(); ++i) //Draw Small Enemies
			{
				vecEnemySmall[i]->Draw();
			}

			for (unsigned int i = 0; i < vecCargoShips.size(); ++i) //Draw CargoShips
			{
				vecCargoShips[i]->Draw();
			}

			for (unsigned int i = 0; i < vecBullets.size(); ++i) //Draw Bullets
			{
				vecBullets[i]->Draw();
			}

			enemyBoss->Draw();
			playerShip->Draw();
		}
		else
		{
			//Draw Menu (Title Image)
			m_pMenu->Draw();
			CGame& game = CGame::GetInstance();
			game.DrawHighScore();

			for (unsigned int b = 0; b < vecButtons.size(); ++b)//Draw buttons
			{
				vecButtons[b]->Draw();
			}
		}
	}
	else
	{
		//Draw WinLose
		winLoseScreen->Draw();
	}

    //DrawScore();
	//DrawFPS();

	//Draw Debug

	/*CBackBuffer* pBackBuffer = CGame::GetInstance().GetBackBuffer();
	TCHAR text[256];
	swprintf_s(text, 256, L"Player Move Speed: %d", playerMoveSpeed);
	TextOut(pBackBuffer->GetBFDC(), 32, 32, text, wcslen(text));
	swprintf_s(text, 256, L"Player Shoot Speed: %d", playerShootSpeed);
	TextOut(pBackBuffer->GetBFDC(), 32, 52, text, wcslen(text));
	swprintf_s(text, 256, L"Pirate Move Speed: %d", pirateMoveSpeed);
	TextOut(pBackBuffer->GetBFDC(), 32, 72, text, wcslen(text));
	swprintf_s(text, 256, L"Pirate Shoot Speed: %d", pirateShootSpeed);
	TextOut(pBackBuffer->GetBFDC(), 32, 92, text, wcslen(text));*/
}

void CLevel::Process(float _fDeltaTick)
{
	if (!gameOver)
	{
		if (!isPaused)
		{
			m_pBackground->Process(_fDeltaTick);

			enemyBoss->SetSpeeds(pirateMoveSpeed, pirateShootSpeed);
			enemyBoss->Process(_fDeltaTick);
			playerShip->SetSpeeds(playerMoveSpeed, playerShootSpeed);
			playerShip->Process(_fDeltaTick);
			//pCredits->Process(_fDeltaTick);

			for (unsigned int i = 0; i < vecEnemySmall.size(); ++i) //Process Small Enemies
			{
				vecEnemySmall[i]->SetSpeeds(pirateMoveSpeed, pirateShootSpeed);
				vecEnemySmall[i]->Process(_fDeltaTick);
				//Lose Condition
				if (vecEnemySmall[i]->GetY() >= m_iHeight*0.575)
				{
					gameOver = true;
					gameWon = false;
					//CGame::GetInstance().GameOverLost();
					break;
				}

			}

			for (unsigned int i = 0; i < vecBullets.size(); ++i) //Process Bullets
			{
				vecBullets[i]->Process(_fDeltaTick);
			}

			for (unsigned int i = 0; i < vecCargoShips.size(); ++i) //Process CargoShips
			{
				vecCargoShips[i]->Process(_fDeltaTick);
			}

			//SetBricksRemaining(GetAliveEnemies());

			if (bossActivated == false)
			{
				if (GetAliveEnemies() < (enemyAmount / 2))
				{
					enemyBoss->Activate();
					//Play Boss Fight BGM
					CGame& game = CGame::GetInstance();
					HINSTANCE gameInst = game.GetAppInstance();
					PlaySound(MAKEINTRESOURCE(IDR_BOSSBGM), gameInst, SND_RESOURCE | SND_ASYNC | SND_LOOP);
					bossActivated = true;
				}
			}

			//Win Condition
			if (GetAliveEnemies() == 0)
			{
				if (enemyBoss->IsAlive() == false)
				{
					gameOver = true;
					gameWon = true;
				}
			}

			//Player Death Lose Condition
			if (playerShip->GetAlive() == false)
			{
				gameOver = true;
				gameWon = false;
				//CGame::GetInstance().GameOverLost();
			}
		}
		else //If paused
		{
			//Menu (Title Image)
			m_pMenu->Process(_fDeltaTick);

			//Process Menu Buttons

			static int cButton = 1;
			static bool buttonPressed = false;
			static float tElapsed = 0;

			if (buttonPressed == true)
			{
				tElapsed += _fDeltaTick;
				if (tElapsed >= 0.25f)
				{
					buttonPressed = false;
					tElapsed = 0;
				}
			}

			if (buttonPressed == false)
			{
				if (GetAsyncKeyState(VK_UP) & 0x8000)
				{
					if (cButton > 1)
					{
						cButton -= 1;
					}
					else if (cButton <= 1)
					{
						cButton = 2;
					}
					buttonPressed = true;
				}
				if (GetAsyncKeyState(VK_DOWN) & 0x8000)
				{
					if (cButton < 2)
					{
						cButton += 1;
					}
					else if (cButton >= 2)
					{
						cButton = 1;
					}
					buttonPressed = true;
				}
				if ((GetAsyncKeyState(VK_RETURN) & 0x8000))
				{
					if (cButton == 1)
					{
						//set pause false
						SetPause(false);
					}
					if (cButton == 2)
					{
						//Post Quit Message
						PostQuitMessage(0);
					}
					buttonPressed = true;
				}
			}

			for (unsigned int b = 0; b < vecButtons.size(); ++b)//Process buttons
			{
				vecButtons[b]->SetCurrentButton(cButton);
				vecButtons[b]->Process(_fDeltaTick);
			}
		}
	}
	else //If Game Over
	{
		winLoseScreen->Process(_fDeltaTick);

		//WinLose
		if (gameWon == false)
		{
			winLoseScreen->SetCurrentFrame(1, 0);

			CGame& game = CGame::GetInstance();

			if ((GetAsyncKeyState(VK_ESCAPE) & 0x8000))
			{
				CGame::GetInstance().GameOverLost();
				game.SetMultiplier(1, false); //Reset score multiplier when you lose
			}
		}
		else
		{
			winLoseScreen->SetCurrentFrame(0, 0);

			CGame& game = CGame::GetInstance();			

			if ((GetAsyncKeyState(VK_ESCAPE) & 0x8000))
			{
				game.GameOverWon();
				game.SetMultiplier(1, true); //Add to the score multiplier when you win
			}
		}
	}

	m_fpsCounter->CountFramesPerSecond(_fDeltaTick);
}

int CLevel::GetAliveEnemies() const
{
	int aliveNum = 0;

	for (unsigned int i = 0; i < vecEnemySmall.size(); ++i) //Process Small Enemies
	{
		if (!vecEnemySmall[i]->IsHit())
		{
			aliveNum += 1;
		}
	}

	return aliveNum;
}

void CLevel::SetSpeeds(int _plMove, int _plShoot, int _piMove, int _piShoot)
{
	playerMoveSpeed = _plMove;
	playerShootSpeed = _plShoot;
	pirateMoveSpeed = _piMove;
	pirateShootSpeed = _piShoot;
}

PlayerShip* CLevel::GetPaddle() const
{
    return (playerShip);
}

int CLevel::GetBricksRemaining() const
{
    return (m_iBricksRemaining);
}

void CLevel::SetBricksRemaining(int _i)
{
    m_iBricksRemaining = _i;
    UpdateScoreText();
}

void CLevel::DrawScore()
{
    HDC hdc = CGame::GetInstance().GetBackBuffer()->GetBFDC();

    const int kiX = 40;
    const int kiY = m_iHeight - 80;
	SetBkMode(hdc, TRANSPARENT);
    
    TextOutA(hdc, kiX, kiY, m_strScore.c_str(), static_cast<int>(m_strScore.size()));
}

void CLevel::UpdateScoreText()
{
    m_strScore = "ENEMIES REMAINING: ";

    m_strScore += ToString(GetBricksRemaining());
}


void CLevel::DrawFPS()
{
	HDC hdc = CGame::GetInstance().GetBackBuffer()->GetBFDC(); 

	m_fpsCounter->DrawFPSText(hdc, m_iWidth, m_iHeight);

}

void CLevel::SetPause(bool _pause)
{
	isPaused = _pause;
}

bool CLevel::GetPause() const
{
	return isPaused;
}