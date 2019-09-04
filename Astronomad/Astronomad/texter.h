#pragma once
#include <iostream>
#include <SOIL.h>
#include <vector>
#include <ft2build.h>
#include "shaderloader.h"
#include "assetloader.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "utils.h"
#include "camera.h"
#include FT_FREETYPE_H

class Level;
class Texter
{
public:
	Texter(
		std::string _text,
		std::string _font,
		glm::vec3 pos,
		ShaderLoader* _shdloader,
		AssetLoader* _tloader,
		Camera* _cam,
		bool _isUI = true,
		bool _isBillboard = true,
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f),
		float scale = 1.0f
	);

	~Texter();
	void Render();
	void SetText(std::string newText) { text = newText; };
	void SetColor(glm::vec3 newColor) { color = newColor; };
	void SetScale(GLfloat newScale) { scale = newScale; };
	void SetPosition(glm::vec3 newPosition) { position = newPosition; };
	void SetIsUI(bool _isUI);
	void SetIsBillboard(bool _isBillboard);
	void SetCamera(Camera* _cam);
	void SetIsActive(bool _isActive);

	bool GetIsActive() const;

private:
	int isWorld;
	bool isUI;
	bool isBillboard;
	bool isActive;
	Camera* levelCamera;
	AssetLoader* textureLoader;

	//GLuint GenerateTexture(FT_Face _face);
	std::string text;
	GLfloat scale;
	glm::vec3 color;
	glm::vec3 position;

	GLuint VAO, VBO, program;
	GLuint projLoc, viewLoc, modelLoc, textColorLoc, texLoc, isWorldLoc, scaleLoc;
	glm::mat4 proj;
	glm::mat4 view;
	glm::mat4 model;
	glm::mat4 scaleMatrix;
	std::map<GLchar, FontChar>* Characters;
};