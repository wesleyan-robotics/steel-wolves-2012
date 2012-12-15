#ifndef JOYSTICK_H
#define JOYSTICK_H

const int DEADZONE = 5;
const float SPEED_LIMIT_FACTOR = 0.75;

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

bool inDeadzone(const int x)
{
	if (x < DEADZONE && x > -DEADZONE) return true;
	else return false;
}

/* This uses the new method of driving where:
 * LEFT Joystick  - Moves forward and backward
 * RIGHT Joystick - Moves left and right
 */
void setDirectionFromJoystick()
{
	int lWheel;
	int rWheel;
	int vertical = convertJoystickToMotor(joystick.joy1_y1);
	int horizontal = convertJoystickToMotor(joystick.joy1_x2);

	if (inDeadzone(vertical) && inDeadzone(horizontal))
	{
		setpower(0);
		return;
	} else if (!inDeadzone(horizontal)) {
			vertical = 5;
	}

	lWheel = (vertical + horizontal) * SPEED_LIMIT_FACTOR;
	rWheel = (vertical - horizontal) * SPEED_LIMIT_FACTOR;

	setPower(lWheel, rWheel);
}

void setAccordionFromJoystick()
{
	// Increase 90 deg
	switch (joystick.joy1_TopHat) {
		case DPAD_UP:
			raiseAccordion();
			break;
		case DPAD_DOWN:
			lowerAccordion();
			break;
		default:
			stopAccordion();
			break;
	}
}

void drive() {
	while(true)
	{
		getJoystickSettings(joystick);
		if (DRIVING_ENABLED)   { setDirectionFromJoystick(); }
		if (ACCORDION_ENABLED) { setAccordionFromJoystick(); }
		displaySonarDebug();
	}
}

void displayJoystickDebug()
{
	nxtDisplayTextLine(0, "Joy1: (%i, %i)", joystick.joy1_x1, joystick.joy1_y1);
	nxtDisplayTextLine(1, "Joy2: (%i, %i)", joystick.joy1_y2, joystick.joy1_x2);
	nxtDisplayTextLine(2, "Joy1: Buttons:");
	nxtDisplayTextLine(3, " 1:%i 2:%i 3:%i 4:%i", joy1Btn(1), joy1Btn(2), joy1Btn(3), joy1Btn(4));
	nxtDisplayTextLine(4, " 5:%i 6:%i 7:%i 8:%i", joy1Btn(5), joy1Btn(6), joy1Btn(7), joy1Btn(8));
	nxtDisplayTextLine(5, " 9:%i 10:%i 11:%i", joy1Btn(9), joy1Btn(10), joy1Btn(11));
	nxtDisplayTextLine(6, " 12:%i", joy1Btn(12));
	nxtDisplayTextLine(7, "Joy2 TopHat: %i", joystick.joy1_TopHat);
}
#endif
