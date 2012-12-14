#ifndef SONAR_SCANNER_H
#define SONAR_SCANNER_H

/**
 * The ID of the lower sensor.
 */
int LowSensor = -1;

/**
 * The ID of the high sensor.
 */
int HighSensor = -1;

/**
 * Initializes the sonar sensors. Must be called before other functions are used.
 *
 * @param lower The ID of the lower sensor.
 * @param higher The ID of the higher sensor.
 */
void initSonars(int lower, int higher)
{
	LowSensor = lower;
	HighSensor = higher;
}

int bottomDistance()  { return SensorValue[LowSensor]; }
int topDistance()     { return SensorValue[HighSensor]; }
bool bottomDetected() { return bottomDistance() < 60 && topDistance() > 40; }
bool topDetected()    { return topDistance() < 100; }


void startScan()
{
	int distanceTime = 4000;

	// Go forward until bottom bar detected
	//setPower(10);
	while (true) {
		int b = bottomDistance();
		if (bottomDetected()) {
			writeDebugStreamLine("true - %i", b);
		} else {
			writeDebugStreamLine("false - %i", b);
		}
		wait1Msec(1000);
	}
	//setPower(0);

	return;

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
	nxtDisplayTextLine(0, "Sonar Top   : %i", SensorValue[HighSensor]);
	nxtDisplayTextLine(1, "Sonar Bottom: %i", SensorValue[LowSensor]);
}

#endif
