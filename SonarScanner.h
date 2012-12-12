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
void initSonars(int lower, int higher)
{
	lowSensor = lower;
	highSensor = higher;
}

#endif
