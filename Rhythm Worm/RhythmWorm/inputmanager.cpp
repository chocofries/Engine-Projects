#include "inputmanager.h"
#include "game.h"

InputManager::InputManager()
{
}

InputManager::~InputManager()
{
}

void InputManager::KeyboardDown(unsigned char _key, int x, int y)
{
	keyState[_key] = KEY_DOWN;
}

void InputManager::KeyboardUp(unsigned char _key, int x, int y)
{
	keyState[_key] = KEY_UP;
}

void InputManager::SpecialKeyDown(int _key, int x, int y)
{
	switch (_key)
	{
	case GLUT_KEY_LEFT:
	{
		arrowState[DIR_LEFT] = KEY_DOWN;
		break;
	}
	case GLUT_KEY_RIGHT:
	{
		arrowState[DIR_RIGHT] = KEY_DOWN;
		break;
	}
	case GLUT_KEY_UP:
	{
		arrowState[DIR_UP] = KEY_DOWN;
		break;
	}
	case GLUT_KEY_DOWN:
	{
		arrowState[DIR_DOWN] = KEY_DOWN;
		break;
	}
	}
}

void InputManager::SpecialKeyUp(int _key, int x, int y)
{
	switch (_key)
	{
	case GLUT_KEY_LEFT:
	{
		arrowState[DIR_LEFT] = KEY_UP;
		break;
	}
	case GLUT_KEY_RIGHT:
	{
		arrowState[DIR_RIGHT] = KEY_UP;
		break;
	}
	case GLUT_KEY_UP:
	{
		arrowState[DIR_UP] = KEY_UP;
		break;
	}
	case GLUT_KEY_DOWN:
	{
		arrowState[DIR_DOWN] = KEY_UP;
		break;
	}
	}
}

void InputManager::MouseClick(int button, int state, int x, int y)
{
	if (button >= 3)
	{
		return;
	}

	mouseState[button] = (state == GLUT_DOWN) ? KEY_DOWN : KEY_UP;
}

void InputManager::MouseWheel(int button, int dir, int x, int y)
{
	mouseWheelDir = dir;
}

void InputManager::UpdateKeys()
{
	mouseWheelDir = 0;
	arrowState[DIR_UP] = KEY_UP;
	arrowState[DIR_DOWN] = KEY_UP;
	arrowState[DIR_LEFT] = KEY_UP;
	arrowState[DIR_RIGHT] = KEY_UP;
	keyState['s'] = KEY_UP;
}

InputState InputManager::GetKeyState(unsigned char _key) const
{
	return keyState[_key];
}

InputState InputManager::GetMouseState(unsigned int _button) const
{
	return mouseState[_button];
}

InputState InputManager::GetArrowState(unsigned int _arrowDir) const
{
	return arrowState[_arrowDir];
}

int InputManager::GetMouseWheelDir() const
{
	return mouseWheelDir;
}

int InputManager::GetMouseX() const
{
	return mouseX;
}

int InputManager::GetMouseY() const
{
	return mouseY;
}

void InputManager::SetMouseX(int _x)
{
	mouseX = _x;
}

void InputManager::SetMouseY(int _y)
{
	mouseY = _y;
}
