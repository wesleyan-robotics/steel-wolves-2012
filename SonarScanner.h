#ifndef SONAR_SCANNER_H
#define SONAR_SCANNER_H

#include "Motor.h"

/**
 * The ID of the lower sensor.
 */
int lowSensor = -1;

/**
 * The ID of the high sensor.
 */
int highSensor = -1;

/**
 * Initializes the sonar sensors. Must be called before other functions are used.
 *
 * @param lower The ID of the lower sensor.
 * @param higher The ID of the higher sensor.
 */
void initSonars(int low, int high)
{
	lowSensor = low;
	highSensor = high;
}

bool anyDetection()
{
    return (SensorValue[lowSensor] & SensorValue[highSensor]) == 255;
}

void sonarScan()
{
    setPower(50);
    while (!anyDetection()) { }
    setPower(0);
}

void displaySonarDebug()
{
	nxtDisplayTextLine(0, "Low Sonar : (%i)", SensorValue[lowSensor]);
	nxtDisplayTextLine(1, "High Sonar: (%i)", SensorValue[highSensor]);
}

#endif
