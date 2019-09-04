#include "buttonset.h"
#include "clock.h"

ButtonSet::ButtonSet(Clock* _clock)
{
	loopDelay = 0.2f;
	pressTime = loopDelay;
	clock = _clock;
	enterPressed = true;
}

ButtonSet::~ButtonSet()
{
}

void ButtonSet::AddButton(Button * _button)
{
	//Add button to the vector
	vecButtons.push_back(_button);
	buttonNum = vecButtons.size();

	//Set the button's ID
	_button->SetID(buttonNum-1);

	//Set as the current button, if it is the first added
	if (currentButton == nullptr)
	{
		currentID = _button->GetID();
		currentButton = vecButtons[currentID];
		currentButton->SetState(BS_HOVER);
	}
}

void ButtonSet::SelectButton(int ID)
{
	if (!vecButtons.empty())
	{
		if (ID < buttonNum && ID >= 0)
		{
			currentID = ID;

			currentButton->SetState(BS_NONE);
			currentButton = vecButtons[currentID];
			currentButton->SetState(BS_HOVER);
		}
	}
}

void ButtonSet::NextButton()
{
	if (currentButton->GetState() != BS_PRESS && currentButton->GetState() != BS_RELEASE)
	{
		if (pressTime >= loopDelay)
		{
			if (!vecButtons.empty())
			{
				if (currentID + 1 < buttonNum)
				{
					currentID++;
				}
				else
				{
					currentID = 0;
				}

				currentButton->SetState(BS_NONE);
				currentButton = vecButtons[currentID];
				currentButton->SetState(BS_HOVER);
				pressTime = 0.0f;
			}
		}
		else
		{
			pressTime += (float)(clock->GetDeltaTick());
		}
	}
}

void ButtonSet::PrevButton()
{
	if (currentButton->GetState() != BS_PRESS && currentButton->GetState() != BS_RELEASE)
	{
		if (pressTime >= loopDelay)
		{
			if (!vecButtons.empty())
			{
				if (currentID > 0)
				{
					currentID--;
				}
				else
				{
					currentID = buttonNum - 1;
				}

				currentButton->SetState(BS_NONE);
				currentButton = vecButtons[currentID];
				currentButton->SetState(BS_HOVER);
				pressTime = 0.0f;
			}
		}
		else
		{
			pressTime += (float)(clock->GetDeltaTick());
		}
	}
}

void ButtonSet::PressButton()
{
	if (!vecButtons.empty())
	{
		//currentButton->SetState(BS_PRESS);
		enterPressed = true;
	}
}

void ButtonSet::ReleaseButton()
{
	if (!vecButtons.empty())
	{
		if (currentButton->GetState() == BS_PRESS)
		{
			//currentButton->SetState(BS_RELEASE);
			enterPressed = false;
		}
	}
}

void ButtonSet::ResetPressTime()
{
	pressTime = loopDelay;
}

void ButtonSet::ResetButtonNum()
{
	buttonNum = vecButtons.size();
}

void ButtonSet::SetButtonNum(int _num)
{
	buttonNum = _num;
}

void ButtonSet::SetEveryIsActive(bool _isActive)
{
	for (unsigned int i = 0; i < vecButtons.size(); i++)
	{
		vecButtons[i]->GetTexter()->SetIsActive(_isActive);
		vecButtons[i]->SetIsActive(_isActive);
	}
}

void ButtonSet::SetSingleIsActive(int ID, bool _isActive)
{
	vecButtons[ID]->GetTexter()->SetIsActive(_isActive);
	vecButtons[ID]->SetIsActive(_isActive);
}

void ButtonSet::SetSingleText(int ID, std::string _str)
{
	vecButtons[ID]->GetTexter()->SetText(_str);
}

void ButtonSet::SetSingleColor(int ID, glm::vec3 col)
{
	vecButtons[ID]->GetTexter()->SetColor(col);
}

void ButtonSet::SetEnterPressed(bool _pressed)
{
	enterPressed = _pressed;
}

void ButtonSet::SetSingleState(int ID, ButtonState _state)
{
	vecButtons[ID]->SetState(_state);
}

int ButtonSet::GetCurrentID() const
{
	return currentID;
}

bool ButtonSet::GetEnterPressed() const
{
	return enterPressed;
}
