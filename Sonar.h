#ifndef SONAR_SCANNER_H
#define SONAR_SCANNER_H

#define BOTTOM_DISTANCE SensorValue[LowSensor]
#define TOP_DISTANCE SensorValue[HighSensor]
#define FULLY_DETECTED detected(BOTTOM_DISTANCE) && detected(TOP_DISTANCE)

typedef enum {
	BOTTOM,
	MIDDLE,
	TOP
} PegLevel;

const PegLevel DEFAULT_PLACEMENT = MIDDLE;

// NOTE: The sonic sensor's distance is measured in cenimeters
const int ACCORDION_DISTANCE = 20;  // TODO: Actually know when to raise and lower the accordion
const int PLACE_RING_DISTANCE = 15; // TODO: Ditto

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

void waitSensor()
{
	// Slow down getting values from sensor
	wait1Msec(100);
}

bool isDetected(int value)
{
	return value < 255;
}

void placeRingOn(PegLevel level)
{
	// TODO: Write code using the new sonic sensor on the accordion
	// NOTE: We of cource cannot go to the third peg level
}

task sonarScan()
{
	// TODO: Write code for the new scanning method
}

void displaySonarDebug()
{
	nxtDisplayTextLine(0, "Peg Sensor      : %i", SensorValue[PegSensor]);
	nxtDisplayTextLine(1, "Accordion Sensor: %i", SensorValue[AccordionSensor]);
	nxtDisplayTextLine(2, "Light Sensor    : %i", SensorValue[LightSensor]);
}

#endif
