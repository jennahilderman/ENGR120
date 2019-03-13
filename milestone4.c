#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in1,    infraFront,     sensorReflection)
#pragma config(Sensor, dgtl1,  tooFarLED,      sensorDigitalOut)
#pragma config(Sensor, dgtl5,  justRightLED,   sensorDigitalOut)
#pragma config(Sensor, dgtl6,  tooCloseLED,    sensorDigitalOut)
#pragma config(Sensor, dgtl7,  button,         sensorTouch)
#pragma config(Sensor, dgtl8,  switchRight,    sensorTouch)
#pragma config(Sensor, dgtl9,  switchLeft,     sensorTouch)
#pragma config(Sensor, dgtl11, batInput,       sensorSONAR_cm)
#pragma config(Sensor, I2C_1,  movingI2C,      sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_2,  clawI2C,        sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port1,           speedMotorL,   tmotorVex393_HBridge, openLoop, encoderPort, I2C_1)
#pragma config(Motor,  port2,           speedMotorR,   tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port3,           clawMotor,     tmotorVex393_MC29, openLoop, encoderPort, I2C_2)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

//Some important constants
// How far it should back up
const		int retreat = -50;
// How fast it should move forward
const       int movingSpeed = 30;
// How fast it should turn
const		int turningSpeed = 20;
// How fast the claw should open
const		int openClawSpeed = 30;
// How far the claw should open
const		int openClawDistance = 122;
// What the ultrasonic range finder reads when it isnt finding anything
const       int tooFar = -1;
// how far away the robot should be from the beacon to make the connection
const       int justRight = 7;
// Set the LED to this to turn it off
const       int LIGHT_OFF = 0;
// Set the LED to this to turn it on
const       int LIGHT_ON = 1;
// The distance away from a wall to turn away from it
const		int wallDistance = 20;
// how far it should turn
const		int rotate = 100;

//declaring button_pushed to be  boolean
bool    button_pushed;
bool	right_pushed;
bool	left_pushed;

// an enumeration for the position of the robot
enum t_position {
    straight = 0,
    found,
	lost,
    right,
    wall,
    stopped,
    started
};

//an enumeration for the claw
enum t_claw{
	neutral = 0,
	open,
	close,
	backup,
};

// Initialize the enumerations
t_position position;
t_claw claw;
int count;
int done;


// Function to see if a button is pushed.
void monitorInput(){
    if(SensorValue(button) && !button_pushed){
        button_pushed = true;
    }
	if(SensorValue(switchRight) && !right_pushed){
		right_pushed = true;
	}
	if(SensorValue(switchLeft) && !left_pushed){
		left_pushed = true;
	}
}


// Perform processing of measurements.
// Should be called with rate of at least 20 Hertz for proper detection of puck.
int monitorLight()
{
	// Static variables are a special class of variables that maintain
	// their values between subsequent calls to a function.  The intialization
	// values are only stored in the variables when the function is first called.
	// After that, the values that were in the variable at the end of the last time
	// the function is called will be stored in the variable when the function
	// execution is started.
	static int minLevelIR1 = 1200;	// Minimum light level seen by IR sensor 1
	static int maxLevelIR1 = 0;			// Maximum light level seen by IR sensor 1
	static int diffLevelIR1 = 0;		// Delta between maximum and minimum seen in last 0.1 seconds

	int lightLevel1 = SensorValue[infraFront];

	// Check if 100 msecs have elapsed.
	if ( time1[T1] > 100 )  {

		// 100 msecs have elapsed.  Compute delta of light level.
		diffLevelIR1 = maxLevelIR1 - minLevelIR1;

		// Reset calculation for next 100 msecs.
		maxLevelIR1 = 0;
		minLevelIR1 = 1200;
		clearTimer(T1);

		}
	else {
		// Check for new minimum/maximum light levels.
		if ( lightLevel1 < minLevelIR1 ) {
			minLevelIR1 = lightLevel1;
		}
		else if ( lightLevel1 > maxLevelIR1 ) {
			maxLevelIR1 = lightLevel1;
		}
	}
	return(diffLevelIR1);
}


// The finction that finds the position of the robot in relation to the beacon and the walls of the arena
int findPosition(){

	// If the front IR reading is in the IR sensor threshold
	if ( monitorLight() >= 200 ){
		// Set position to found
		if (position != started){
			position = found;
			return 0;
		}
		//return 0
		return 0;
	}
	// If the front IR reading is out the IR sensor threshold and the back IR reading is out the IR sensor threshold but the ultrasonic range finder notices a wall
	if ( (monitorLight() < 200 && SensorValue[batInput] <= wallDistance) || right_pushed ){
		//Set position to wall
		if (position != started){
		position = wall;
		right_pushed = false;
		return 1;
	}
		//return 1
		right_pushed = false;
		return 1;
	}

	if(left_pushed){
		if(position != started){
			position = right;
			left_pushed = false;
			return 2;
		}
		left_pushed = false;
		return 2;
	}

	// If the front IR reading is out the IR sensor threshold and the back IR reading is out the IR sensor threshold but the ultrasonic range finder does not notices a wall
	if ( monitorLight() < 200 && (SensorValue[batInput] >= wallDistance || SensorValue[batInput] == tooFar)){
		//Set position to straight
		if (position != started){
			position = straight;
			return 3;
		}
		//return 3
		return 3;
	}
	//Should never get here
	return 4;
}

