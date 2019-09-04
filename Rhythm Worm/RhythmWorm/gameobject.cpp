#include "gameobject.h"

GameObject::GameObject()
	: x(0.0f)
	, y(0.0f)
	, z(0.0f)
	, xRot(0.0f)
	, yRot(0.0f)
	, zRot(0.0f)
	, xScale(1.0f)
	, yScale(1.0f)
	, zScale(1.0f)
	, rotationAngle(0.0f)
	, isLoaded(false)
{
}

GameObject::~GameObject()
{
}

void GameObject::Initialise()
{
}

void GameObject::Update(double dTime)
{
}

void GameObject::Draw()
{
}

void GameObject::SetCamera(Camera * _cam)
{
	gameCamera = _cam;
}

void GameObject::SetX(float _x)
{
	x = _x;
}

void GameObject::SetY(float _y)
{
	y = _y;
}

void GameObject::SetZ(float _z)
{
	z = _z;
}

void GameObject::SetXRot(float _xRot)
{
	xRot = _xRot;
}

void GameObject::SetYRot(float _yRot)
{
	yRot = _yRot;
}

void GameObject::SetZRot(float _zRot)
{
	zRot = _zRot;
}

void GameObject::SetXScale(float _xScale)
{
	xScale = _xScale;
}

void GameObject::SetYScale(float _yScale)
{
	yScale = _yScale;
}

void GameObject::SetZScale(float _zScale)
{
	zScale = _zScale;
}

void GameObject::SetRotationAngle(float _angle)
{
	rotationAngle = _angle;
}

void GameObject::SetImageFilePath(const char * _imagefilepath)
{
	imageFilePath = _imagefilepath;
}

void GameObject::SetShaderLoader(ShaderLoader * _sloader)
{
	shaderLoader = _sloader;
}

void GameObject::SetTextureLoader(TextureLoader * _tloader)
{
	textureLoader = _tloader;
}

void GameObject::SetInputManager(InputManager * _imanager)
{
	inputManager = _imanager;
}

void GameObject::SetVBO(GLuint* _vbo)
{
	VBO = _vbo;
	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
}

void GameObject::SetClock(Clock * _clock)
{
	clock = _clock;
}

void GameObject::SetVecTexts(std::vector<Texter*>* _vecTexts)
{
	vecTexts = _vecTexts;
}

void GameObject::SetIsActive(bool _isActive)
{
	isActive = _isActive;
}

float GameObject::GetX() const
{
	return x;
}

float GameObject::GetY() const
{
	return y;
}

float GameObject::GetZ() const
{
	return z;
}

float GameObject::GetXRot() const
{
	return xRot;
}

float GameObject::GetYRot() const
{
	return yRot;
}

float GameObject::GetZRot() const
{
	return zRot;
}

float GameObject::GetRotationAngle() const
{
	return rotationAngle;
}

float GameObject::GetXScale() const
{
	return xScale;
}

float GameObject::GetYScale() const
{
	return yScale;
}

float GameObject::GetZScale() const
{
	return zScale;
}

bool GameObject::GetIsActive() const
{
	return isActive;
}

ShaderLoader * GameObject::GetShaderLoader()
{
	return shaderLoader;
}

TextureLoader * GameObject::GetTextureLoader()
{
	return textureLoader;
}

GLuint * GameObject::GetVBO()
{
	return VBO;
}

Camera * GameObject::GetCamera()
{
	return gameCamera;
}

Clock * GameObject::GetClock()
{
	return clock;
}

int GameObject::GetScreenX() const
{
	return screenX;
}

int GameObject::GetScreenY() const
{
	return screenY;
}
