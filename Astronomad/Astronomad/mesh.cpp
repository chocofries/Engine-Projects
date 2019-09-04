#include "mesh.h"

Mesh::Mesh()
{
}

Mesh::Mesh(GameObject* _parent)
{
	parent = _parent;
	levelCamera = parent->GetCamera();
	shaderLoader = parent->GetShaderLoader();
	assetLoader = parent->GetAssetLoader();
	clock = parent->GetClock();
	VBO = parent->GetVBO();
	animSpeed = 0.0f;
	frameAmount = 1;
	frameRows = 1;
	frameColumns = 1;
	currentFrame = 0;
	currentFrameX = 0;
	currentFrameY = 0;
	frameW = 1.0f;
	frameH = 1.0f;
	isActive = false;
	isVisible = true;
	xRot = 0.0f;
	yRot = 0.0f;
	zRot = 1.0f;
	ambientStr = 0.05f;
	ambientColor = glm::vec3(1.0f, 1.0f, 1.0f);
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	lightPos = glm::vec3(-200.0f, -600.0f, -300.0f);
}

Mesh::~Mesh()
{
	glDeleteTextures(1, &texture);
	glDeleteVertexArrays(1, &VAO);
}

void Mesh::Initialise() //Default initialises as a quad
{
	GLfloat quadVerts[] =
	{
		//--------------------------------------------- First Triangle (Left-bottom)
		// Position				 // Color			 //Tex Coords
		-0.24f, -0.24f, 0.0f,	 0.0f, 1.0f, 1.0f,	 0.0f, 1.0f,	//Left
		0.24f, -0.24f, 0.0f,	 1.0f, 0.0f, 1.0f,	 1.0f, 1.0f,	//Right
		-0.24f,  0.24f, 0.0f,	 1.0f, 1.0f, 0.0f,	 0.0f, 0.0f,	//Top
		//--------------------------------------------- Second Triangle (Right-top)
		0.24f,  0.24f, 0.0f,	 0.0f, 1.0f, 1.0f,	 1.0f, 0.0f		//Left
	};

	GLuint quadIndices[] =
	{
		0, 1, 2, //1st Triangle
		3, 2, 1  //2nd Triangle
	};

	program = shaderLoader->CreateProgram("Assets/VertexShader3D.vs", "Assets/FragmentShaderRimModel.fs", "vertShader3D0", "fragShaderRimModel0");

	//VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//EBO
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

	//Vertex Attributes
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	//Backface Culling
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);

	//Get Uniform Locations
	modelLoc = glGetUniformLocation(program, "model");
	transInvModelLoc = glGetUniformLocation(program, "transInvModel");
	MVPLoc = glGetUniformLocation(program, "MVP");
	texLoc = glGetUniformLocation(program, "texture_diffuse1");
	skyBoxLoc = glGetUniformLocation(program, "skybox");

	ambientStrLoc = glGetUniformLocation(program, "ambientStr");
	ambientColorLoc = glGetUniformLocation(program, "ambientColor");
	lightColorLoc = glGetUniformLocation(program, "lightColor");
	lightPosLoc = glGetUniformLocation(program, "lightPos");
	camPosLoc = glGetUniformLocation(program, "camPos");
}

void Mesh::InitMesh(const char* _imagepath, const char* _imagename, float _animSpeed, int _frameAmount, int _columns, int _rows, int _currentFrame, int _imageW, int _imageH)
{
	imageFilePath = _imagepath;
	LoadTheImage(_imagename);
	Initialise();
	InitAnimation(_animSpeed, _frameAmount, _columns, _rows, _currentFrame, _imageW, _imageH);
}


void Mesh::InitAnimation(float _animSpeed, int _frameAmount, int _columns, int _rows, int _currentFrame, int _imageW, int _imageH)
{
	animSpeed = _animSpeed;
	frameAmount = _frameAmount;
	frameColumns = _columns;
	frameRows = _rows;
	currentFrame = _currentFrame;
	frameW = 1.0f / frameColumns;
	frameH = 1.0f / frameRows;
	spriteW = (float)_imageW / (float)_columns;
	spriteH = (float)_imageH / (float)_rows;
	tElapsed = 0.0f;
}

