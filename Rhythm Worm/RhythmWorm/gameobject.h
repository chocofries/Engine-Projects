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
#include "textureloader.h"
#include "texter.h"
#include "inputmanager.h"
#include "clock.h"

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
	void SetRotationAngle(float _angle);

	void SetImageFilePath(const char* _imagefilepath);
	void SetShaderLoader(ShaderLoader* _sloader);
	void SetTextureLoader(TextureLoader* _tloader);
	void SetInputManager(InputManager* _imanager);
	void SetVBO(GLuint* _vbo);
	void SetClock(Clock* _clock);
	void SetVecTexts(std::vector<Texter*>* _vecTexts);
	void SetIsActive(bool _isActive);

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
	bool GetIsActive() const;

	ShaderLoader* GetShaderLoader();
	TextureLoader* GetTextureLoader();
	GLuint* GetVBO();
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

	bool isActive;

	//Position relative to screen (in pixels)
	int screenX;
	int screenY;

	const char* imageFilePath;
	unsigned char* image;
	int texW;
	int texH;
	bool isLoaded; //If the image has been loaded

	ShaderLoader* shaderLoader;
	TextureLoader* textureLoader;
	InputManager* inputManager;
	Clock* clock;
	Camera* gameCamera;
	GLuint* VBO;

	GLuint texture;
	double deltaTime;

	std::vector<Texter*>* vecTexts;
};