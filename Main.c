#pragma config(Hubs,  S1, HTMotor,  HTMotor,  none,     none)
#pragma config(Sensor, S3,     magnetic,       sensorHiTechnicMagnetic)
#pragma config(Motor,  motorA,           ,             tmotorNXT, openLoop, encoder)
#pragma config(Motor,  motorB,           ,             tmotorNXT, openLoop, encoder)
#pragma config(Motor,  motorC,           ,             tmotorNXT, openLoop, encoder)
#pragma config(Motor,  mtr_S1_C1_1,     rightWheel,    tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C1_2,     leftWheel,     tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_1,     accordion,     tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_2,     motorG,        tmotorTetrix, openLoop)

//#include "HTMAG-driver.h"
#include "JoystickDriver.c"  //Include file to "handle" the Bluetooth messages.

const int DEADZONE = 5;
const int ACCORDION_SPEED = 7;

void initializeRobot()
{
	// Place code here to initialize servos to starting positions.
	// Sensors are automatically configured and setup by ROBOTC. They may need a brief time to stabilize.
	return;
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

void setPower(const int power)
{
	motor[leftWheel] = power;
	motor[rightWheel] = power;
}

void setPower(const int leftPower, const int rightPower)
{
	motor[leftWheel] = leftPower;
	motor[rightWheel] = rightPower;
}

void setDirectionFromJoystickOLD()
{
	int y1 = joystick.joy1_y1;
	int y2 = joystick.joy1_y2;

	setInDeadzone(&y1);
    setInDeadzone(&y2);
	setPower(convertJoystickToMotor(y1), convertJoystickToMotor(y2));
}

/* This uses the new method of driving where:
 * LEFT Joystick  - Moves forward and backward
 * RIGHT Joystick - Moves left and right
 */
void setDirectionFromJoystick()
{
	int lWheel;
	int rWheel;
	int vertical = joystick.joy1_y1;
	int horizontal = joystick.joy1_x2;

	if(isInDeadzone(vertical)) return;
	setInDeadzone(&horizontal);

	if(horizontal < 0) // left
	{
		lWheel = vertical + horizontal;
		rWheel = vertical - horizontal;
	} else { // right
		lWheel = vertical - horizontal;
		rWheel = vertical + horizontal;
	}

	setPower(lWheel, rWheel);
}

void setAccordionFromJoystick()
{
	// Increase 90 deg
	if (joy1Btn(1))
		motor[accordion] = ACCORDION_SPEED;
	else if (joy1Btn(2))
		motor[accordion] = -ACCORDION_SPEED;
	else
		motor[accordion] = 0;
}

task main()
{
	initializeRobot();

	waitForStart();
	while(true)
	{
		getJoystickSettings(joystick);

		setDirectionFromJoystick();
		setAccordionFromJoystick();
		displayJoystickDebug();
	}
}
