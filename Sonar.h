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

void waitSensor() { wait1Msec(100); } // Slow down getting values from sensor
bool detected(int value) { return value < 255; }

void waitSensor(int sensor, int min)
{
	while (SensorValue[sensor] > min) { }
	return;
}

void findBottom(int timeout)
{
	ClearTimer(T1);
	while (true) {
		if (time1[T1] > timeout) return;

		int b = BOTTOM_DISTANCE;
		if (detected(b)) {
			writeDebugStreamLine("true - %i", b);
			break;
		} else {
			writeDebugStreamLine("false - %i", b);
			break;
		}
		waitSensor();
	}
}

bool detectBothTurning(TurnDirection direction, int timeout, int power)
{
	ClearTimer(T1);
	turn(direction, power);
	while (true) {
		if (time1[T1] > timeout) return false;
		if (FULLY_DETECTED) return true;
		waitSensor();
	}
	return false;
}

void placeRingOn(PegLevel level)
{
	int s = LowSensor;

	// Get in position to raise the accordion
	setPower(10);
	waitSensor(s, ACCORDION_DISTANCE);
	setPower(0);
	raiseAccordion();
	wait1Msec(4000);
	stopAccordion();

	// Place the peg
	setPower(10);
	waitSensor(s, PLACE_RING_DISTANCE);

	// Backoff from the peg to lower the accordion
	setPower(-10);
	waitSensor(s, ACCORDION_DISTANCE);
	lowerAccordion();
	wait1Msec(2000);
	setPower(0);
}

void startScan()
{
	bool bothDetected = false;
	int turnTime = 4000;

	while (true) {
		// Go forward until bottom bar detected
		setPower(10);
		findBottom(5000);

		// Scan until we descover where both sensors match
		if (detectBothTurning(LEFT, turnTime / 2, 20)) {
			placeRingOn(DEFAULT_PLACEMENT);
		} else if (detectBothTurning(RIGHT, turnTime, 20)) {
			placeRingOn(DEFAULT_PLACEMENT);
		} else {
			// Reset position as nothing was found
			turn(LEFT, 20);
			wait1Msec(turnTime / 2);
		}
	}
}

void displaySonarDebug()
{
	nxtDisplayTextLine(0, "Sonar Top   : %i", SensorValue[HighSensor]);
	nxtDisplayTextLine(1, "Sonar Bottom: %i", SensorValue[LowSensor]);
}

#endif
