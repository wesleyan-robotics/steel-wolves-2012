#pragma config(Hubs,  S1, HTMotor,  HTMotor,  none,     none)
#pragma config(Sensor, S2,     SonarBottom,    sensorSONAR)
#pragma config(Sensor, S3,     SonarTop,       sensorSONAR)
#pragma config(Motor,  motorA,           ,             tmotorNXT, openLoop, encoder)
#pragma config(Motor,  motorB,           ,             tmotorNXT, openLoop, encoder)
#pragma config(Motor,  motorC,           ,             tmotorNXT, openLoop, encoder)
#pragma config(Motor,  mtr_S1_C1_1,     LeftWheel,     tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C1_2,     AccordionTop,  tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_1,     AccordionBottom, tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_2,     RightWheel,    tmotorTetrix, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

// NOTE: Include EVERYTHING before Joystick.h
#include "JoystickDriver.c"

#include "Motor.h"
#include "Accordion.h"
#include "Sonar.h"
#include "Joystick.h"

const bool TESTING_AUTONOMOUS = false;
const bool DRIVING_ENABLED    = false;
const bool ACCORDION_ENABLED  = true;

void initRobot()
{
    initMotors(LeftWheel, RightWheel);
    initAccordion(AccordionBottom, AccordionTop);
    initSonars(SonarBottom, SonarTop);
}

void startWithAutonomous()
{
	// TODO: Need to detect when we can control the robot with the joysticks
    //       so that we can return from this
	//startScan();

	while(true)
	{
		getJoystickSettings(joystick);
		if (DRIVING_ENABLED)   { setDirectionFromJoystick(); }
		if (ACCORDION_ENABLED) { setAccordionFromJoystick(); }
		displaySonarDebug();
	}
}

task main()
{
	initRobot();
	waitForStart();

	if (TESTING_AUTONOMOUS) {
		startScan();
		return;
	} else {
		startWithAutonomous();
	}
}
