#pragma once
#include "sprite.h"
#include <fmod.hpp>

class Level;

class Particle : public GameObject
{
public:
	Particle(Level* _level);
	virtual ~Particle();

	virtual void Initialise();
	virtual void Update(double dTime);
	virtual void Draw();

	void Activate(glm::vec3 _pos, float _scale);

private:
	Level * level;
	Sprite * sprite;
	FMOD::Sound* sndBoom1;
	FMOD::Sound* sndBoom2;
	FMOD::Sound* sndBoom3;
};