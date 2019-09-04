#pragma once

#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include <iostream>
#include<string>
#include <map>


class ShaderLoader
{
private:
	std::string ReadShader(const char *filename);
	GLuint CreateShader(GLenum shaderType, std::string source, const  char* shaderName);

public:
	ShaderLoader(void);
	~ShaderLoader(void);
	GLuint CreateProgram(const char* VertexShaderFilename, const  char* FragmentShaderFilename, const char* vertexShaderName, const  char* fragmentShaderName);

private:
	std::map<std::string, GLuint> vertexShaderMap;
	std::map<std::string, GLuint> fragmentShaderMap;
	std::map<std::string, GLuint> programMap;
};
