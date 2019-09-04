#pragma once
#include "sprite.h"

class Level;

enum ButtonState
{
	BS_NONE,
	BS_HOVER,
	BS_PRESS,
	BS_RELEASE,
	BS_SPECIAL
};

class Button : public GameObject
{
public:
	Button(std::string _name, float _x, float _y, Level* _level);
	virtual ~Button();

	virtual void Initialise();
	virtual void Update(double dTime);
	virtual void Draw();

	void SetID(unsigned int _ID);
	void SetState(ButtonState _state);
	unsigned int GetID() const;
	ButtonState GetState() const;
	Texter* GetTexter() const;

private:
	Level * level;
	Texter * texter;
	std::string name;	//Name displayed on the button
	std::string font;
	unsigned int ID;	//Unique ID of button
	ButtonState state;	//State of the button
};