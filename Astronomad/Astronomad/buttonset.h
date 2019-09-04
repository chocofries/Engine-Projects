#pragma once
#include "button.h"

class Clock;

//Manages a set of buttons, and gives functionality to cycle through them
class ButtonSet
{
public:
	ButtonSet(Clock * _clock);
	~ButtonSet();

	void AddButton(Button* _button);
	void SelectButton(int ID);
	void NextButton();
	void PrevButton();
	void PressButton();
	void ReleaseButton();
	void ResetPressTime();

	void ResetButtonNum(); //Resets buttonNum to size of vector
	void SetButtonNum(int _num);
	void SetEveryIsActive(bool _isActive);
	void SetSingleIsActive(int ID, bool _isActive);
	void SetSingleText(int ID, std::string _str);
	void SetSingleColor(int ID, glm::vec3 col);
	void SetEnterPressed(bool _pressed);
	void SetSingleState(int ID, ButtonState _state);

	int GetCurrentID() const;
	bool GetEnterPressed() const;

private:
	Clock * clock;
	Button * currentButton;
	std::vector<Button*> vecButtons;
	int buttonNum;		//Number of buttons currently active (not necessarily amount in vector)
	int currentID;		//Current ID of the button
	float loopDelay;	//The delay before moving to the next button
	float pressTime;	//The amount of time the button has been pressed
	bool enterPressed;	//If the enter input is being pressed
};