#pragma once
#include <iostream>
#include <Windows.h>
#include <ctime>
#include <cstdlib>

class Clock
{
public:
	Clock();
	~Clock();

	bool Initialise();

	void Process();

	double GetDeltaTick();
	int GetMsPassed() const;

protected:
	double timeElapsed;
	double deltaTime;
	double lastTime;
	double currentTime;
	double secondsPerCount;
	int frameCount = 0;

	clock_t startTime;
	double msPassed;

};
