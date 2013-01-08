#ifndef JOYSTICK_H
#define JOYSTICK_H

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
	BUTTON_X  = 0,
	BUTTON_A  = 1,
	BUTTON_B  = 2,
	BUTTON_Y  = 3,
	BUTTON_LB = 4,
	BUTTON_RB = 5,
	BUTTON_LT = 6,
	BUTTON_RT = 7,
	BUTTON_BACK  = 8,
	BUTTON_START = 9
} JoystickButtons;

typedef enum {
	AUTONOMOUS_MODE = 0,
	TELEOP_MODE = 1
} MatchState;

typedef enum {
	MODE_ENABLED = 0,
	MODE_DISABLED = 1
} ModeStatus;

typedef struct {
	int left;
	int right;
	DriveDirection direction;
} DrivingState;

const int DEADZONE = 5;

float currentSpeedFactor = NORMAL_SPEED_FACTOR;

/**
 * Converts the joystick amount to a motor amount out of 100.
 *
 * @param power Joystick power to convert
 */
int convertJoystickToMotor(int power)
{
	if (power > 0) {
		return floor((power / 127.0) * 100.0);
	} else {
		return floor((power / 128.0) * 100.0);
	}
}

bool inDeadzone(const int x)
{
	return (x < DEADZONE && x > -DEADZONE);
}

void calculateDirectionNormal(int vertical, int horizontal, DrivingState *state)
{
	int lWheel = 0;
	int rWheel = 0;
	bool verticalInDeadzone = inDeadzone(vertical);
	bool horizontalInDeadzone = inDeadzone(horizontal);

	if (verticalInDeadzone && horizontalInDeadzone) {
		vertical = 0;
		horizontal = 0;
	} else if (verticalInDeadzone && !horizontalInDeadzone) {
		// Allow slight turning by moving just the horizontal amount
		vertical = 20;
	} else if (!verticalInDeadzone && horizontalInDeadzone) {
		// Make sure not to turn while going only forward when the horizontal
	    // is in the deadzone
		horizontal = 0;
	}

	DriveDirection dir = getDirection(horizontal);
	switch (dir) {
		case LEFT:
			// Left wheel will go slower then right
			lWheel = floor(vertical * (1 - (-horizontal / 100.0)));
			rWheel = vertical;
			break;
		case RIGHT:
			// Right wheel will go slower then the left
			lWheel = vertical;
			rWheel = floor(vertical * (1 - (horizontal / 100.0)));
			break;
		case STRAIGHT:
			lWheel = vertical;
			rWheel = vertical;
	}

	// Limit the speed to the current speed limit
	state->left  = floor(lWheel * currentSpeedFactor);
	state->right = floor(rWheel * currentSpeedFactor);
	state->direction = dir;
}

/* This uses the new method of driving where:
 * LEFT Joystick  - Moves forward and backward
 * RIGHT Joystick - Moves left and right
 */
void setDirectionFromJoystick()
{
	DrivingState state;
	int vertical = convertJoystickToMotor(joystick.joy1_y1);
	int horizontal = convertJoystickToMotor(joystick.joy1_x2);

	calculateDirectionNormal(vertical, horizontal, &state);
	setPower(state.left, state.right);

#if GLOBAL_LOGGING
    char* dirName = directionToString(state.direction);
	nxtDisplayTextLine(0, "dir = %s", dirName);
	nxtDisplayTextLine(1, "vert = %i", vertical);
	nxtDisplayTextLine(2, "hori = %i", horizontal);
	nxtDisplayTextLine(3, "lPower = %i", state.left);
	nxtDisplayTextLine(4, "rPower = %i", state.right);
#endif
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
void setSpeedFromJoystick()
{
	if (joy1Btn(BUTTON_LT)) {
		currentSpeedFactor = LOW_SPEED_FACTOR;
	} else if (joy1Btn(BUTTON_RT)) {
		currentSpeedFactor = FULL_SPEED_FACTOR;
	} else {
		currentSpeedFactor = NORMAL_SPEED_FACTOR;
	}

#if GLOBAL_LOGGING
	writeDebugStreamLine("setSpeedFromJoystick: currentSpeedFactor = %f", currentSpeedFactor);
#endif
}

void setTurnInPlaceFromJoystick()
{
	int lWheel = 0;
	int rWheel = 0;
	int vertical = convertJoystickToMotor(joystick.joy1_y1);

	DriveDirection dir = getDirection(vertical);
	switch (dir) {
		case LEFT:
			lWheel = -vertical;
			rWheel = vertical;
			break;
		case RIGHT:
			lWheel = vertical;
			rWheel = -vertical;
			break;
		case STRAIGHT:
			lWheel = 0;
			rWheel = 0;
			break;
	}

	setPower(lWheel, rWheel);
}

void teleop()
{
#if GLOBAL_DRIVING
	setSpeedFromJoystick();

	//if (joy1Btn(BUTTON_A)) {
	//	setTurnInPlaceFromJoystick();
	//} else {
		setDirectionFromJoystick();
	//}
#endif

#if GLOBAL_ACCORDION
	setAccordionFromJoystick();
#endif

#if GLOBAL_LOGGING
	writeDebugStreamLine("Teleop called");
#endif
}

void run()
{
#if GLOBAL_LOGGING
	StartTask(logSensorValues);
#endif

#if (defined(NXT) || defined(TETRIX)) && (_TARGET == "Robot") && !defined(NaturalLanguage)
	while(true) {
		getJoystickSettings(joystick);

		#if GLOBAL_LOGGING && 0
			writeDebugStreamLine("UserMode: %i", joystick.UserMode);
			writeDebugStreamLine("StopPgm : %i", joystick.StopPgm);
		#endif

		if ((joystick.UserMode == (bool)AUTONOMOUS_MODE) && GLOBAL_AUTONOMOUS) {
			if (joystick.StopPgm == (bool)MODE_ENABLED) {
				#if GLOBAL_LOGGING && 0
					writeDebugStreamLine("Autonomous in enabled state");
				#endif

				if (!isAutonomousRunning) {
					StartTask(autonomous);
					isAutonomousRunning = true;
				}
			}
		} else if ((joystick.UserMode == (bool)TELEOP_MODE) && GLOBAL_TELEOP) {
			if (joystick.StopPgm == (bool)MODE_ENABLED) {
				#if GLOBAL_LOGGING
					writeDebugStreamLine("Teleop in enabled state");
				#endif

				if (isAutonomousRunning) {
					StopTask(autonomous);
					isAutonomousRunning = false;
				}

				teleop();
			}
		}
	}
#elif (defined(NXT) || defined(TETRIX)) && (_TARGET == "Emulator")
	#error "run() was called but the joystick code does not compile properly on the emulator. Run on the robot instead."
#else
	#error "This platform does not support the joystick code. How you managed to get this error, I don't know."
#endif
}

#endif
