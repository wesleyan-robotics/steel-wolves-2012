#ifndef SONAR_SCANNER_H
#define SONAR_SCANNER_H

#include "Accordion.h"

#define PEG_VALUE SensorValue[PegSensor]
#define ACCORDIAN_VALUE SensorValue[AccordionSensor]
#define LIGHT_VALUE SensorValue[LightSensor]
#define IS_ON_BOARD (LIGHT_VALUE > HIGH_THRESHOLD && LIGHT_VALUE < LOW_THRESHOLD)
#define IS_LIGHT_HIGH (LIGHT_VALUE > HIGH_THRESHOLD)
#define IS_LIGHT_LOW (LIGHT_VALUE < LOW_THRESHOLD)

typedef enum {
	COLLAPSED = 2,
	BOTTOM = 2,
	MIDDLE = 6,
	TOP = -1 // NOTE: We can't get the the third peg
} PegLevel;

const PegLevel DEFAULT_PEG_PLACEMENT = BOTTOM;

// NOTE: The sonic sensor's distance is measured in cenimeters
const int HIGH_THRESHOLD = 80;
const int LOW_THRESHOLD = 20;
const int ACCORDION_DISTANCE = 32;
const int PLACE_RING_DISTANCE = 10;

bool isAutonomousStarted = false;

/**
 * The ID of the bottom sonic sensor.
 */
int PegSensor = -1;

/**
 * The ID of the accordion sonic sensor.
 */
int AccordionSensor = -1;

/**
 * The ID of the light sonic sensor.
 */
int LightSensor = -1;

/**
 * Initializes the sonar sensors. Must be called before other functions are used.
 *
 * @param lower The ID of the lower sensor.
 * @param higher The ID of the higher sensor.
 */
void initSonars(int peg, int accordion, int light)
{
	PegSensor = peg;
	AccordionSensor = accordion;
	LightSensor = light;
}

void waitUntilSensorGreaterThan(int sensor, int value)
{
	while (true) {
		if (SensorValue[sensor] > value) {
			break;
		}
	}
}

void waitUntilSensorLessThan(int sensor, int value)
{
	while (true) {
		if (SensorValue[sensor] < value) {
			break;
		}
	}
}

void setAccordionTo(PegLevel level)
{
	// Check that we are not higher first
	if (ACCORDIAN_VALUE > (int)level) {
		lowerAccordion();
		waitUntilSensorLessThan(AccordionSensor, BOTTOM);
		stopAccordion();
	} else if (ACCORDIAN_VALUE < (int)level) {
		raiseAccordion();
		waitUntilSensorGreaterThan(AccordionSensor, BOTTOM);
		stopAccordion();
	}
}

void waitSensor()
{
	// Slow down getting values from sensor
	wait1Msec(100);
}

bool isDetected(int value)
{
	return value < 255;
}

void alignToLiftingPosition()
{
	// Keep moving on the line until we reach the accordion distance
	while (PEG_VALUE > ACCORDION_DISTANCE) {
		if (IS_LIGHT_HIGH) {
			turn(LEFT, AUTONOMOUS_SPEED, 0.20);
			waitUntilSensorLessThan(LightSensor, LOW_THRESHOLD);
		} else if (IS_LIGHT_LOW) {
			turn(RIGHT, AUTONOMOUS_SPEED, 0.20);
			waitUntilSensorGreaterThan(LightSensor, HIGH_THRESHOLD);
		}
	}

	// Raise up the accordion
	setAccordionTo(DEFAULT_PEG_PLACEMENT);

	// Drive into the peg to place it
	setPower(RING_PLACEMENT_SPEED);
	waitUntilSensorLessThan(PegSensor, PLACE_RING_DISTANCE);
	setPower(0);

	// Drive back out
	setPower(-RING_PLACEMENT_SPEED);
	waitUntilSensorGreaterThan(PegSensor, ACCORDION_DISTANCE);
	setPower(0);

	// Lower the accordion back down
	setAccordionTo(COLLAPSED);
}

task autonomous()
{
	// NOTE: We assume we are in the right direction!
	while(!IS_ON_BOARD) {
		setPower(AUTONOMOUS_SPEED);
	}

	alignToLiftingPosition();
}

void displaySonarDebug()
{
	nxtDisplayTextLine(0, "Peg Sensor      : %i", SensorValue[PegSensor]);
	nxtDisplayTextLine(1, "Accordion Sensor: %i", SensorValue[AccordionSensor]);
	nxtDisplayTextLine(2, "Light Sensor    : %i", SensorValue[LightSensor]);
}

#endif
