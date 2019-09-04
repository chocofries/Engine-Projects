#include "sprite.h"

Sprite::Sprite(GameObject* _parent)
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
	isBillboard = false;
}

Sprite::~Sprite()
{
	glDeleteTextures(1, &texture);
	glDeleteVertexArrays(1, &VAO);
}

void Sprite::Initialise()
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

	if (!isBillboard)
	{
		program = shaderLoader->CreateProgram("Assets/VertexShader.vs", "Assets/FragmentShader.fs", "vertShader0", "fragShader0");
	}
	else
	{
		program = shaderLoader->CreateProgram("Assets/VertexShaderBillboard.vs", "Assets/FragmentShader.fs", "vertShaderBillboard0", "fragShader0");
	}

	//glGenBuffers(1, &VBO);

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
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);
	//glEnable(GL_CULL_FACE);

	//Get Uniform Locations
	texOffsetLoc = glGetUniformLocation(program, "texOffset");
	texScaleLoc = glGetUniformLocation(program, "texScale");
	modelLoc = glGetUniformLocation(program, "model");
	viewLoc = glGetUniformLocation(program, "view");
	projLoc = glGetUniformLocation(program, "proj");
	texLoc = glGetUniformLocation(program, "tex");
	if (isBillboard)
	{
		scaleLoc = glGetUniformLocation(program, "scale");
	}
}

void Sprite::InitSprite(const char* _imagepath, const char* _imagename, float _animSpeed, int _frameAmount, int _columns, int _rows, int _currentFrame, int _imageW, int _imageH)
{
	imageFilePath = _imagepath;
	LoadTheImage(_imagename);
	Initialise();
	InitAnimation(_animSpeed, _frameAmount, _columns, _rows, _currentFrame, _imageW, _imageH);
}


void Sprite::InitAnimation(float _animSpeed, int _frameAmount, int _columns, int _rows, int _currentFrame, int _imageW, int _imageH)
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
	facing = 1.0f;
	tElapsed = 0.0f;
}

void Sprite::LoadTheImage(const char* _imagename)
{
	texture = assetLoader->CreateTexture(imageFilePath, _imagename);
}

void Sprite::Update(double dTime)
{
	deltaTime = (float)dTime;
	x = parent->GetX();
	y = parent->GetY();
	z = parent->GetZ();
	xRot = 0.0f;
	yRot = 0.0f;
	zRot = 1.0f;
	rotationAngle = parent->GetRotationAngle();
	xScale = spriteW * facing * spriteScale;
	yScale = spriteH * spriteScale;
	zScale = 1.0f;
	isActive = parent->GetIsActive();
	isVisible = isActive;

	////View Matrix
	view = levelCamera->GetView();

	////Projection Matrix
	proj = levelCamera->GetProjection();

	////Position
	glm::vec3 objPosition = glm::vec3(x, y, z);
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), objPosition);

	//Scale
	glm::vec3 objScale = glm::vec3(xScale, yScale, zScale);
	scaleMatrix = glm::scale(glm::mat4(1.0f), objScale);

	//Rotation
	glm::vec3 rotationAxisZ = glm::vec3(xRot, yRot, zRot);
	glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f), glm::radians(rotationAngle), rotationAxisZ);

	////Model Matrix
	model = translationMatrix * rotationZ * scaleMatrix;
	

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

	glutPostRedisplay();
}

void Sprite::Draw()
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

		//View Matrix
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		//Projection Matrix
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));


		//Scale Matrix
		if (isBillboard)
		{
			glUniformMatrix4fv(scaleLoc, 1, GL_FALSE, glm::value_ptr(scaleMatrix));
		}

		//Texture Coordinates
		glUniform2f(texOffsetLoc, texOffset.x, texOffset.y);
		glUniform2f(texScaleLoc, texScale.x, texScale.y);

		//Texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(texLoc, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//Drawing based on verts
		glBindVertexArray(VAO);
		//glPolygonMode(GL_BACK, GL_LINE);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glDisable(GL_BLEND);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

int Sprite::GetCurrentFrame() const
{
	return currentFrame;
}

int Sprite::GetFrameAmount() const
{
	return frameAmount;
}

void Sprite::SetAnimSpeed(float _spd)
{
	animSpeed = _spd;
}

void Sprite::SetFacing(float _facing)
{
	facing = _facing;
}

void Sprite::SetSpriteScale(float _scale)
{
	spriteScale = _scale;
}

void Sprite::SetCurrentFrame(int _index)
{
	currentFrame = _index;
}

void Sprite::SetCurrentFrameX(int _x)
{
	currentFrameX = _x;
}

void Sprite::SetCurrentFrameY(int _y)
{
	currentFrameY = _y;
}

void Sprite::SetWidth(float _width)
{
	spriteW = _width;
}

void Sprite::SetHeight(float _height)
{
	spriteH = _height;
}

void Sprite::SetIsVisible(bool _visible)
{
	isVisible = _visible;
}

void Sprite::SetIsBillboard(bool _billboard)
{
	isBillboard = _billboard;
}
