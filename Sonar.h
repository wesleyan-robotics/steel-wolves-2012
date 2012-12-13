#ifndef SONAR_SCANNER_H
#define SONAR_SCANNER_H

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

int bottomDistance()  { return SensorValue[lowSensor]; }
int topDistance()     { return SensorValue[highSensor]; }
bool bottomDetected() { return bottomDistance() < 60 && topDistance() > 40; }
bool topDetected()    { return topDistance() < 100; }


void startScan()
{
	int distanceTime = 4000;

	// Go forward until bottom bar detected
	setPower(50);
	while (!bottomDetected()) { writeDebugStreamLine("%i", bottomDistance()); }
	setPower(0);

	// Turn to the left around 90 degrees
	ClearTimer(T1);
	startTurnTiming(LEFT, 20);
	while (true) {
		if (time1[T1] > (distanceTime / 2)) break;
		if (topDetected()) break;
	}

	// Turn to the right around 180 to scan
	ClearTimer(T1);
	startTurnTiming(RIGHT, 20);
	while (true) {
		if (time1[T1] > distanceTime) break;
		if (topDetected()) break;
	}

	setPower(0);
}

void displaySonarDebug()
{
	nxtDisplayTextLine(0, "Sonar Top   : %i", SensorValue[highSensor]);
	nxtDisplayTextLine(1, "Sonar Bottom: %i", SensorValue[lowSensor]);
}

#endif