// function to stop the motors
void halt(){
	//turn off right motor
    motor[speedMotorR] = 0;
	//turn off left motor
    motor[speedMotorL] = 0;
}

// function to reverse the robot
void goBack(){
	// turn the right motor on and subtract 6 so the right and left go the same speed
    motor[speedMotorR] = movingSpeed-6;
	//turn the left motor on
    motor[speedMotorL] = -movingSpeed;
}

// function to drop the cable
void drop(){
	// reset the claw encoder
	resetMotorEncoder(clawMotor);
	// while the encoder is less then the open claw distance
	while(abs(getMotorEncoder(clawMotor)) <= openClawDistance){
		// turn on the claw motor
		motor[clawMotor] = -openClawSpeed;
	}
	// set claw to neutral
	claw = neutral;
}

// function to make the connection
void connection(){
	while(true){
		// Start of switch statement.
		switch (claw){
			// First case is the claw is still
			case(neutral):
				// Turn the motor off
				motor[clawMotor] = 0;
				// If the straight button is pushed
				if(SensorValue[batInput] <= justRight){
					claw = open;
				}
				// Break out of the switch statement.
				else{
					done = 1;
					button_pushed = false;
					position = started;
				}
				break;
			// Seccond case is the claw is opening.
			case(open):
				// Call the drop function to open the claw
				drop();
				// Change to the neutral state.
				claw = backup;
				// Break out of the switch statement.
				break;
			case(backup):
				// turn off the motor before backing up
				motor[clawMotor] = 0;
				wait1Msec(2000);
				// call the go back function to move backwards
				resetMotorEncoder(speedMotorL);
				//while the encoder is greater then retreat so it only backs up a little
				while(getMotorEncoder(speedMotorL) >= (5*retreat)){
					goBack();
  				}
  				// stop the motor
  				halt();
				// turn the too close LED on
				SensorValue[tooCloseLED] = LIGHT_ON;
				// turn the just right LED on
        		SensorValue[justRightLED] = LIGHT_ON;
				// turn the too far LED on
        		SensorValue[tooFarLED] = LIGHT_ON;
				wait1Msec(5000);
				//turn the too close LED off
				SensorValue[tooCloseLED] = LIGHT_OFF;
				// turn the just right LED off
        		SensorValue[justRightLED] = LIGHT_OFF;
				// turn the too far LED off
        		SensorValue[tooFarLED] = LIGHT_OFF;
				// set claw to neutral
				while(getMotorEncoder(clawMotor) > 0){
					motor[clawMotor] = openClawSpeed;
				}
				motor[clawMotor] = 0;
				claw = neutral;
				// break out of the switch statement
				break;
		}
		if(done == 1){
			done = 0;
			break;
		}
		//end of switch statement
	}
	//end of while loop
}

void lightLED(){
		//Checks to see if the robot is facing the beacon but too far away
    if(SensorValue(batInput) > justRight && monitorLight() >= 200 ){
		// turn the too far LED on
        SensorValue[tooFarLED] = LIGHT_ON;
		// turn the just right LED off
        SensorValue[justRightLED] = LIGHT_OFF;
		// turn the too close LED off
        SensorValue[tooCloseLED] = LIGHT_OFF;
    }
    // Checks to see if the robot is facing the beacon and is the proper distance away
    else if(SensorValue(batInput) <= justRight && monitorLight() >= 200 ){
		// turn the just right LED on
        SensorValue[justRightLED] = LIGHT_ON;
		// turn the too close LED off
        SensorValue[tooCloseLED] = LIGHT_OFF;
		// turn the too far LED off
        SensorValue[tooFarLED] = LIGHT_OFF;
    }
    else{
		// turn the too close LED off
        SensorValue[tooCloseLED] = LIGHT_OFF;
		// turn the just right LED off
        SensorValue[justRightLED] = LIGHT_OFF;
		// turn the too far LED off
        SensorValue[tooFarLED] = LIGHT_OFF;
    }
}

// A function that starts the motor on to go straight
void goStraight(){
	// turn right motor on +6 to make the right and  left go the same speed
    motor[speedMotorR] = -movingSpeed+6;
	// turn left motor on
    motor[speedMotorL] = movingSpeed;
}

// A function that turn the robot to the left
void goLeft(){
	//turn on the right motor
	motor[speedMotorR] = -turningSpeed;
	// turn on the left motor
	motor[speedMotorL] = -turningSpeed;
}

// A function that turns the robot to the right
void goRight(){
	// turn on the right motor
	motor[speedMotorR] = turningSpeed;
	// turn on the left motor
	motor[speedMotorL] = turningSpeed;
}



