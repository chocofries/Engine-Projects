#pragma once
#include <SOIL.h>
#include <iostream>
#include <string>
#include <map>
#include <ft2build.h>
#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include FT_FREETYPE_H

struct FontChar
{
	GLuint TextureID;
	glm::ivec2 size; //Size of glyph
	glm::ivec2 bearing; //Offset of glyph
	GLuint advance;	//How far to move for the next character
};

class TextureLoader
{
public:
	TextureLoader();
	~TextureLoader();

	GLuint CreateTexture(const char* textureFilePath, const char* textureName);
	void CreateFontCharacterMap(const char* fontName, FT_Face _face);

	std::map <std::string, std::map<GLchar, FontChar>*> &GetFontMap();


private:
	std::map<std::string, GLuint> textureMap;
	std::map <std::string, std::map<GLchar, FontChar>*> fontTextureMap;
};