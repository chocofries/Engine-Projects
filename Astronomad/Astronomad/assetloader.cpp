#include "assetloader.h"
#include "model.h"

AssetLoader::AssetLoader()
{
}

AssetLoader::~AssetLoader()
{
	std::map <std::string, std::map<GLchar, FontChar>*>::iterator itr;
	for (itr = fontTextureMap.begin(); itr != fontTextureMap.end();)
	{
		if (itr->second != nullptr)
		{
			delete itr->second;
			fontTextureMap.erase(itr++);
		}
		else
		{
			++itr;
		}
	}
}

GLuint AssetLoader::CreateTexture(const char * textureFilePath, const char * textureName)
{
	std::map<std::string, GLuint>::iterator it;
	GLuint texture;

	it = textureMap.find(textureName);

	if (it == textureMap.end()) //If texture was not found
	{
		unsigned char* image;
		int texChannels, texW, texH;

		//Create the texture
		image = SOIL_load_image(textureFilePath, &texW, &texH, &texChannels, SOIL_LOAD_RGBA);
		const char* debugResult = SOIL_last_result();

		//Texture
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

		//Add the texture to the map
		textureMap.insert({ textureName, texture });

		//Mipmaps
		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else //If it is found, return the existing texture
	{
		texture = it->second;
	}

	return texture;
}

void AssetLoader::CreateFontCharacterMap(const char * fontName, FT_Face _face)
{
	std::map <std::string, std::map<GLchar, FontChar>*>::iterator iter;
	std::map<GLchar, FontChar>* tempMap = nullptr;

	iter = fontTextureMap.find(fontName);

	if (iter == fontTextureMap.end()) //If the map of characters was not found
	{
		std::map<GLchar, FontChar>* charactersMap = new std::map<GLchar, FontChar>;

		for (GLubyte c = 0; c < 128; c++)
		{
			if (FT_Load_Char(_face, c, FT_LOAD_RENDER))
			{
				std::cout << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
				continue;
			}

			//GLuint fontTexture = GenerateTexture(face);
			GLuint fontTexture;
			glGenTextures(1, &fontTexture);
			glBindTexture(GL_TEXTURE_2D, fontTexture);

			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				_face->glyph->bitmap.width,
				_face->glyph->bitmap.rows,
				0,
				GL_RED, GL_UNSIGNED_BYTE,
				_face->glyph->bitmap.buffer
			);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


			//Store character for later use
			FontChar characterThing = { fontTexture,
				glm::ivec2(_face->glyph->bitmap.width, _face->glyph->bitmap.rows),
				glm::ivec2(_face->glyph->bitmap_left, _face->glyph->bitmap_top),
				(GLuint)_face->glyph->advance.x
			};

			(*charactersMap).insert(std::pair<GLchar, FontChar>(c, characterThing));
		}

		//Add the map to the map
		fontTextureMap.insert({ fontName, charactersMap });
	}
	else
	{
		return;
	}
}

std::vector<ModelMesh>* AssetLoader::CreateVecMesh(const char * modelPath, const char * modelName, Model* parentModel, GameObject* parent)
{
	std::vector<ModelMesh>* tempVecMesh = nullptr;

	std::map<std::string, std::vector<ModelMesh>*>::iterator it;

	it = vecMeshMap.find(modelName);

	if (it == vecMeshMap.end()) //If the model was not found
	{
		//tempVecMesh = new Model("Assets/Tank/Tank.obj", parent);
		parentModel->loadModel(modelPath);
		//Add vector of meshes to the map
		tempVecMesh = parentModel->GetVecMeshes();
		vecMeshMap.insert({ modelName, tempVecMesh});
	}
	else //If the model was found
	{
		//Use existing model
		tempVecMesh = it->second;
	}

	return tempVecMesh;
}

std::map <std::string, std::map<GLchar, FontChar>*>& AssetLoader::GetFontMap()
{
	std::map <std::string, std::map<GLchar, FontChar>*>& temp = fontTextureMap;
	return temp;
}
