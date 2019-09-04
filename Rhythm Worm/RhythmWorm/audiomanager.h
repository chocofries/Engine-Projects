#pragma once
#include <fmod.hpp>
#include <vector>

class AudioManager
{
public:
	AudioManager();
	~AudioManager();

	void Initialise();
	void Update();
	unsigned int GetBGMPosition() const;
	unsigned int GetBGMLength() const;

	float GetVolume() const;
	FMOD::System* GetAudioSystem();
	void AddSoundToVector(FMOD::Sound* _sound);

	void PlayBGM(bool _play);

private:
	FMOD::System* audioSystem;
	FMOD::Channel* channelA;
	FMOD::Channel* channelB;
	FMOD::Sound* bgm;
	FMOD::Sound* titleMusic;
	std::vector<FMOD::Sound*> vecSounds;

	unsigned int length;
	unsigned int position;
	float volume;
};