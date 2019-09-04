#include "particle.h"
#include "level.h"

Particle::Particle(Level* _level)
{
	level = _level;
	VBO = level->GetVBO();
	levelCamera = level->GetCamera();
	inputManager = level->GetInputManager();
	shaderLoader = level->GetShaderLoader();
	assetLoader = level->GetTextureLoader();
	audioManager = level->GetAudioManager();
	vecTexts = level->GetVecTexts();
	clock = level->GetClock();
	xRot = 0.0f;
	yRot = 1.0f;
	zRot = 0.0f;
	rotationAngle = 0.0f;
	isActive = false;
}

Particle::~Particle()
{
	delete sprite;
}

void Particle::Initialise()
{
	sprite = new Sprite(this);
	sprite->SetIsBillboard(true);
	sprite->InitSprite("Assets/Boom.png", "Boom", 0.005f, 64, 8, 8, 0, 1024, 1024);

	audioManager->GetAudioSystem()->createSound("Assets/explosion1.wav", FMOD_DEFAULT, 0, &sndBoom1);
	audioManager->AddSoundToVector(sndBoom1);
	audioManager->GetAudioSystem()->createSound("Assets/explosion2.wav", FMOD_DEFAULT, 0, &sndBoom2);
	audioManager->AddSoundToVector(sndBoom2);
	audioManager->GetAudioSystem()->createSound("Assets/explosion3.wav", FMOD_DEFAULT, 0, &sndBoom3);
	audioManager->AddSoundToVector(sndBoom3);
}

void Particle::Update(double dTime)
{
	deltaTime = (float)dTime;

	sprite->Update(deltaTime);
	if (sprite->GetCurrentFrame() >= sprite->GetFrameAmount()-1) //If reached the end of the animation
	{
		sprite->SetCurrentFrameX(0);
		sprite->SetCurrentFrameY(0);
		sprite->SetCurrentFrame(0);
		x = 10000.0f;
		z = 10000.0f;
		isActive = false; //Deactivate
	}
}

void Particle::Draw()
{
	sprite->Draw();
}

void Particle::Activate(glm::vec3 _pos, float _scale)
{
	x = _pos.x;
	z = _pos.z;
	sprite->SetSpriteScale((_scale * (float)(rand() % 10)*0.1f) + 0.1f);
	sprite->SetCurrentFrameX(0);
	sprite->SetCurrentFrameY(0);
	sprite->SetCurrentFrame(0);
	isActive = true;
	rotationAngle = (float)(rand()%360);

	//Play random explosion sound
	int chosen = rand() % 3;
	switch (chosen)
	{
	case 0:
		audioManager->GetAudioSystem()->playSound(sndBoom1, 0, false, 0);
		break;
	case 1:
		audioManager->GetAudioSystem()->playSound(sndBoom2, 0, false, 0);
		break;
	case 2:
		audioManager->GetAudioSystem()->playSound(sndBoom3, 0, false, 0);
		break;
	}

}