void Mesh::LoadTheImage(const char* _imagename)
{
	texture = assetLoader->CreateTexture(imageFilePath, _imagename);
}

void Mesh::Update(double dTime)
{
	deltaTime = (float)dTime;
	x = parent->GetX();
	y = parent->GetY();
	z = parent->GetZ();
	rotationAngle = parent->GetRotationAngle();
	xScale = parent->GetXScale();
	yScale = parent->GetYScale();
	zScale = parent->GetZScale();
	xRot = parent->GetXRot();
	yRot = parent->GetYRot();
	zRot = parent->GetZRot();
	isActive = parent->GetIsActive();
	isVisible = isActive;

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

	////Transposed, Inversed Model Matrix
	transInvModel = glm::transpose(glm::inverse(model));

	////View Matrix
	view = levelCamera->GetView();

	////Projection Matrix
	proj = levelCamera->GetProjection();

	////MVP Matrix
	MVP = proj * view * model;

	//Texture Coordinate Offset & Scale
	tElapsed += deltaTime;
	if (animSpeed > 0.0f)
	{
		if (tElapsed > animSpeed)
		{
			tElapsed = 0.000f;
			if (currentFrame < frameAmount - 1)
			{
				if (currentFrameX < frameColumns - 1)
				{
					currentFrameX += 1;
				}
				else
				{
					currentFrameX = 0;
					currentFrameY += 1;
				}
				currentFrame += 1;
			}
			else
			{
				currentFrameX = 0;
				currentFrameY = 0;
				currentFrame = 0;
			}
		}
	}

	texOffset = glm::vec2(currentFrameX, currentFrameY);
	texScale = glm::vec2(frameW, frameH);
	camPos = levelCamera->GetCamPos();

	glutPostRedisplay();
}

void Mesh::Draw()
{
	if (isVisible)
	{
		glUseProgram(program);

		//Alpha Blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//Depth Enable
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		//Model Matrix
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		//Trans Inv Model
		glUniformMatrix4fv(transInvModelLoc, 1, GL_FALSE, glm::value_ptr(transInvModel));

		//MVP Matrix
		glUniformMatrix4fv(MVPLoc, 1, GL_FALSE, glm::value_ptr(MVP));

		//Lighting
		glUniform1f(ambientStrLoc, ambientStr);
		glUniform3f(ambientColorLoc, ambientColor.r, ambientColor.g, ambientColor.b);
		glUniform3f(lightColorLoc, lightColor.r, lightColor.g, lightColor.b);
		glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z); 
		glUniform3f(camPosLoc, camPos.x, camPos.y, camPos.z);

		//Texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(texLoc, 0);

		//Skybox
		glActiveTexture(GL_TEXTURE1);
		glUniform1i(skyBoxLoc, 1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxTexture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//Drawing based on verts
		glBindVertexArray(VAO);
		//glPolygonMode(GL_BACK, GL_LINE);
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glDisable(GL_BLEND);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
}

int Mesh::GetCurrentFrame() const
{
	return currentFrame;
}

void Mesh::SetAnimSpeed(float _spd)
{
	animSpeed = _spd;
}

void Mesh::SetCurrentFrame(int _index)
{
	currentFrame = _index;
}

void Mesh::SetCurrentFrameX(int _x)
{
	currentFrameX = _x;
}

void Mesh::SetCurrentFrameY(int _y)
{
	currentFrameY = _y;
}

void Mesh::SetWidth(float _width)
{
	spriteW = _width;
}

void Mesh::SetHeight(float _height)
{
	spriteH = _height;
}

void Mesh::SetIsVisible(bool _visible)
{
	isVisible = _visible;
}