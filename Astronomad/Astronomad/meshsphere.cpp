#include "meshsphere.h"

MeshSphere::MeshSphere(GameObject* _parent, GLuint _skyBoxTexture)
{
	parent = _parent;
	levelCamera = parent->GetCamera();
	shaderLoader = parent->GetShaderLoader();
	assetLoader = parent->GetAssetLoader();
	clock = parent->GetClock();
	VBO = parent->GetVBO();
	isVisible = true;
	xRot = 0.0f;
	yRot = 0.0f;
	zRot = 1.0f;
	ambientStr = 0.05f;
	ambientColor = glm::vec3(1.0f, 1.0f, 1.0f);
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	lightPos = glm::vec3(-200.0f, 600.0f, 300.0f);
	skyBoxTexture = _skyBoxTexture;
}

MeshSphere::~MeshSphere()
{
}

void MeshSphere::Initialise()
{
	float radius = 1.0f;

	const int sections = 30;
	const int vertexAttrib = 8;
	const int indexPerQuad = 6;

	double phi = 0;
	double theta = 0;

	float vertices[(sections) * (sections)* vertexAttrib];
	int offset = 0;
	for (int i = 0; i < sections; i++)
	{
		theta = 0;

		for (int j = 0; j < sections; j++)
		{
			float x = float(cos(phi) * sin(theta));
			float y = float(cos(theta));
			float z = float(sin(phi) * sin(theta));

			//Position
			vertices[offset++] = x * radius;
			vertices[offset++] = y * radius;
			vertices[offset++] = z * radius;

			//Normal?
			vertices[offset++] = x;
			vertices[offset++] = y;
			vertices[offset++] = z;

			//Tex Coords
			vertices[offset++] = (float)i / (sections - 1);
			vertices[offset++] = (float)j / (sections - 1);

			theta += (M_PI / (sections - 1));
		}

		phi += (2 * M_PI) / (sections - 1);
	}

	GLuint indices[(sections) * (sections)* indexPerQuad];
	offset = 0;
	for (int i = 0; i < sections; i++)
	{
		for (int j = 0; j < sections; j++)
		{
			indices[offset++] = (((i + 1) % sections) * sections) + ((j + 1) % sections);
			indices[offset++] = (i * sections) + (j);
			indices[offset++] = (((i + 1) % sections) * sections) + (j);

			indices[offset++] = (i * sections) + ((j + 1) % sections);
			indices[offset++] = (i * sections) + (j);
			indices[offset++] = (((i + 1) % sections) * sections) + ((j + 1) % sections);
		}
	}

	program = shaderLoader->CreateProgram("Assets/VertexShader3D.vs", "Assets/FragmentShaderRimModel.fs", "vertShader3D0", "fragShaderRimModel0");

	//GLuint EBO, VBO;

	//VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//EBO
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//Vertex Attributes
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	indexCount = sizeof(indices) / sizeof(GLuint);

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

void MeshSphere::Update(double dTime)
{
	Mesh::Update(dTime);
}

void MeshSphere::Draw()
{
	Mesh::Draw();
}
