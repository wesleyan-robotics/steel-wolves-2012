#ifndef JOYSTICK_H
#define JOYSTICK_H

const int DEADZONE = 5;

typedef enum {
	DPAD_UP = 0,
	DPAD_UP_RIGHT,
	DPAD_RIGHT,
	DPAD_DOWN_RIGHT,
	DPAD_DOWN,
	DPAD_DOWN_LEFT,
	DPAD_LEFT
} JoystickDPad;

typedef enum {
	BUTTON_X = 0,
	BUTTON_A,
	BUTTON_B,
	BUTTON_Y
} JoystickButtons;

int convertJoystickToMotor(const int power)
{
	return ((power * 100) / 127);
}

bool isInDeadzone(const int y)
{
	if (y < DEADZONE && y > -DEADZONE) return true;
	else return false;
}

void setInDeadzone(int* joystickValue)
{
	if (isInDeadzone(*joystickValue))
	{
		*joystickValue = 0;
	}
}

#endif
