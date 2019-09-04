#pragma once
#include "level.h"

class CubeMap : public GameObject
{
public:
	CubeMap(Level* level, std::vector<std::string>* vecCubeMapPaths, std::string _fullPath);
	virtual ~CubeMap();

	virtual void Initialise();
	virtual void Update(double dTime);
	virtual void Draw();

	GLuint GetTexture() const;

private:
	Level * level;

	GLuint program;
	GLuint VBO;
	GLuint VAO;
	GLuint EBO;

	GLuint modelLoc;
	GLuint viewLoc;
	GLuint projLoc;

	std::string fullPath;
	std::vector<std::string>* vecCubeMapPaths;

	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};