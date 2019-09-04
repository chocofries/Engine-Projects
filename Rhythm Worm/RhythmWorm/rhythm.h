#pragma once
#include <iostream>
#include <fstream>
#include <queue>
#include "gameobject.h"
#include "audiomanager.h"
#include "clock.h"

class Rhythm
{
public:
	Rhythm(Clock* _clock, InputManager* _input, AudioManager* _audio);
	~Rhythm();

	void Update();
	void ProcessInput();
	void FindNextBeatPosition();

	float GetBPM() const;
	double GetBPMTick() const;
	int GetTickMS() const;
	float GetBeatWindow() const;
	bool GetIsOnBeat() const;
	bool GetIsInWindow() const;
	bool GetIsOnBeatBegin() const;

	int GetNextPosition() const;

	void SetBPM(float _bpm);
	void SetBeatWindow(float _secs);
	void SetIsActive(bool _isActive);
	void ReadBeatMap();
	void WriteBeatMap();

	void Reset();

private:
	Clock* clock;
	InputManager* inputManager;
	AudioManager* audioManager;
	double elapsedTime;
	double elapsedWindow;
	double deltaTime;
	float bpm;
	double bpmTick;
	double beatWindow;
	bool isOnBeat;
	bool isOnBeatBegin;
	bool isInWindow;
	bool isActive;
	bool isWriting;
	bool beatWritten;
	int nextPos;
	int nextBeat;
	int lastPos;
	int tickMS;
	int windowStart;
	int windowEnd;

	std::queue<int> positionQueue; //Queue of positions
	std::queue<int> beatQueue; //Queue of beats
	std::map<int, int> beatMap; //first value is the time, second value is type of beat
	std::ifstream beatRead;
	std::ofstream beatWrite;
};