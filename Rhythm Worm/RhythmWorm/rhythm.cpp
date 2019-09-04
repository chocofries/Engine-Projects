
#include "rhythm.h"


Rhythm::Rhythm(Clock* _clock, InputManager* _input, AudioManager* _audio)
{
	clock = _clock;
	inputManager = _input;
	audioManager = _audio;
	elapsedTime = 0.0f;
	bpm = 120.0000f;
	bpmTick = (double)(60.000f / bpm);
	tickMS = (int)(bpmTick * 1000);
	beatWindow = 0.4f;
	isOnBeat = false;
	isInWindow = false;
	isOnBeatBegin = false;
	isWriting = false;
	beatWritten = false;
	lastPos = 0;

	ReadBeatMap();
}

Rhythm::~Rhythm()
{
	WriteBeatMap(); //Writes beatmap to a file
}

void Rhythm::Update()
{
	deltaTime = clock->GetDeltaTick();
	elapsedTime += deltaTime;

	if (isActive)
	{
		ProcessInput();


		if (audioManager->GetBGMPosition() >= (unsigned int)(lastPos + tickMS))
		{
			int result = audioManager->GetBGMPosition() + (int)(tickMS*0.5f); //Quantizing
			result -= result % tickMS;
			lastPos = result;
			windowStart = lastPos + (int)(tickMS*0.3f);
			windowEnd = lastPos + (int)(tickMS*0.6f);
			isOnBeat = true;
		}
		else
		{
			isOnBeat = false;
		}
		
		if (audioManager->GetBGMPosition() >= (unsigned int)windowStart)
		{
			if (audioManager->GetBGMPosition() >= (unsigned int)windowEnd)
			{
				isInWindow = true;
			}
			else
			{
				isInWindow = false;
			}
		}
	}

}

void Rhythm::ProcessInput()
{
	//if (inputManager->GetKeyState('s') == KEY_DOWN)
	//{
	//	//Insert a beat into the map with the current song position
	//	int result = audioManager->GetBGMPosition() + (int)(tickMS*0.5f); //Quantizing
	//	result -= result % tickMS;
	//	int pos = result;
	//	int beat = 1;
	//	beatMap.insert({ pos, beat});
	//}
}

void Rhythm::FindNextBeatPosition()
{
	//Find the next beat in map
	if (!positionQueue.empty())
	{
		positionQueue.pop();
		beatQueue.pop();

		if (!positionQueue.empty())
		{
			nextPos = positionQueue.front();
			nextBeat = beatQueue.front();
		}
		else
		{
			nextPos = 1000000;
			nextBeat = 0;
		}
	}
}

float Rhythm::GetBPM() const
{
	return bpm;
}

double Rhythm::GetBPMTick() const
{
	return bpmTick;
}

int Rhythm::GetTickMS() const
{
	return tickMS;
}

float Rhythm::GetBeatWindow() const
{
	return (float)(beatWindow);
}

bool Rhythm::GetIsOnBeat() const
{
	return isOnBeat;
}

bool Rhythm::GetIsInWindow() const
{
	return isInWindow;
}

bool Rhythm::GetIsOnBeatBegin() const
{
	return isOnBeatBegin;
}


int Rhythm::GetNextPosition() const
{
	return nextPos;
}

void Rhythm::SetBPM(float _bpm)
{
	bpm = _bpm;
}

void Rhythm::SetBeatWindow(float _secs)
{
	beatWindow = _secs;
}

void Rhythm::SetIsActive(bool _isActive)
{
	isActive = _isActive;
}

void Rhythm::ReadBeatMap()
{
	std::string strLine;
	std::map<int, int>::iterator it;
	std::string strPos;
	std::string strBeat;
	bool isPosition = true;

	beatMap.clear();
	beatRead.open("Assets/BeatMapA.txt");

	if (beatRead.is_open())
	{
		while (getline(beatRead,strLine)) //Going through the file line by line
		{
			//Reset values
			strPos = "";
			strBeat = "";
			isPosition = true;

			//Looping through characters in the line
			for (std::string::iterator it = strLine.begin(); it != strLine.end(); ++it) 
			{
				if (*it == ':')
				{
					isPosition = false;
				}

				if (*it != ':')
				{
					if (isPosition == true)
					{
						strPos += *it; //Position in Song (in milliseconds)
					}
					else
					{
						strBeat += *it; //Type of Beat
					}
				}
			}

			//Convert strings to integers
			int pos = atoi(strPos.c_str());
			int beat = atoi(strBeat.c_str());

			//Add values to map & queues
			beatMap.insert({pos,beat});
			positionQueue.push(pos);
			beatQueue.push(beat);
		}
		beatRead.close();
	}
}

void Rhythm::WriteBeatMap()
{
	beatWrite.open("Assets/BeatMapA.txt");

	std::map<int, int>::iterator it;

	for (it = beatMap.begin(); it != beatMap.end(); it++)
	{
		int pos = it->first;
		int beat = it->second;
		beatWrite << pos << ":" << beat << std::endl;
	}

	beatWrite.close();
}

void Rhythm::Reset()
{
	lastPos = 0;
	beatMap.clear();
	while (!beatQueue.empty())
	{
		beatQueue.pop();
	}
	while (!positionQueue.empty())
	{
		positionQueue.pop();
	}
	ReadBeatMap();
	nextPos = positionQueue.front();
}
