#pragma once
enum InputState
{
	KEY_UP,
	KEY_DOWN,
	KEY_UP_BEGIN,
	KEY_DOWN_BEGIN
};

enum MouseButton
{
	MOUSE_LEFT,
	MOUSE_MIDDLE,
	MOUSE_RIGHT
};

enum DirKey
{
	DIR_LEFT,
	DIR_UP,
	DIR_RIGHT,
	DIR_DOWN
};

class InputManager
{
public:
	InputManager();
	~InputManager();

	void KeyboardDown(unsigned char _key, int x, int y);
	void KeyboardUp(unsigned char _key, int x, int y);
	void SpecialKeyDown(int _key, int x, int y);
	void SpecialKeyUp(int _key, int x, int y);
	void MouseClick(int button, int state, int x, int y);
	void MouseWheel(int button, int dir, int x, int y);

	void UpdateKeys();

	InputState GetKeyState(unsigned char _key) const;
	InputState GetMouseState(unsigned int _button) const;
	InputState GetArrowState(unsigned int _arrowDir) const;
	int GetMouseWheelDir() const;
	int GetMouseX() const;
	int GetMouseY() const;

	void SetMouseX(int _x);
	void SetMouseY(int _y);

private:
	InputState keyState[255];
	InputState mouseState[3];
	InputState arrowState[4];

	bool keyIsBegin[255];
	int mouseWheelDir;

	int mouseX;
	int mouseY;

};