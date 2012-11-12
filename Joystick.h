#ifndef JOYSTICK_H
#define JOYSTICK_H

const int DEADZONE = 5;

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
