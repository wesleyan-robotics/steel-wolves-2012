#ifndef JOYSTICK_H
#define JOYSTICK_H

// We are not bothing with autonomous since it does not work with the field control

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

typedef struct {
	int left;
	int right;
	DriveDirection direction;
} DrivingState;

typedef enum {
	AUTONOMOUS_MODE = 0,
	TELEOP_MODE = 1
} MatchState;

typedef enum {
	MODE_ENABLED = 0,
	MODE_DISABLED = 1
} ModeStatus;

const int DEADZONE = 5;
const int JOYSTICK_FETCH_DELAY_MS = 150;

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
	int lWheel;
	int rWheel;

	if (inDeadzone(vertical) && inDeadzone(horizontal)) {
		lWheel = 0;
		rWheel = 0;
	} else if (!inDeadzone(horizontal)) {
		vertical = 5;
	}

	lWheel = (vertical + horizontal) * currentSpeedFactor;
	rWheel = (vertical - horizontal) * currentSpeedFactor;

	// Limit the speed to the current speed limit
	state->left  = floor(lWheel);
	state->right = floor(rWheel);
	//state->direction = dir;
}

bool isJoystickInDeadzone()
{
	int vertical = convertJoystickToMotor(joystick.joy1_y1);
	int horizontal = convertJoystickToMotor(joystick.joy1_x2);

	if (inDeadzone(vertical) && inDeadzone(horizontal)) {
		return true;
	} else {
		return false;
	}
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

bool isAccordionButtonPressed()
{
	switch (joystick.joy1_TopHat) {
		case DPAD_UP:
		case DPAD_DOWN:
			return true;
		default:
			return false;
	}
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
#if !GLOBAL_TELEOP
    return;
#endif

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
	bool isAutonomousRunning = false;
	bool isTeleopRunning = false;
	bool teleopMode = false;

#if GLOBAL_LOGGING
	StartTask(logSensorValues);
#endif

#if (defined(NXT) || defined(TETRIX)) && (_TARGET == "Robot") && !defined(NaturalLanguage)
	while(true) {
		getJoystickSettings(joystick);
		teleopMode = joystick.UserMode == (bool)(TELEOP_MODE) && joystick.StopPgm == (bool)(MODE_ENABLED);

		// Run the current status
		if ((!isAutonomousRunning && !isTeleopRunning) && !teleopMode) {
			writeDebugStreamLine("Started autonomous mode task");
			StartTask(autonomous);
			isAutonomousRunning = true;
		} else if (!isAutonomousRunning && isTeleopRunning) {
			teleop();
		}

		// Switch to teleop mode if the joystick has been moved out of the deadzone
		if (teleopMode && isAutonomousRunning && !isTeleopRunning) {
			writeDebugStreamLine("Switching to teleop mode");
			StopTask(autonomous);
			isAutonomousRunning = false;
			isTeleopRunning = true;
		}

		wait1Msec(JOYSTICK_FETCH_DELAY_MS);
	}
#elif (defined(NXT) || defined(TETRIX)) && (_TARGET == "Emulator")
	#error "run() was called but the joystick code does not compile properly on the emulator. Run on the robot instead."
#else
	#error "This platform does not support the joystick code. How you managed to get this error, I don't know."
#endif
}

#endif
