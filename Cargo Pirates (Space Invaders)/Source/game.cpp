
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
#include <iostream>
#include <fstream>

// Local Includes
#include "Clock.h"
#include "Level.h"
#include "BackBuffer.h"
#include "utils.h"

// This Include
#include "Game.h"

// Static Variables
CGame* CGame::s_pGame = 0;

// Static Function Prototypes

// Implementation

CGame::CGame()
	: m_pLevel(0)
	, m_pClock(0)
	, m_hApplicationInstance(0)
	, m_hMainWindow(0)
	, m_pBackBuffer(0)
	, width(0)
	, height(0)
	, letsRestart(false)
	, playerMoveSpeed(2)
	, playerShootSpeed(2)
	, pirateMoveSpeed(2)
	, pirateShootSpeed(2)
	, currentScore(0)
	, scoreMultiplier(1)
	, openName(false)
	, scorePlace(0)
{

}

CGame::~CGame()
{
	delete m_pLevel;
	m_pLevel = 0;

	delete m_pBackBuffer;
	m_pBackBuffer = 0;

	delete m_pClock;
	m_pClock = 0;
}

bool CGame::Initialise(HINSTANCE _hInstance, HWND _hWnd, int _iWidth, int _iHeight)
{
	m_hApplicationInstance = _hInstance;
	m_hMainWindow = _hWnd;
	width = _iWidth;
	height = _iHeight;

	//firstName = "Tom Hanks";
	//firstScore = 5000;
	//secondName = "Pirate";
	//secondScore = 3000;
	//thirdName = "Your Mom";
	//thirdScore = 1000;
	currentName = "Mystery Man";
	ReadHighScore();

	m_pClock = new CClock();
	VALIDATE(m_pClock->Initialise());
	m_pClock->Process();

	m_pBackBuffer = new CBackBuffer();
	VALIDATE(m_pBackBuffer->Initialise(_hWnd, _iWidth, _iHeight));

	m_pLevel = new CLevel();
	VALIDATE(m_pLevel->Initialise(_iWidth, _iHeight));

	ShowCursor(false);

	return (true);
}

void CGame::Draw(float _fDeltaTick)
{
	m_pBackBuffer->Clear();

	m_pLevel->Draw(_fDeltaTick);
	DrawScore();
	//DrawHighScore();

	m_pBackBuffer->Present();
}

void CGame::Process(float _fDeltaTick)
{
	if (m_pLevel != nullptr)
	{
		m_pLevel->SetSpeeds(playerMoveSpeed,playerShootSpeed,pirateMoveSpeed,pirateShootSpeed);
		m_pLevel->Process(_fDeltaTick);

		/*if (GetAsyncKeyState(VK_F4) & 0x8000)
		{
			m_pLevel->SetPause(true);
		}
		if (GetAsyncKeyState(VK_F5) & 0x8000)
		{
			m_pLevel->SetPause(false);
		}*/

		/*if (GetAsyncKeyState(VK_F6) & 0x8000)
		{
			GameRestart();
		}
		else */
		
		if (letsRestart)
		{
			GameRestart();
		}
	}
}

void CGame::GameRestart()
{
	if (m_pLevel != nullptr)
	{
		CLevel* tempLevel = m_pLevel;
		m_pLevel = new CLevel();
		m_pLevel->Initialise(width, height);
		delete tempLevel;
	}
	CheckHighScore();
	letsRestart = false;
}

void CGame::ExecuteOneFrame()
{
	float fDT = m_pClock->GetDeltaTick();

	Process(fDT);
	Draw(fDT);

	m_pClock->Process();

	Sleep(1);
}

CGame& CGame::GetInstance()
{
	if (s_pGame == 0)
	{
		s_pGame = new CGame();
	}

	return (*s_pGame);
}

void CGame::GameOverWon()
{
	//MessageBox(m_hMainWindow, L"Captain Philips: It's gonna be okay.", L"You Won!", MB_OK);
	letsRestart = true;
}

void CGame::GameOverLost()
{
	//MessageBox(m_hMainWindow, L"Pirate: Look at me. I am the captain now.", L"You Lose!", MB_OK);
	letsRestart = true;
}

void CGame::DestroyInstance()
{
	delete s_pGame;
	s_pGame = 0;
}

void CGame::SetSpeeds(int _plMove, int _plShoot, int _piMove, int _piShoot)
{
	playerMoveSpeed = _plMove;
	playerShootSpeed = _plShoot;
	pirateMoveSpeed = _piMove;
	pirateShootSpeed = _piShoot;
}

void CGame::SetScore(int _score, bool _isAdding)
{
	if (!_isAdding)
	{
		currentScore = _score;
	}
	else
	{
		currentScore += _score;
	}
}

void CGame::SetMultiplier(int _multi, bool _isAdding)
{
	if (!_isAdding)
	{
		scoreMultiplier = _multi;
	}
	else
	{
		scoreMultiplier += _multi;
	}
}

