#ifndef SONAR_SCANNER_H
#define SONAR_SCANNER_H

#define PEG_VALUE SensorValue[PegSensor]
#define ACCORDION_VALUE SensorValue[AccordionSensor]
#define LIGHT_VALUE SensorValue[LightSensor]
#define IS_LIGHT_WHITE (LIGHT_VALUE > WHITE_THRESHOLD_BOTTOM && LIGHT_VALUE < WHITE_THRESHOLD_TOP)
#define IS_LIGHT_BLACK (LIGHT_VALUE > BLACK_THRESHOLD_BOTTOM && LIGHT_VALUE < BLACK_THRESHOLD_TOP)

/* Each value of PegLevel is the distance the accordion sensor
 * has to reach to get there */
typedef enum {
	COLLAPSED = 8,
	BOTTOM    = 14,
	MIDDLE    = 14,    /* TODO: At the moment, since the accordion is leaning over too
	                              much, we cannot get the actual height for the middle peg */
	/* TOP    = 8  */  /* NOTE: We can't get the the third peg, but for safety reasons
	                              we'll just make it the bottom peg */
} PegLevel;

const PegLevel DEFAULT_PEG_PLACEMENT = BOTTOM;

const int BOARD_SEACRH_SPEED   = 20;
const int ALIGNMENT_SPEED      = 20;
const int RING_PLACEMENT_SPEED = 15;

// Values relavant to the light sensor
const int WHITE_THRESHOLD_TOP    = 32;
const int WHITE_THRESHOLD_BOTTOM = 28;

const int BLACK_THRESHOLD_TOP    = 22;
const int BLACK_THRESHOLD_BOTTOM = 18;

// Distances in centimeters for the sonic sensor
// TODO: We will need to get the actual values!
const int ACCORDION_DISTANCE =  30;
const int PLACE_RING_DISTANCE = 26;

bool isAutonomousRunning = false;
bool isLoggingSensorValues = false;

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

void setAccordionToPeg(PegLevel level)
{
	if (ACCORDION_VALUE == (int)level) {
		return;
	} else if (ACCORDION_VALUE > (int)level) {
		// Lower it if we are higher in the first place
		setAccordion(ACCORDION_LOW_SPEED_DOWN);
		waitUntilSensorLessThan(AccordionSensor, level);
		stopAccordion();
	} else if (ACCORDION_VALUE < (int)level) {
		// Now raise it to the correct level
		setAccordion(ACCORDION_LOW_SPEED_UP);
		waitUntilSensorGreaterThan(AccordionSensor, level);
		stopAccordion();
	}
}

void driveToBoard()
{
	int counter = 0;

	// NOTE: We assume we are in the right direction!
	while((!IS_LIGHT_BLACK && !IS_LIGHT_WHITE) || (PEG_VALUE > ACCORDION_DISTANCE)) {
		setPower(BOARD_SEACRH_SPEED);
		counter++;

		#if GLOBAL_LOGGING
			writeDebugStreamLine("[%i] -> %i | Status: Searching for black board", counter, LIGHT_VALUE);
		#endif
	}
	setPower(0);
}

void traceLine()
{
	int counter = 0;

	// Keep moving on the line until we reach the accordion distance
	while (PEG_VALUE > ACCORDION_DISTANCE) {
		if (IS_LIGHT_WHITE) {
			turn(LEFT, ALIGNMENT_SPEED, 0);
			waitUntilSensorLessThan(LightSensor, BLACK_THRESHOLD_BOTTOM);
			counter++;

			#if GLOBAL_LOGGING
				writeDebugStreamLine("[%i] Status: Moving to dark", counter);
			#endif
		} else if (IS_LIGHT_BLACK) {
			turn(RIGHT, ALIGNMENT_SPEED, 0);
			waitUntilSensorGreaterThan(LightSensor, WHITE_THRESHOLD_BOTTOM);
			counter++;

			#if GLOBAL_LOGGING
				writeDebugStreamLine("[%i] Status: Moving to bright", counter);
			#endif
		}
	}

	#if GLOBAL_LOGGING
		writeDebugStreamLine("[DONE] Status: Reached accordion distance");
	#endif
	setPower(0);
}

void placeRing()
{
	#if GLOBAL_LOGGING
		writeDebugStreamLine("[START] Status: Starting to place the ring");
		writeDebugStreamLine("Status: Setting accordion to the default placement");
	#endif

	// Raise up the accordion
	setAccordionToPeg(DEFAULT_PEG_PLACEMENT);

	#if GLOBAL_LOGGING
		writeDebugStreamLine("Status: Accordion set to default placement");
		writeDebugStreamLine("Status: Driving into the placement distance");
	#endif

	// Drive into the peg to place it
	setPower(RING_PLACEMENT_SPEED);
	waitUntilSensorLessThan(PegSensor, PLACE_RING_DISTANCE);
	setPower(0);

	#if GLOBAL_LOGGING
		writeDebugStreamLine("Status: Ring is (hopefully) now on the peg.");
		writeDebugStreamLine("Status: Lowering accordion to drop the ring.");
	#endif

	// Drop the ring
	setAccordionToPeg(BOTTOM - 5);

	#if GLOBAL_LOGGING
		writeDebugStreamLine("Status: Accordion has been lowed to drop the ring");
		writeDebugStreamLine("Status: Driving back out to collapse the accordion");
	#endif

	// Drive back out
	setPower(-RING_PLACEMENT_SPEED);
	waitUntilSensorGreaterThan(PegSensor, ACCORDION_DISTANCE);
	setPower(0);

	#if GLOBAL_LOGGING
		writeDebugStreamLine("Status: Finished driving back out");
		writeDebugStreamLine("Status: Collapsing accordion");
	#endif

	// Lower the accordion back down
	setAccordionToPeg(COLLAPSED);

	#if GLOBAL_LOGGING
		writeDebugStreamLine("Status: Finished collapsing accordion");
		writeDebugStreamLine("[FINISHED AUTONOMOUS!]");
	#endif
}

task logSensorValues()
{
	if (!isLoggingSensorValues) {
		isLoggingSensorValues = true;
		while (true) {
			nxtDisplayTextLine(5, "PegSen    = %i", SensorValue[PegSensor]);
			nxtDisplayTextLine(6, "AccordSen = %i", SensorValue[AccordionSensor]);
			nxtDisplayTextLine(7, "LightSen  = %i", SensorValue[LightSensor]);
		}
	}
}

task autonomous()
{
	driveToBoard();
	traceLine();
	//placeRing();
	setPower(0);
}

#endif
