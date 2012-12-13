#ifndef MOTOR_H
#define MOTOR_H

/**
 * Identifies the wheel.
 */
typedef enum {
	LEFT_WHEEL,
	RIGHT_WHEEL
} Wheel;

typedef enum {
	RIGHT,
	LEFT
} TurnDirection;

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
int leftMotor = -1;

/**
 * The ID of the right motor.
 */
int rightMotor = -1;

/*
 * The ID of the motor that has the encoder on it
 */
int motorEncoder = -1;

/**
 * Initializes the motors. Must be called before other functions are used.
 *
 * @param left The ID of the left motor.
 * @param right The ID of the right motor.
 */
void initMotors(int left, int right)
{
	leftMotor = left;
	rightMotor = right;
}

/**
 * Sets the power of both motors.
 *
 * @param power The power to set.
 */
void setPower(int power)
{
	motor[rightMotor] = power;
	motor[leftMotor] = power;
}

/**
 * Sets the power of each motor.
 *
 * @param left The power of the left motor.
 * @param right The power of the right motor.
 */
void setPower(int left, int right)
{
	motor[leftMotor] = left;
	motor[rightMotor] = right;
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
	while(nMotorRunState[leftMotor] != runStateHoldPosition) { }
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
	setEncoderTarget(leftMotor, round(deg));
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
void pointTurn(Wheel stopWheel, int deg, int power)
{
	int moveMotor;
	int stopMotor;

	if (stopWheel == RIGHT_WHEEL)
	{
		stopMotor = rightMotor;
		moveMotor = leftMotor;
	}
	else
	{
		stopMotor = leftMotor;
		moveMotor = rightMotor;
    }

	setEncoderTarget(moveMotor, deg);
	motor[stopMotor] = 0;
	reachTargetAtPower(moveMotor, power);
}


void startTurnTiming(TurnDirection dir, int power)
{
	int moveMotor;
	int stopMotor;

	if (dir == RIGHT)
	{
		stopMotor = rightMotor;
		moveMotor = leftMotor;
	}
	else
	{
		stopMotor = leftMotor;
		moveMotor = rightMotor;
    }

    motor[stopMotor] = 0;
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
	startTurnTiming(dir, power);
	wait1Msec(ms);
	setPower(0);
}

#endif
