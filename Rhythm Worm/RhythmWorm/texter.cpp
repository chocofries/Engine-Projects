#include "texter.h"

Texter::Texter(std::string _text, std::string _font, glm::vec3 pos,
	ShaderLoader* _shdloader, TextureLoader* _tloader, Camera* _cam,
	bool _isUI, glm::vec3 color, float scale)
{
	text = _text;
	SetColor(color);
	SetScale(scale);
	SetPosition(pos);
	isUI = (_isUI);
	levelCamera = _cam;
	textureLoader = _tloader;
	isActive = true;

	GLfloat halfWidth = (GLfloat)WINDOW_WIDTH * 0.5f;
	GLfloat halfHeight = (GLfloat)WINDOW_HEIGHT * 0.5f;

	if (isUI) //If the text is in screen-space
	{
		isWorld = 0;
		proj = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight);
	}
	else //If the text is in world-space
	{
		isWorld = 1;
		proj = levelCamera->GetProjection();
		view = levelCamera->GetView();
	}

	program = _shdloader->CreateProgram("Assets/TextVert.vs", "Assets/TextFrag.fs", "TextVert0", "TextFrag0");

	FT_Library ft;
	FT_Face face;

	//Init the font lib
	if (FT_Init_FreeType(&ft) != 0)
	{
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	}

	//Load font as face
	if (FT_New_Face(ft, _font.c_str(), 0, &face) != 0)
	{
		std::cout << "ERROR::FREETYPE: Failed to Load font" << std::endl;
	}

	FT_Set_Pixel_Sizes(face, 0, 72);		//Set size to load glyph as
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);	//Disable byte-alignment restriction

											//Load the first 128 characters of the ASCII set

	textureLoader->CreateFontCharacterMap("Font0", face);
	std::map <std::string, std::map<GLchar, FontChar>*>& fontMap = textureLoader->GetFontMap();
	Characters = fontMap["Font0"];

	//Destroy FreeType objects once finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	//Configure VAO/VBO for texture quads
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//Get Uniform Locations
	textColorLoc = glGetUniformLocation(program, "textColor");
	texLoc = glGetUniformLocation(program, "tex");
	projLoc = glGetUniformLocation(program, "proj");
	viewLoc = glGetUniformLocation(program, "view");
	modelLoc = glGetUniformLocation(program, "model");
	isWorldLoc = glGetUniformLocation(program, "isWorld");
}

Texter::~Texter()
{
}

void Texter::Render()
{
	if (isActive)
	{
		if (!isUI) //If in world-space
		{
			////Position
			glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), position);

			//Rotation
			glm::vec3 rotationAxisZ = glm::vec3(0, 1.0f, 0);
			glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), rotationAxisZ);

			//Scale
			glm::vec3 objScale = glm::vec3(0.01f, 0.01f, 0.0f);
			glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), objScale);

			//Model Matrix
			model = translationMatrix * rotationZ * scaleMatrix;

			//View Matrix
			view = levelCamera->GetView();

			//Projection Matrix
			proj = levelCamera->GetProjection();
		}

		//Enable blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Activate corresponding render state
		glUseProgram(program);
		glUniform3f(textColorLoc, color.x, color.y, color.z);
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniform1i(isWorldLoc, isWorld);
		glBindVertexArray(VAO);

		//Iterate through the characters
		glm::vec3 textPos = position;
		for (std::string::const_iterator c = text.begin(); c != text.end(); c++)
		{
			FontChar ch = (*Characters)[*c];
			GLfloat xpos = textPos.x + ch.bearing.x * scale;
			GLfloat ypos = textPos.y - (ch.size.y - ch.bearing.y) * scale;
			GLfloat w = ch.size.x * scale;
			GLfloat h = ch.size.y * scale;

			//Update VBO for each character
			GLfloat vertices[6][4] = {
				{ xpos, ypos + h, 0.0, 0.0 },{ xpos, ypos, 0.0, 1.0 },{ xpos + w, ypos, 1.0, 1.0 },
			{ xpos, ypos + h, 0.0, 0.0 },{ xpos + w, ypos, 1.0, 1.0 },{ xpos + w, ypos + h, 1.0, 0.0 }
			};

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

			//Render the glyph texture over the quad
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, ch.TextureID);
			glUniform1i(texLoc, 0);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			textPos.x += (ch.advance >> 6) * scale; //Advance cursors for the next glyph
		}

		glUseProgram(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);
	}
}

void Texter::SetIsUI(bool _isUI)
{
	isUI = _isUI;
}

void Texter::SetCamera(Camera * _cam)
{
	levelCamera = _cam;
}

void Texter::SetIsActive(bool _isActive)
{
	isActive = _isActive;
}