// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;
// GL Includes

// EDIT
#include "utils.h"
#include "camera.h"
#include "gameobject.h"
// EDIT END

struct Vertex 
{
	glm::vec3 Position; // Position	
	glm::vec3 Normal; // Normal	
	glm::vec2 TexCoords; // TexCoords
};

struct MeshTexture
{
	GLuint id;
	string type;
	aiString path;
};

class ModelMesh 
{
public:
	/*  Mesh Data  */
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<MeshTexture> textures;

	/*  Functions  */
	// Constructor
	ModelMesh(vector<Vertex> vertices, vector<GLuint> indices, vector<MeshTexture> textures, GLuint program, GameObject* parent, GLuint skyBoxTexture)
	{
		this->parentModel = parent;
		this->skyBoxTexture = skyBoxTexture;
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;
		// Now that we have all the required data, set the vertex buffers and its attribute pointers.
		this->setupMesh();
		this->mvpLoc = glGetUniformLocation(program, "MVP");
		this->modelLoc = glGetUniformLocation(program, "model");
		this->transInvModelLoc = glGetUniformLocation(program, "transInvModel");

		this->ambientStrLoc = glGetUniformLocation(program, "ambientStr");
		this->ambientColorLoc = glGetUniformLocation(program, "ambientColor");
		this->lightColorLoc = glGetUniformLocation(program, "lightColor");
		this->lightPosLoc = glGetUniformLocation(program, "lightPos");
		this->camPosLoc = glGetUniformLocation(program, "camPos");

		ambientStr = 0.2f;
		ambientColor = glm::vec3(1.0f, 1.0f, 1.0f);
		lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
		lightPos = glm::vec3(200.0f, 600.0f, 300.0f);
	}

	void Update(Camera* camera, GLuint program, glm::mat4 &modelMatrix, glm::mat4 &_mvp, glm::mat4 &transInv)
	{
		camPos = camera->GetCamPos();
		model = modelMatrix;
		mvp = _mvp;
		transInvModel = transInv;
	}

	// Render the mesh
	void Render(Camera* camera, GLuint program, glm::mat4 &modelMatrix, glm::mat4 &_mvp, glm::mat4 &transInv)
	{
		camPos = camera->GetCamPos();
		model = modelMatrix;
		mvp = _mvp;
		transInvModel = transInv;

		glUseProgram(program);

		// Bind appropriate textures
		GLuint diffuseNr = 1;
		GLuint specularNr = 1;
		for (GLuint i = 0; i < this->textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i); // Active proper texture unit before binding
											  // Retrieve texture number (the N in diffuse_textureN)
			stringstream ss;
			string number;
			string name = this->textures[i].type;
			if (name == "texture_diffuse")
				ss << diffuseNr++; // Transfer GLuint to stream
			else if (name == "texture_specular")
				ss << specularNr++; // Transfer GLuint to stream
			number = ss.str();
			// Now set the sampler to the correct texture unit
			glUniform1i(glGetUniformLocation(program, (name + number).c_str()), i);
			// And finally bind the texture
			glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
		}

		//Camera
		glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(transInvModelLoc, 1, GL_FALSE, glm::value_ptr(transInvModel));

		//Lighting
		glUniform1f(ambientStrLoc, ambientStr);
		glUniform3f(ambientColorLoc, ambientColor.r, ambientColor.g, ambientColor.b);
		glUniform3f(lightColorLoc, lightColor.r, lightColor.g, lightColor.b);
		glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(camPosLoc, camPos.x, camPos.y, camPos.z);

		glActiveTexture(GL_TEXTURE10);
		glUniform1i(glGetUniformLocation(program, "skybox"), 10);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxTexture);

		// Draw mesh
		glBindVertexArray(this->VAO);
		glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// Always good practice to set everything back to defaults once configured.
		for (GLuint i = 0; i < this->textures.size(); i++){
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		glActiveTexture(GL_TEXTURE10);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

private:
	//Parent Model
	GameObject* parentModel;

	/*  Render data  */
	GLuint VAO, VBO, EBO;
	GLuint mvpLoc;
	GLuint modelLoc;
	GLuint transInvModelLoc;
	GLuint ambientStrLoc;
	GLuint ambientColorLoc;
	GLuint lightColorLoc;
	GLuint lightPosLoc;
	GLuint camPosLoc;
	GLuint skyBoxTexture;

	float ambientStr;
	glm::vec3 ambientColor;
	glm::vec3 lightColor;
	glm::vec3 lightPos;
	glm::vec3 camPos;
	glm::mat4 mvp;
	glm::mat4 transInvModel;
	glm::mat4 model;
	

	/*  Functions    */
	// Initializes all the buffer objects/arrays
	void setupMesh()
	{
		// Create buffers/arrays
		glGenVertexArrays(1, &this->VAO);
		glGenBuffers(1, &this->VBO);
		glGenBuffers(1, &this->EBO);

		glBindVertexArray(this->VAO);
		// Load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		// A great thing about structs is that their memory layout is sequential for all its items.
		// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
		// again translates to 3/2 floats which translates to a byte array.
		glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

		// Set the vertex attribute pointers
		// Vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
		// Vertex Normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
		// Vertex Texture Coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

		glBindVertexArray(0);
	}
};