task main()
{
	// Start
	// Set button_ pushed to false
	button_pushed = false;
	// Set position to started
    position = started;
	// Start of while
    while (true){
		// Check to see if the button is pushed
    	monitorInput();
		monitorLight();
		lightLED();
		// Start of switch
        switch(position){

            // The started case
            case(started):
                // Stop the motor
                halt();
                // Reset the encoders
                resetMotorEncoder(clawMotor);
                resetMotorEncoder(speedMotorL);
				monitorInput();
                // When the button is pushed switch state
                if ( button_pushed ) {
                    // If button pushed, change state
						// While the IR sensors are not sencing anything and the encoder is less then 400
                		while(monitorLight() < 200 && getMotorEncoder(speedMotorL) <= 400){
                			// Turn the robot to the right
							goRight();
                		}
						// Stop the motor
                		halt();
						//Change the position to stopped
                		position = stopped;
						//find the actual position
                		findPosition();
                    // Clear flag to indicate button processed.
                    button_pushed = false;
                }
			//break out of the started case
            break;

			//The stopped case
            case(stopped):
				// stop the motor
            	halt();
            	// Reset the encoders
            	resetMotorEncoder(clawMotor);
            	resetMotorEncoder(speedMotorL);
				//find the position
            	findPosition();
			// break out of the stopped case
            break;

			// The straight case
            case(straight):
            	// Reset the encoder
				resetMotorEncoder(speedMotorL);
				//While there there is no wall of beacon sensed
            	while((SensorValue(batInput) >= wallDistance || SensorValue(batInput) == tooFar) && (!left_pushed && !right_pushed)) {
					monitorInput();
					monitorLight();
					//Go straight
                    goStraight();
                	}
				// Find the position
                findPosition();
			// Break out of the straight case
            break;

			//The found case
            case(found):
				//While the beacon is too far away
              	while((SensorValue(batInput) >= justRight || SensorValue(batInput) == tooFar) &&  (monitorLight() >= 200 && !left_pushed && !right_pushed)){
					monitorInput();
					monitorLight();
					// go straight
            	    goStraight();
          		}
          		halt();
				resetMotorEncoder(speedMotorL);
				// If the front IR sensor is still within the threshold
				if(left_pushed){
					findPosition();
					break;
				}
				if(right_pushed){
					findPosition();
					break;
				}
				if (monitorLight() < 200 && (SensorValue(batInput) >= justRight || SensorValue(batInput) == tooFar )){
					position = lost;
					break;
				}
				if (SensorValue(batInput) <= justRight && monitorLight() >= 200){
					//Wait a few seconds
                	wait1Msec(1000);
					//Make the connection
					connection();
    				//Change the position back to started so that the robot can do it again
									button_pushed = false;
									right_pushed = false;
									left_pushed = false;
                	position = started;
					break;
              	}
				findPosition();
				// break out of the found case
                break;

				//The lost case
				case(lost):
					monitorInput();
					monitorLight();
					//Reset the left motor encoder
					resetMotorEncoder(speedMotorL);
					//while the front IR sensor does not see the beacon
					count = 0;
					while (monitorLight() < 200){
						//While the encoder is greater or equal to -50
						while(getMotorEncoder(speedMotorL) >= -30-(20*count) && monitorLight() < 200 ){
							//Go left
							goLeft();
						}
						//Check to the right
						while(getMotorEncoder(speedMotorL) <= 30+(20*count) && monitorLight() < 200){
							//Go right
							goRight();
						}
						count++;
					}
					if (monitorLight() >= 200){
						position = found;
					}
					else{
						findPosition();
					}
				break;

				//The wall case
                case(wall):
					//Backup a little
                	while(getMotorEncoder(speedMotorL) >= retreat){
						goBack();
  					}
  					// stop the motor
  					halt();
                	wait1Msec(1000);
					//go left
                	resetMotorEncoder(speedMotorL);
					// while the robot is not facing the beacon and only rotates up to the rotate constant
    				while(monitorLight() < 200 && getMotorEncoder(speedMotorL) >= -rotate){
						//turn left
						goLeft();
					}
					// turn off the motors
  					halt();
  					wait1Msec(1000);
					//find position
  					right_pushed = false;
                	findPosition();
				// break out of the wall case
                break;

				//The wall case
                case(right):
					monitorInput();
					monitorLight();
					//Backup a little
                	while(getMotorEncoder(speedMotorL) >= retreat){
						goBack();
  					}
  					// stop the motor
  					halt();
                	wait1Msec(1000);
					//go left
                	resetMotorEncoder(speedMotorL);
					// while the robot is not facing the beacon and only rotates up to the rotate constant
    				while(monitorLight() < 200 && getMotorEncoder(speedMotorL) <= rotate){
						//turn left
						goRight();
					}
					left_pushed = false;
					// turn off the motors
  					halt();
  					wait1Msec(1000);
					//find position
                	findPosition();
				// break out of the wall case
                break;

		}
	}
}
