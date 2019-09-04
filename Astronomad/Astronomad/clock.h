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

	void SetClockStart(bool _start);
	double GetDeltaTick();
	int GetMsPassed() const;
	clock_t GetStartTime() const;
	void SetStartTime(clock_t time);

protected:
	bool clockStart;
	double timeElapsed;
	double deltaTime;
	double lastTime;
	double currentTime;
	double secondsPerCount;
	int frameCount = 0;

	clock_t startTime;
	double msPassed;

};
