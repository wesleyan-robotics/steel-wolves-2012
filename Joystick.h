#ifndef JOYSTICK_H
#define JOYSTICK_H

const int DEADZONE = 5;

const float NORMAL_SPEED_FACTOR = 0.75;
const float FULL_SPEED_FACTOR = 1.0;
const float LOW_SPEED_FACTOR = 0.5;

float currentSpeed = NORMAL_SPEED_FACTOR;

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
	BUTTON_A = 1,
	BUTTON_B = 2,
	BUTTON_Y = 3,
	BUTTON_LEFT_BUMPER = 4,
	BUTTON_RIGHT_BUMPER = 5,
	BUTTON_LEFT_TRIGGER = 6,
	BUTTON_RIGHT_TRIGGER = 7
} JoystickButtons;

/**
 * Converts the joystick amount to a motor amount out of 100.
 *
 * @param power Joystick power to convert
 */
int convertJoystickToMotor(const int power)
{
	return ((power * 100) / 127);
}

bool inDeadzone(const int x)
{
	return (x < DEADZONE && x > -DEADZONE);
}

/* This uses the new method of driving where:
 * LEFT Joystick  - Moves forward and backward
 * RIGHT Joystick - Moves left and right
 */
void setDirectionFromJoystick()
{
	int lWheel = 0;
	int rWheel = 0;
	int vertical = convertJoystickToMotor(joystick.joy1_y1);
	int horizontal = convertJoystickToMotor(joystick.joy1_x2);

	if (inDeadzone(vertical) && inDeadzone(horizontal)) {
		setPower(0);
		return;
	} else if (inDeadzone(vertical) && !inDeadzone(horizontal)) {
		// Allow slight turning by moving just the horizontal amount
		vertical = 5;
	} else if (!inDeadzone(vertical) && inDeadzone(horizontal)) {
		// Make sure not to turn while going only forward when the horizontal
	    // is in the deadzone
		horizontal = 0;
	}

	TurnDirection dir = getDirection(horizontal);
	switch (dir) {
		case LEFT:
			// Left wheel will go slower then right
			lWheel = vertical;
			rWheel = vertical * (horizontal / 100);
			break;
		case RIGHT:
			// Right wheel will go slower then the left
			lWheel = vertical * (horizontal / 100);
			rWheel = vertical;
			break;
	}

	// Limit overall speed
	lWheel = lWheel * currentSpeed;
	rWheel = rWheel * currentSpeed;

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

/**
 * Sets the speed from the back buttons on the joystick.
 * The following buttons do the following actions:
 *
 * LEFT TRIGGER  | Low speed factor    | 50%
 * RIGHT TRIGGER | Full speed factor   | 100%
 * NONE          | Normal speed factor | 75%
 */
void setSpeedFromJoystick() {
	if (joy1Btn(BUTTON_LEFT_TRIGGER)) {
		currentSpeed = LOW_SPEED_FACTOR;
	} else if (joy1Btn(BUTTON_RIGHT_TRIGGER)) {
		currentSpeed = FULL_SPEED_FACTOR;
	} else {
		currentSpeed = NORMAL_SPEED_FACTOR;
	}
}

void setTurnInPlaceFromJoystick()
{
	int lWheel = 0;
	int rWheel = 0;
	int horizontal = convertJoystickToMotor(joystick.joy1_x1);

	TurnDirection dir = getDirection(horizontal);
	switch (dir) {
		case LEFT:
			lWheel = -horizontal;
			rWheel = horizontal;
			break;
		case RIGHT:
			lWheel = horizontal;
			rWheel = -horizontal;
			break;
	}

	setPower(lWheel, rWheel);
}

void drive()
{
	while(true) {
		getJoystickSettings(joystick);

		if (DRIVING_ENABLED) {
			setSpeedFromJoystick();
			if (joy1Btn(BUTTON_A)) {
				setTurnInPlaceFromJoystick();
			} else {
				setDirectionFromJoystick();
			}
		}

		if (ACCORDION_ENABLED) {
			setAccordionFromJoystick();
		}

		displaySonarDebug();
	}
}
#endif
