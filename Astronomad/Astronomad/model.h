#pragma once
// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;
// GL Includes

#include <glew.h>
#include <freeglut.h>
#include <SOIL.h>

//#include <glm.hpp>
//#include <gtc/matrix_transform.hpp>
//#include <gtc/type_ptr.hpp>
//
//#include <gtc/quaternion.hpp>
//#include <gtx/quaternion.hpp>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "ModelMesh.h"
#include "Camera.h"
#include "shaderloader.h"
#include "assetloader.h"
#include "gameobject.h"
#include "level.h"

class Model : public GameObject
{
public:
	
	GLuint program;
	Camera* camera;
	AssetLoader* assetLoader;
	GameObject* parent;
	GLuint skyboxTexture;
	
	/*  Functions   */
	// Constructor, expects a filepath to a 3D model.
	Model(std::string path, GameObject* _parent)
	{
		this->parent = _parent;
		this->camera = parent->GetCamera();
		this->shaderLoader = parent->GetShaderLoader();
		this->assetLoader = parent->GetAssetLoader();
		this->skyboxTexture = parent->GetSkyBoxTexture();
		this->program = shaderLoader->CreateProgram("Assets/VertexShader3DModel.vs", "Assets/FragmentShaderRimModel.fs", "vertModel0", "fragModelRim0");
		meshes = assetLoader->CreateVecMesh(path.c_str(), path.c_str(), this, parent);
		//this->loadModel(path);
		xRot = 0.0f;
		yRot = 0.0f;
		zRot = 1.0f;
	}

	virtual void Update(double dTime)
	{
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

		//Position
		glm::vec3 objPosition = glm::vec3(x, y, z);
		glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), objPosition);

		//Rotation
		glm::vec3 rotationAxisZ = glm::vec3(xRot, yRot, zRot);
		glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f), glm::radians(rotationAngle), rotationAxisZ);

		//Scale
		glm::vec3 objScale = glm::vec3(xScale, yScale, zScale);
		glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), objScale);

		////Model Matrix
		modelMatrix = translationMatrix * rotationZ * scaleMatrix;
		mvp = camera->GetProjection() * camera->GetView() * modelMatrix;
		transInvModel = glm::transpose(glm::inverse(modelMatrix));

		//for (GLuint i = 0; i < this->meshes->size(); i++) {

		//	//printf("mesh size: %d \n", meshes.size());
		//	this->meshes[i]->Update(camera, program, modelMatrix, mvp, transInvModel);
		//}

		if (!meshes->empty())
		{
			for (auto it = meshes->begin(); it != meshes->end(); it++)
			{
				ModelMesh* tempMesh = &(*it);
				tempMesh->Update(camera, program, modelMatrix, mvp, transInvModel);
			}
		}
	}

	// Draws the model, and thus all its meshes
	virtual void Draw()
	{
		//for (GLuint i = 0; i < this->meshes->size(); i++)
		//{
		////printf("mesh size: %d \n", meshes.size());
		//	this->meshes[i].Render(camera, program);
		//}

		if (!meshes->empty())
		{
			for (auto it = meshes->begin(); it != meshes->end(); it++)
			{
				ModelMesh* tempMesh = &(*it);
				tempMesh->Render(camera, program, modelMatrix, mvp, transInvModel);
			}
		}
	}

	// Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	void loadModel(string path)
	{
		meshes = &tempVec;
		// Read file via ASSIMP
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

		// Check for errors
		if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
		{
			cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
			return;
		}
		// Retrieve the directory path of the filepath
		this->directory = path.substr(0, path.find_last_of('/'));

		// Process ASSIMP's root node recursively
		this->processNode(scene->mRootNode, scene);
	}

	vector<ModelMesh>* GetVecMeshes()
	{
		return meshes;
	}

private:
	/*  Model Data  */
	glm::mat4 modelMatrix;
	glm::mat4 mvp;
	glm::mat4 transInvModel;
	vector<ModelMesh> tempVec;
	vector<ModelMesh>* meshes;
	string directory;
	vector<MeshTexture> textures_loaded;	// Stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.

	/*  Functions   */

	// Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void processNode(aiNode* node, const aiScene* scene)
	{
		// Process each mesh located at the current node
		for (GLuint i = 0; i < node->mNumMeshes; i++)
		{
			// The node object only contains indices to index the actual objects in the scene. 
			// The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			this->meshes->push_back(this->processMesh(mesh, scene));
		}


		// After we've processed all of the meshes (if any) we then recursively process each of the children nodes
		for (GLuint i = 0; i < node->mNumChildren; i++)
		{
			this->processNode(node->mChildren[i], scene);
		}

	}

	ModelMesh processMesh(aiMesh* mesh, const aiScene* scene)
	{
		// Data to fill
		vector<Vertex> vertices;
		vector<GLuint> indices;
		vector<MeshTexture> textures;

		// Walk through each of the mesh's vertices
		for (GLuint i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			glm::vec3 vector; // We declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
							  // Positions
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;
			// Normals
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
			// Texture Coordinates
			if (mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
			{
				glm::vec2 vec;
				// A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
				// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			vertices.push_back(vertex);
		}
		// Now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
		for (GLuint i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			// Retrieve all indices of the face and store them in the indices vector
			for (GLuint j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}
		// Process materials
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			// We assume a convention for sampler names in the shaders. Each diffuse texture should be named
			// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
			// Same applies to other texture as the following list summarizes:
			// Diffuse: texture_diffuseN
			// Specular: texture_specularN
			// Normal: texture_normalN

			// 1. Diffuse maps
			vector<MeshTexture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			// 2. Specular maps
			vector<MeshTexture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		}

		// Return a mesh object created from the extracted mesh data
		return ModelMesh(vertices, indices, textures, program, this, skyboxTexture);
	}

	// Checks all material textures of a given type and loads the textures if they're not loaded yet.
	// The required info is returned as a Texture struct.
	vector<MeshTexture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
	{
		vector<MeshTexture> textures;
		for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
			GLboolean skip = false;
			for (GLuint j = 0; j < textures_loaded.size(); j++)
			{
				if (textures_loaded[j].path == str)
				{
					textures.push_back(textures_loaded[j]);
					skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
					break;
				}
			}
			if (!skip)
			{   // If texture hasn't been loaded already, load it
				MeshTexture texture;
				texture.id = TextureFromFile(str.C_Str(), this->directory);
				texture.type = typeName;
				texture.path = str;
				textures.push_back(texture);
				this->textures_loaded.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
			}
		}
		return textures;
	}

	GLint TextureFromFile(const char* path, string directory)
	{
		//Generate texture ID and load texture data 
		string pathStr = string(path);
		string dirStr = directory;

		string fileStr = dirStr + '/' + pathStr;

		cout << "texture filename: " << fileStr << endl;

		const char* fileChar = fileStr.c_str();

		//Get rid of spaces and periods
		pathStr.erase(std::remove(pathStr.begin(), pathStr.end(), ' '), pathStr.end());
		pathStr.erase(std::remove(pathStr.begin(), pathStr.end(), '.'), pathStr.end());
		const char* pathChar = pathStr.c_str();

		GLuint textureID = assetLoader->CreateTexture(fileChar, pathChar);

		return textureID;
	}
};

