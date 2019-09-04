// Local Includes
#include "clock.h"

Clock::Clock()
	: timeElapsed(0.0)
	, deltaTime(0.0)
	, lastTime(0.0)
	, currentTime(0.0)
{

}

Clock::~Clock()
{

}

bool Clock::Initialise()
{
	__int64 _TimerFrequency, _currTime;
	QueryPerformanceFrequency((LARGE_INTEGER*)&_TimerFrequency);
	secondsPerCount = 1.0 / static_cast<double>(_TimerFrequency);

	QueryPerformanceCounter((LARGE_INTEGER*)&_currTime);
	currentTime = static_cast<double>(_currTime);
	lastTime = static_cast<double>(_currTime);

	//
	startTime = clock();

	return (true);
}

void Clock::Process()
{
	//Get the time this frame.
	__int64 currTime;

	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	currentTime = static_cast<double>(currTime);

	//Time difference between this frame and the previous frame
	deltaTime = (currentTime - lastTime)*secondsPerCount;

	//Prepare for the next frame
	lastTime = currentTime;

	//Force non-negative
	if (deltaTime < 0.00000f)
	{
		deltaTime = 0.00000f;
	}

	timeElapsed += deltaTime;

	//Milliseconds passed
	msPassed = (clock() - startTime);
}

double Clock::GetDeltaTick()
{
	return (deltaTime);
}

int Clock::GetMsPassed() const
{
	return ((int)msPassed);
}
