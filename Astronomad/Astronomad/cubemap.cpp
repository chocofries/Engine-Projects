#include "cubemap.h"

CubeMap::CubeMap(Level* level, std::vector<std::string>* vecCubeMapPaths, std::string _fullPath)
{
	this->level = level;
	levelCamera = level->GetCamera();
	shaderLoader = level->GetShaderLoader();
	assetLoader = level->GetTextureLoader();
	clock = level->GetClock();
	//VBO = level->GetVBO();
	this->vecCubeMapPaths = vecCubeMapPaths;
	fullPath = _fullPath;
	xRot = 0.0f;
	yRot = 0.0f;
	zRot = 1.0f;
	xScale = 20000.0f;
	yScale = 20000.0f;
	zScale = 20000.0f;
	isActive = true;
	isLoaded = true;
}

CubeMap::~CubeMap()
{
}

void CubeMap::Initialise()
{
	GLfloat cubeVerts[] = {
		//Positions              //Normals             //Tex Coords

		 //Front
		-0.5f,  0.5f,  0.5f,     0.0f, 0.0f, -1.0f,     0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,     0.0f, 0.0f, -1.0f,     0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,     0.0f, 0.0f, -1.0f,     1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,     0.0f, 0.0f, -1.0f,     1.0f, 0.0f,
		 //Back
		-0.5f,  0.5f, -0.5f,     0.0f, 0.0f, -1.0f,     1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,     0.0f, 0.0f, -1.0f,     1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,     0.0f, 0.0f, -1.0f,     0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,     0.0f, 0.0f, -1.0f,     0.0f, 0.0f,
		 //Left
		-0.5f,  0.5f, -0.5f,     0.0f, 0.0f, -1.0f,     0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,     0.0f, 0.0f, -1.0f,     0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,     0.0f, 0.0f, -1.0f,     1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,     0.0f, 0.0f, -1.0f,     1.0f, 0.0f,
		//Right
		0.5f,  0.5f,  0.5f,     0.0f, 0.0f, -1.0f,     0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,     0.0f, 0.0f, -1.0f,     0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,     0.0f, 0.0f, -1.0f,     1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,     0.0f, 0.0f, -1.0f,     1.0f, 0.0f,
		//Top
	   -0.5f,  0.5f, -0.5f,     0.0f, 0.0f, -1.0f,     0.0f, 0.0f,
	   -0.5f,  0.5f,  0.5f,     0.0f, 0.0f, -1.0f,     0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,     0.0f, 0.0f, -1.0f,     1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,     0.0f, 0.0f, -1.0f,     1.0f, 0.0f,
		//Bottom
		-0.5f, -0.5f,  0.5f,     0.0f, 0.0f, -1.0f,     0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,     0.0f, 0.0f, -1.0f,     0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,     0.0f, 0.0f, -1.0f,     1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,     0.0f, 0.0f, -1.0f,     1.0f, 0.0f
	};

	GLuint cubeIndis[] = {

		//Front
		0, 1, 2,
		0, 2, 3,
		//Back
		7, 6, 5,
		7, 5, 4,
		//Left
		8, 9, 10,
		8, 10, 11,
		//Right
		12, 13, 14,
		12, 14, 15,
		//Top
		16, 17, 18,
		16, 18, 19,
		//Bottom
		20, 21, 22,
		20, 22, 23
	};

	program = shaderLoader->CreateProgram("Assets/VertexShaderCubeMap.vs","Assets/FragmentShaderCubeMap.fs","vertCubeMap0","fragCubeMap0"); //Cubemap shaders

	glGenBuffers(1, &VBO);
	//glBindBuffer(GL_ARRAY_BUFFER, levelVBO);

	//VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//EBO
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndis), cubeIndis, GL_STATIC_DRAW);

	//Vertex Attributes
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	//Load Images
	unsigned char* image;
	int texChannels, tWidth, tHeight;

	//Texture
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	for (int i = 0; i < 6; i++)
	{
		std::string strPath = fullPath;
		strPath.append(vecCubeMapPaths->at(i));
	
		//Create the texture
		image = SOIL_load_image(strPath.c_str(), &tWidth, &tHeight, &texChannels, SOIL_LOAD_RGBA);
		const char* debugResult = SOIL_last_result();

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, tWidth, tHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

		SOIL_free_image_data(image);
	}

	//Wrapping
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	//Backface Culling
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
	glEnable(GL_CULL_FACE);

	//MipMaps
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	//Get Uniform Locations
	modelLoc = glGetUniformLocation(program, "model");
	viewLoc = glGetUniformLocation(program, "view");
	projLoc = glGetUniformLocation(program, "proj");
}

void CubeMap::Update(double dTime)
{
	deltaTime = (float)dTime;
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	rotationAngle = 0.0f;
	xScale = 20000.0f;
	yScale = 20000.0f;
	zScale = 20000.0f;

	////Position
	glm::vec3 objPosition = glm::vec3(x, y, z);
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), objPosition);

	//Rotation
	glm::vec3 rotationAxisZ = glm::vec3(xRot, yRot, zRot);
	glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f), glm::radians(rotationAngle), rotationAxisZ);

	//Scale
	glm::vec3 objScale = glm::vec3(xScale, yScale, zScale);
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), objScale);

	////Model Matrix
	model = translationMatrix * rotationZ * scaleMatrix;

	////View Matrix
	view = levelCamera->GetView();

	////Projection Matrix
	proj = levelCamera->GetProjection();

	glutPostRedisplay();
}

void CubeMap::Draw()
{
	glUseProgram(program);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	//Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	glUniform1i(glGetUniformLocation(program, "cubeSampler"), 0);

	//Model Matrix
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	//View Matrix
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	//Projection Matrix
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glUseProgram(0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

GLuint CubeMap::GetTexture() const
{
	return texture;
}