void CGame::DrawScore()
{
	CBackBuffer* pBackBuffer = GetBackBuffer();
	TCHAR text[256];
	swprintf_s(text, 256, L"SCORE: %d", currentScore);
	TextOut(pBackBuffer->GetBFDC(), 32, 32, text, wcslen(text));
	swprintf_s(text, 256, L"MULTIPLIER: x%d", scoreMultiplier);
	TextOut(pBackBuffer->GetBFDC(), 32, 52, text, wcslen(text));
}

void CGame::DrawHighScore()
{
	CBackBuffer* pBackBuffer = GetBackBuffer();
	TCHAR text[256];

	std::wstring firstStr = std::wstring(firstName.begin(), firstName.end());
	const wchar_t* szName1 = firstStr.c_str();
	swprintf_s(text, 256, L"1. %s", szName1);
	TextOut(pBackBuffer->GetBFDC(), 1400, 32, text, wcslen(text));
	swprintf_s(text, 256, L"Score: %d", firstScore);
	TextOut(pBackBuffer->GetBFDC(), 1400, 52, text, wcslen(text));

	std::wstring secondStr = std::wstring(secondName.begin(), secondName.end());
	const wchar_t* szName2 = secondStr.c_str();
	swprintf_s(text, 256, L"2. %s", szName2);
	TextOut(pBackBuffer->GetBFDC(), 1400, 92, text, wcslen(text));
	swprintf_s(text, 256, L"Score: %d", secondScore);
	TextOut(pBackBuffer->GetBFDC(), 1400, 112, text, wcslen(text));

	std::wstring thirdStr = std::wstring(thirdName.begin(), thirdName.end());
	const wchar_t* szName3 = thirdStr.c_str();
	swprintf_s(text, 256, L"3. %s", szName3);
	TextOut(pBackBuffer->GetBFDC(), 1400, 152, text, wcslen(text));
	swprintf_s(text, 256, L"Score: %d", thirdScore);
	TextOut(pBackBuffer->GetBFDC(), 1400, 172, text, wcslen(text));
}

void CGame::SaveHighScore()
{
	//Add the name to the place

	switch (scorePlace)
	{
	case 1:
	{
		thirdName = secondName;
		thirdScore = secondScore;
		secondName = firstName;
		secondScore = firstScore;
		firstName = currentName;
		firstScore = currentScore;
		break;
	}
	case 2:
	{
		thirdName = secondName;
		thirdScore = secondScore;
		secondName = currentName;
		secondScore = currentScore;
		break;
	}
	case 3:
	{
		thirdName = currentName;
		thirdScore = currentScore;
		break;
	}
	default:
		break;
	}

	std::ofstream scoreFile;

	scoreFile.open("scores.txt");
	scoreFile << firstName << std::endl;
	scoreFile << firstScore << std::endl;
	scoreFile << secondName << std::endl;
	scoreFile << secondScore << std::endl;
	scoreFile << thirdName << std::endl;
	scoreFile << thirdScore << std::endl;
	scoreFile.close();

	currentScore = 0;
}

void CGame::CheckHighScore()
{
	scorePlace = 0;

	//Checks if the current score is high enough for the scores list
	if (currentScore > thirdScore)
	{
		if (currentScore > secondScore)
		{
			if (currentScore > firstScore)
			{
				scorePlace = 1;
			}
			else
			{
				scorePlace = 2;
			}
		}
		else
		{
			scorePlace = 3;
		}
	}
	else
	{
		return;
	}

	//Show the name dialog
	if (scorePlace != 0)
	{
		openName = true;
		return;
	}
}

void CGame::ReadHighScore()
{
	std::string strLine;
	std::ifstream scoreFile;

	scoreFile.open("scores.txt");
	if (scoreFile.is_open())
	{
		int i = 1;

		while (getline(scoreFile, strLine))
		{
			switch (i)
			{
			case 1:
			{
				firstName = strLine;
				break;
			}
			case 2:
			{
				firstScore = atoi(strLine.c_str());
				break;
			}
			case 3:
			{
				secondName = strLine;
				break;
			}
			case 4:
			{
				secondScore = atoi(strLine.c_str());
				break;
			}
			case 5:
			{
				thirdName = strLine;
				break;
			}
			case 6:
			{
				thirdScore = atoi(strLine.c_str());
				break;
			}
			default:
				break;
			}

			i += 1;
		}

		scoreFile.close();
	}

}

void CGame::SetCurrentName(std::string _name)
{
	currentName = _name;
}

int CGame::GetMultiplier() const
{
	return (scoreMultiplier);
}

void CGame::SetOpenName(bool _open)
{
	openName = _open;
}

bool CGame::GetOpenName() const
{
	return openName;
}

CBackBuffer* CGame::GetBackBuffer()
{
	return (m_pBackBuffer);
}

CLevel* CGame::GetLevel()
{
	return (m_pLevel);
}

HINSTANCE CGame::GetAppInstance()
{
	return (m_hApplicationInstance);
}

HWND CGame::GetWindow()
{
	return (m_hMainWindow);
}

