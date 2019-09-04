#include "ShaderLoader.h" 
#include<iostream>
#include<fstream>
#include<vector>

ShaderLoader::ShaderLoader(void) {}
ShaderLoader::~ShaderLoader(void) {}


std::string ShaderLoader::ReadShader(const char *filename)
{
	std::string shaderCode;
	std::ifstream file(filename, std::ios::in);

	if (!file.good()) {
		std::cout << "Can't read file " << filename << std::endl;
		std::terminate();
	}

	file.seekg(0, std::ios::end);
	shaderCode.resize((unsigned int)file.tellg());
	file.seekg(0, std::ios::beg);
	file.read(&shaderCode[0], shaderCode.size());
	file.close();
	return shaderCode;
}

GLuint ShaderLoader::CreateShader(GLenum shaderType, std::string source, const char* shaderName)
{
	std::map<std::string, GLuint>::iterator it;
	GLuint shader;

	if (shaderType == GL_VERTEX_SHADER) //If the shader is a vertex shader instead of a fragment shader
	{
		it = vertexShaderMap.find(shaderName);

		if (it == vertexShaderMap.end()) //If the shader is not found
		{
			//Create the shader
			int compile_result = 0;

			shader = glCreateShader(shaderType);
			const char *shader_code_ptr = source.c_str();
			const int shader_code_size = source.size();

			glShaderSource(shader, 1, &shader_code_ptr, &shader_code_size);
			glCompileShader(shader);
			glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_result);

			//Add the shader to the map
			vertexShaderMap.insert({ shaderName, shader });

			//check for errors
			if (compile_result == GL_FALSE)
			{

				int info_log_length = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);
				std::vector<char> shader_log(info_log_length);
				glGetShaderInfoLog(shader, info_log_length, NULL, &shader_log[0]);
				std::cout << "ERROR compiling vertex shader: " << shaderName << std::endl << &shader_log[0] << std::endl;
				return 0;
			}
		}
		else //If it is found, return the existing shader
		{
			shader = it->second;
		}
	}
	else if (shaderType == GL_FRAGMENT_SHADER) //If it is a fragment shader
	{
		it = fragmentShaderMap.find(shaderName);

		if (it == fragmentShaderMap.end()) //If the shader is not found
		{
			//Create the shader
			int compile_result = 0;

			shader = glCreateShader(shaderType);
			const char *shader_code_ptr = source.c_str();
			const int shader_code_size = source.size();

			glShaderSource(shader, 1, &shader_code_ptr, &shader_code_size);
			glCompileShader(shader);
			glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_result);

			//Add the shader to the map
			fragmentShaderMap.insert({ shaderName, shader });

			//check for errors
			if (compile_result == GL_FALSE)
			{

				int info_log_length = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);
				std::vector<char> shader_log(info_log_length);
				glGetShaderInfoLog(shader, info_log_length, NULL, &shader_log[0]);
				std::cout << "ERROR compiling fragment shader: " << shaderName << std::endl << &shader_log[0] << std::endl;
				return 0;
			}
		}
		else //If it is found, return the existing shader
		{
			shader = it->second;
		}
	}

	return shader;
}

GLuint ShaderLoader::CreateProgram(const char* vertexShaderFilename, const char* fragmentShaderFilename, const char* vertexShaderName, const char* fragmentShaderName)
{
	//Concatenate the shader names to create the program name
	std::string vertString(vertexShaderName);
	std::string fragString(fragmentShaderName);
	std::string programString = vertString + fragString;

	if (programMap.find(programString) == programMap.end()) //If the shader is not found
	{
		//read the shader files and save the code
		std::string vertex_shader_code = ReadShader(vertexShaderFilename);
		std::string fragment_shader_code = ReadShader(fragmentShaderFilename);

		GLuint vertex_shader = CreateShader(GL_VERTEX_SHADER, vertex_shader_code, vertexShaderName);
		GLuint fragment_shader = CreateShader(GL_FRAGMENT_SHADER, fragment_shader_code, fragmentShaderName);

		int link_result = 0;
		//create the program handle, attach the shaders and link it
		GLuint program = glCreateProgram();
		glAttachShader(program, vertex_shader);
		glAttachShader(program, fragment_shader);

		glLinkProgram(program);
		glGetProgramiv(program, GL_LINK_STATUS, &link_result);

		//Add the program to the map
		programMap.insert({ programString, program });

		//check for link errors
		if (link_result == GL_FALSE)
		{
			int info_log_length = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);
			std::vector<char> program_log(info_log_length);
			glGetProgramInfoLog(program, info_log_length, NULL, &program_log[0]);
			std::cout << "Shader Loader : LINK ERROR" << std::endl << &program_log[0] << std::endl;
			return 0;
		}

		return program;
	}
	else //If it is found, return the existing program
	{
		return (programMap.find(programString)->second);
	}
}