
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

#if !defined(__GAME_H__)
#define __GAME_H__

// Library Includes
#include <windows.h>
#include <string>

// Local Includes
#include "clock.h"

// Types

// Constants

// Prototypes
class CLevel;
class CBackBuffer;

class CGame
{
	// Member Functions
public:
	virtual ~CGame();

	virtual bool Initialise(HINSTANCE _hInstance, HWND _hWnd, int _iWidth, int _iHeight);

	virtual void Draw(float _fDeltaTick);
	virtual void Process(float _fDeltaTick);

	void ExecuteOneFrame();

	CBackBuffer* GetBackBuffer();
	CLevel* GetLevel();
	HINSTANCE GetAppInstance();
	HWND GetWindow();

	void GameRestart();
	void GameOverWon();
	void GameOverLost();

	// Singleton Methods
	static CGame& GetInstance();
	static void DestroyInstance();
	void SetSpeeds(int _plMove, int _plShoot, int _piMove, int _piShoot);
	void SetScore(int _score, bool _isAdding);
	void SetMultiplier(int _multi, bool _isAdding);
	void DrawScore();
	void DrawHighScore();
	void SaveHighScore();
	void CheckHighScore();
	void ReadHighScore();
	void SetCurrentName(std::string _name);

	int GetMultiplier() const;

	void SetOpenName(bool _open);
	bool GetOpenName() const;

protected:

private:
	CGame();
	CGame(const CGame& _kr);
	CGame& operator= (const CGame& _kr);

	// Member Variables
public:

protected:
	CClock* m_pClock;
	CLevel* m_pLevel;

	CBackBuffer* m_pBackBuffer;

	//Application data
	HINSTANCE m_hApplicationInstance;
	HWND m_hMainWindow;

	// Singleton Instance
	static CGame* s_pGame;

private:
	int width;
	int height;
	int currentScore;
	int scoreMultiplier; //The multiplier to the score, is incremented every time the player wins a game
	bool letsRestart;

	int scorePlace; //The place the current score places
	int firstScore; //The highest score
	int secondScore; //The second highest score
	int thirdScore; //The third highest score
	std::string firstName;
	std::string secondName;
	std::string thirdName;
	std::string currentName;

	int playerMoveSpeed; // 1-3
	int playerShootSpeed; // 1-3
	int pirateMoveSpeed; // 1-3
	int pirateShootSpeed; // 1-3

	bool openName; //Whether to open the name dialog
};

#endif    // __GAME_H__

