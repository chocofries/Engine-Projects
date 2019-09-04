#pragma once
#include "ShaderLoader.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "gameobject.h"
#include "camera.h"


class Sprite : public GameObject
{
public:
	Sprite(GameObject * _parent);
	virtual ~Sprite();

	virtual void Initialise();
	void InitSprite(const char* _imagepath, const char* _imagename, float _animSpeed, int _frameAmount, int _columns, int _rows, int _currentFrame, int _spriteW, int _spriteH);
	void InitAnimation(float _animSpeed, int _frameAmount, int _columns, int _rows, int _currentFrame, int _spriteW, int _spriteH);
	void LoadTheImage(const char* _imagename);
	virtual void Update(double dTime);
	virtual void Draw();

	int GetCurrentFrame() const;
	int GetFrameAmount() const;

	void SetAnimSpeed(float _spd);
	void SetFacing(float _facing);
	void SetSpriteScale(float _scale);
	void SetCurrentFrame(int _index);
	void SetCurrentFrameX(int _x);
	void SetCurrentFrameY(int _y);
	void SetWidth(float _width);
	void SetHeight(float _height);
	void SetIsVisible(bool _visible);
	void SetIsBillboard(bool _billboard);

private:
	GameObject * parent;

	bool isVisible;
	bool isBillboard;

	double tElapsed;
	float animSpeed;
	float frameW, frameH;
	float spriteW, spriteH;
	float facing;
	float spriteScale;
	int frameAmount;
	int frameColumns;
	int frameRows;
	int currentFrame;
	int currentFrameX;
	int currentFrameY;

	GLuint VAO;
	GLuint EBO;
	GLuint program;
	GLuint fillLoc;
	GLuint modelLoc;
	GLuint viewLoc;
	GLuint projLoc;
	GLuint texLoc;
	GLuint texOffsetLoc;
	GLuint texScaleLoc;
	GLuint scaleLoc;

	glm::vec3 fillColor;
	glm::vec2 texOffset;
	glm::vec2 texScale;
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
	glm::mat4 scaleMatrix;
};