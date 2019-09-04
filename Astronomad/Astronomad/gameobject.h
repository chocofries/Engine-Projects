#pragma once
#include <iostream>
#include <SOIL.h>
#include <vector>
#include <thread>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "shaderloader.h"
#include "camera.h"
#include "assetloader.h"
#include "texter.h"
#include "inputmanager.h"
#include "clock.h"

class AudioManager;

struct Path
{
	glm::vec2 start;
	glm::vec2 end;
	float radius;

	Path()
	{
		start = glm::vec2(0.0f, 0.0f);
		end = glm::vec2(1.0f, 0.0f);
		radius = 5.0f;
	}
};

class GameObject
{
public:
	GameObject();
	virtual ~GameObject();

	virtual void Initialise();
	virtual void Update(double dTime);
	virtual void Draw();

	void SetCamera(Camera* _cam);

	void SetX(float _x);
	void SetY(float _y);
	void SetZ(float _z);
	void SetXRot(float _xRot);
	void SetYRot(float _yRot);
	void SetZRot(float _zRot);
	void SetXScale(float _xScale);
	void SetYScale(float _yScale);
	void SetZScale(float _zScale);
	void SetUniformScale(float _scale);
	void SetRotationAngle(float _angle);

	void SetImageFilePath(const char* _imagefilepath);
	void SetShaderLoader(ShaderLoader* _sloader);
	void SetTextureLoader(AssetLoader* _tloader);
	void SetInputManager(InputManager* _imanager);
	void SetVBO(GLuint _vbo);
	void SetClock(Clock* _clock);
	void SetVecTexts(std::vector<Texter*>* _vecTexts);
	void SetIsActive(bool _isActive);

	void SetAvoidRadius(float _avoid);
	void SetVelocity(glm::vec2 _velocity);

	float GetX() const;
	float GetY() const;
	float GetZ() const;
	float GetXRot() const;
	float GetYRot() const;
	float GetZRot() const;
	float GetRotationAngle() const;
	float GetXScale() const;
	float GetYScale() const;
	float GetZScale() const;

	float GetAvoidRadius() const;
	float GetMaxSpeed() const;
	float GetDirection() const;
	glm::vec2 GetVelocity() const;

	bool GetIsActive() const;

	ShaderLoader* GetShaderLoader();
	AssetLoader* GetAssetLoader();
	GLuint GetVBO();
	GLuint GetSkyBoxTexture();
	Camera* GetCamera();
	Clock* GetClock();

	int GetScreenX() const;
	int GetScreenY() const;

protected:
	float x;
	float y;
	float z;
	float xRot;
	float yRot;
	float zRot;
	float xScale;
	float yScale;
	float zScale;
	float rotationAngle;

	//Steering variables
	float maxSpeed;
	float arrivalDist;
	float avoidRadius; //The radius of separation other boids should have to this one
	float direction;
	glm::vec2 velocity;

	bool isActive;

	//Bounds of the level
	float boundW;
	float boundH;

	//Position relative to screen (in pixels)
	int screenX;
	int screenY;

	const char* imageFilePath;
	unsigned char* image;
	int texW;
	int texH;
	bool isLoaded; //If the image has been loaded

	ShaderLoader* shaderLoader;
	AssetLoader* assetLoader;
	InputManager* inputManager;
	AudioManager* audioManager;
	Clock* clock;
	Camera* levelCamera;
	GLuint VBO;
	GLuint skyBoxTexture;

	GLuint texture;
	float deltaTime;

	std::vector<Texter*>* vecTexts;

protected:
	//Steering behaviours
	glm::vec2 Seek(float _tX, float _tY, float _maxSpeed, float _turnSpeed, bool _arrival, bool _flee);
	glm::vec2 Pursue(GameObject* _target, float _maxSpeed, float _turnSpeed, bool _arrival, bool _evade);
	glm::vec2 Wander(float _maxSpeed, float _turnSpeed, float _ahead, float _radius, float _rate);
	glm::vec2 Separation(float _maxSpeed, float _turnSpeed, float _radius, std::vector<GameObject*>* _vecOther);
	glm::vec2 Cohesion(float _maxSpeed, float _turnSpeed, float _radius, std::vector<GameObject*>* _vecOther);
	glm::vec2 Alignment(float _maxSpeed, float _turnSpeed, float _radius, std::vector<GameObject*>* _vecOther);
	glm::vec2 Orbit(float _maxSpeed, float _turnSpeed, float _orbitDist, float _orbitSpeed, std::vector<GameObject*>* _vecOther, GameObject* _leader);
	glm::vec2 Containment(float _maxSpeed, float _turnSpeed, float _buffer, float _boundW, float _boundH);
	glm::vec2 FollowPath(float _maxSpeed, float _turnSpeed, Path _path);
};