#pragma once
#include "ShaderLoader.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "gameobject.h"
#include "camera.h"


class Mesh : public GameObject
{
public:
	Mesh();
	Mesh(GameObject * _parent);
	virtual ~Mesh();

	virtual void Initialise();
	void InitMesh(const char* _imagepath, const char* _imagename, float _animSpeed, int _frameAmount, int _columns, int _rows, int _currentFrame, int _spriteW, int _spriteH);
	void InitAnimation(float _animSpeed, int _frameAmount, int _columns, int _rows, int _currentFrame, int _spriteW, int _spriteH);
	void LoadTheImage(const char* _imagename);
	virtual void Update(double dTime);
	virtual void Draw();

	int GetCurrentFrame() const;

	void SetAnimSpeed(float _spd);
	void SetCurrentFrame(int _index);
	void SetCurrentFrameX(int _x);
	void SetCurrentFrameY(int _y);
	void SetWidth(float _width);
	void SetHeight(float _height);
	void SetIsVisible(bool _visible);

protected:
	GameObject * parent;

	bool isVisible;

	double tElapsed;
	float animSpeed;
	float frameW, frameH;
	float spriteW, spriteH;
	int frameAmount;
	int frameColumns;
	int frameRows;
	int currentFrame;
	int currentFrameX;
	int currentFrameY;

	GLuint indexCount;
	//GLuint myVBO;
	GLuint VAO;
	GLuint EBO;
	GLuint program;
	GLuint modelLoc;
	GLuint transInvModelLoc;
	GLuint texLoc;
	GLuint ambientStrLoc;
	GLuint ambientColorLoc;
	GLuint lightColorLoc;
	GLuint lightPosLoc;
	GLuint camPosLoc;
	GLuint MVPLoc;
	GLuint skyBoxLoc;
	GLuint skyBoxTexture;

	float ambientStr;
	glm::vec3 ambientColor;
	glm::vec3 lightColor;
	glm::vec3 lightPos;
	glm::vec3 camPos;

	glm::vec3 fillColor;
	glm::vec2 texOffset;
	glm::vec2 texScale;
	glm::mat4 model;
	glm::mat4 transInvModel;
	glm::mat4 view;
	glm::mat4 proj;
	glm::mat4 MVP;
};