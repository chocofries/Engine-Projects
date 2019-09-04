#include "button.h"
#include "level.h"

Button::Button(std::string _name, float _x, float _y, Level* _level)
{
	this->level = _level;
	VBO = level->GetVBO();
	levelCamera = level->GetCamera();
	inputManager = level->GetInputManager();
	shaderLoader = level->GetShaderLoader();
	assetLoader = level->GetTextureLoader();
	vecTexts = level->GetVecTexts();
	clock = level->GetClock();
	x = _x;
	y = _y;
	xRot = 0.0f;
	yRot = 1.0f;
	zRot = 0.0f;
	name = _name;
	ID =  0;
	state = BS_NONE;
	font = "Assets/Fonts/nasalization-rg.ttf";
	isActive = true;
	Initialise();
}

Button::~Button()
{
}

void Button::Initialise()
{
	texter = new Texter(name, font, glm::vec3(x, y, z), shaderLoader, assetLoader, levelCamera, true, false);
	texter->SetScale(0.5f);
	texter->SetColor(glm::vec3(0.8f,0.8f,0.8f));
	vecTexts->push_back(texter);
}

void Button::Update(double dTime)
{
	texter->SetPosition(glm::vec3(x, y, z));
	
	switch (state)
	{
	case BS_NONE:
	{
		texter->SetScale(0.5f);
		texter->SetColor(glm::vec3(0.8f, 0.8f, 0.8f));
		break;
	}
	case BS_HOVER:
	{
		texter->SetScale(0.6f);
		texter->SetColor(glm::vec3(1.0f, 1.0f, 1.0f));
		break;
	}
	case BS_PRESS:
	{
		texter->SetScale(0.4f);
		texter->SetColor(glm::vec3(0.6f, 0.6f, 0.6f));
		break;
	}
	case BS_RELEASE:
	{
		texter->SetScale(0.4f);
		texter->SetColor(glm::vec3(0.6f, 0.6f, 0.6f));
		state = BS_HOVER;
		break;
	}
	case BS_SPECIAL:
	{
		texter->SetScale(0.6f);
		texter->SetColor(glm::vec3(0.4f, 0.9f, 1.0f));
		break;
	}
	default:
		break;
	}
}

void Button::Draw()
{
}

void Button::SetID(unsigned int _ID)
{
	ID = _ID;
}

void Button::SetState(ButtonState _state)
{
	state = _state;
}

unsigned int Button::GetID() const
{
	return ID;
}

ButtonState Button::GetState() const
{
	return state;
}

Texter * Button::GetTexter() const
{
	return texter;
}
