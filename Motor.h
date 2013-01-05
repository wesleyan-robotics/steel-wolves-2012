#ifndef MOTOR_H
#define MOTOR_H

const float NORMAL_SPEED_FACTOR = 0.75;
const float FULL_SPEED_FACTOR = 1.0;
const float LOW_SPEED_FACTOR = 0.5;
const float AUTONOMOUS_SPEED_FACTOR = LOW_SPEED_FACTOR;
const float RING_PLACEMENT_SPEED_FACTOR = 0.2;

const int FULL_SPEED = 100;
const int NORMAL_SPEED = 100 * NORMAL_SPEED_FACTOR;
const int LOW_SPEED = 100 * LOW_SPEED_FACTOR;
const int AUTONOMOUS_SPEED = 100 * AUTONOMOUS_SPEED_FACTOR;
const int RING_PLACEMENT_SPEED = 100 * RING_PLACEMENT_SPEED_FACTOR;

/**
 * The diameter of the wheel in inches
 */
const float WHEEL_DIAMETER = 3.0;

/**
 * The circumfrence of the wheel in inches
 */
const float WHEEL_CIRCUMFRENCE = PI * WHEEL_DIAMETER;

/**
 * The digrees of 1 full rotation.
 */
const float ROTATION_DEG = 360.0;

/**
 * The ID of the left motor.
 */
int LeftMotor = -1;

/**
 * The ID of the right motor.
 */
int RightMotor = -1;

typedef enum {
	LEFT,
	RIGHT
} TurnDirection;

/**
 * Gets the direction from a horizontal value where
 * a positive value is going forward and
 * a negative value is going backwards.
 *
 * @param value The horizontal value representing the direction
 */
TurnDirection getDirection(int horizontal)
{
	if (horizontal < 0) {
		return LEFT;
	} else {
		return RIGHT;
	}
}

/**
 * Initializes the motors. Must be called before other functions are used.
 *
 * @param left The ID of the left motor.
 * @param right The ID of the right motor.
 */
void initMotors(int left, int right)
{
	LeftMotor = left;
	RightMotor = right;
}

/**
 * Sets the power of both motors.
 *
 * @param power The power to set.
 */
void setPower(int power)
{
	motor[RightMotor] = power;
	motor[LeftMotor] = power;
}

/**
 * Sets the power of each motor.
 *
 * @param left The power of the left motor.
 * @param right The power of the right motor.
 */
void setPower(int left, int right)
{
	motor[LeftMotor] = left;
	motor[RightMotor] = right;
}

/**
 * Resets the motor encoder and then sets its target value.
 *
 * @param motorID The motor being changed.
 * @param target The motor encoder's target in degress to reach.
 */
void setEncoderTarget(int motorID, int target)
{
	// Reset value and set target
	nMotorEncoder[motorID] = 0;
	nMotorEncoderTarget[motorID] = target;
}

/**
 * Run until both motor encoders' targets have been reached
 *
 * Runs at the power specified until the motor encoder's target has
 * been reached using both motors. Note that only the left motor encoder's
 * value is checked.
 *
 * @param power The power to run at while the target has not been reached.
 */
void reachTargetAtDualPower(int power)
{
	// Set speed and run to degrees
	setPower(power);
	// Wait until target is reached
	while(nMotorRunState[LeftMotor] != runStateHoldPosition) { }
	setPower(0);
}

/**
 * Run until the motor encoder's target is reached
 *
 * Runs at the power specified until the motor encoder's target has
 * been reached using the specified motor.
 *
 * @param motorID The motor to run.
 * @param power The power to run at while the target has not been reached.
 */
void reachTargetAtPower(int motorID, int power)
{
	// Set speed and run to degrees
	motor[motorID] = power;
	// Wait until target is reached
	while(nMotorRunState[motorID] != runStateHoldPosition) { }
	motor[motorID] = 0;
}

/**
 * Converts the distance in degress of motor rotations to the distance in inches.
 *
 * @param deg The amount in degress to convert.
 */
float convertDegToInches(float deg)
{
	return WHEEL_CIRCUMFRENCE / (ROTATION_DEG / deg);
}

/**
 * Converts the distance in inches to the distance in degress of motor rotations.
 *
 * @param inches The amount in inches to convert.
 */
float convertInchesToDeg(float inches)
{
	return (inches / WHEEL_CIRCUMFRENCE) * ROTATION_DEG;
}

/**
 * Move vertically in degrees
 *
 * @param power The power to move at.
 * @param deg The amount in degress of motor rotations to move.
 */
void moveInDeg(int power, float deg)
{
	setEncoderTarget(LeftMotor, round(deg));
	reachTargetAtDualPower(power);
}

/**
 * Move vertically in inches.
 *
 * @param power The power to move at.
 * @param inches The amount in inches to move.
 */
void moveInInches(int power, float inches)
{
	int deg = round(convertInchesToDeg(inches));
	moveInDeg(power, deg);
}

/**
 * Turns with a wheel idle.
 *
 * @param stopWheel The wheel that will be idle while turning.
 * @param deg The amount in degrees to turn.
 * @param power The power to turn with
 */
void pointTurn(TurnDirection dir, int deg, int power)
{
	int moveMotor;
	int stopMotor;

	if (dir == RIGHT)
	{
		stopMotor = RightMotor;
		moveMotor = LeftMotor;
	}
	else
	{
		stopMotor = LeftMotor;
		moveMotor = RightMotor;
    }

	setEncoderTarget(moveMotor, deg);
	motor[stopMotor] = 0;
	reachTargetAtPower(moveMotor, power);
}

void turn(TurnDirection dir, int power)
{
	int moveMotor;
	int stopMotor;

	if (dir == RIGHT)
	{
		stopMotor = RightMotor;
		moveMotor = LeftMotor;
	}
	else
	{
		stopMotor = RightMotor;
		moveMotor = RightMotor;
    }

    motor[stopMotor] = 0;
	motor[moveMotor] = power;
}

void turn(TurnDirection dir, int power, float percentage)
{
	int moveMotor;
	int stopMotor;

	if (dir == RIGHT)
	{
		stopMotor = RightMotor;
		moveMotor = LeftMotor;
	}
	else
	{
		stopMotor = RightMotor;
		moveMotor = RightMotor;
    }

    motor[stopMotor] = power * percentage;
	motor[moveMotor] = power;
}

/**
 * Turns with a wheel idle.
 *
 * @param ms The amount in milliseconds to turn
 * @param power The power to turn with
 */
void pointTurnTiming(TurnDirection dir, int ms, int power)
{
	turn(dir, power);
	wait1Msec(ms);
	setPower(0);
}

#endif
