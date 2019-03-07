#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in1,    infraFrontL,    sensorReflection)
#pragma config(Sensor, in2,    infraBack,      sensorReflection)
#pragma config(Sensor, in3,    infraFrontR,    sensorReflection)
#pragma config(Sensor, dgtl1,  tooFarLED,      sensorDigitalOut)
#pragma config(Sensor, dgtl2,  justRightLED,   sensorDigitalOut)
#pragma config(Sensor, dgtl3,  tooCloseLED,    sensorDigitalOut)
#pragma config(Sensor, dgtl4,  button,         sensorTouch)
#pragma config(Sensor, dgtl5,  batInput,       sensorSONAR_cm)
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
const		int openClawDistance = 40;
// What the ultrasonic range finder reads when it isnt finding anything
const       int tooFar = -1;
// how far away the robot should be from the beacon to make the connection
const       int justRight = 7;
// The IR sensor threshold
const		int IR_SENSOR_THRESHOLD = 450;//830
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

// an enumeration for the position of the robot
enum t_position {
    straight = 0,
    found,
    backwards,
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

// Function to see if a button is pushed.
void monitorInput(){
    if(SensorValue(button) && !button_pushed){
        button_pushed = true;
    }
}

// The finction that finds the position of the robot in relation to the beacon and the walls of the arena
int findPosition(){
	// If the front IR reading is in the IR sensor threshold
	if ( SensorValue[infraFrontL] < IR_SENSOR_THRESHOLD ){
		// Set position to found
		if (position != started){
			position = found;
			return 0;
		}
		//return 0
		return 0;
	}
	// If the back IR reading is in the IR sensor threshold
	if ( SensorValue[infraBack] < IR_SENSOR_THRESHOLD ){
		// Set position to backwards
		if (position != started){
		position = backwards;
		return 1;
	}
		//return 1
		return 1;
	}
	// If the front IR reading is out the IR sensor threshold and the back IR reading is out the IR sensor threshold but the ultrasonic range finder notices a wall
	if ( SensorValue[infraFrontL] > IR_SENSOR_THRESHOLD && SensorValue[infraBack] > IR_SENSOR_THRESHOLD && SensorValue[batInput] <= wallDistance ){
		//Set position to wall
		if (position != started){
		position = wall;
		return 2;
	}
		//return 2
		return 2;
	}
	// If the front IR reading is out the IR sensor threshold and the back IR reading is out the IR sensor threshold but the ultrasonic range finder does not notices a wall
	if ( SensorValue[infraFrontL] > IR_SENSOR_THRESHOLD && SensorValue[infraBack] > IR_SENSOR_THRESHOLD && (SensorValue[batInput] >= wallDistance || SensorValue[batInput] == tooFar)){
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
	//reset the encoder
	resetMotorEncoder(speedMotorL);
	//while the encoder is greater then retreat so it only backs up a little
	while(getMotorEncoder(speedMotorL) >= retreat){
		// turn the right motor on and subtract 6 so the right and left go the same speed
        motor[speedMotorR] = movingSpeed-6;
		//turn the left motor on
        motor[speedMotorL] = -movingSpeed;
  }
  // stop the motor
  halt();
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
				if ( button_pushed ){
					// Change to the open state
					claw = open;
				}
				// Break out of the switch statement.
				break;
			// Seccond case is the claw is opening.
			case(open):
				// Clear flag to indicate button processed
				button_pushed = false;
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
				goBack();
				// set claw to neutral
				claw = neutral;
				// break out of the switch statement
				break;
		}
		//end of switch statement
	}
	//end of while loop
}

void lightLED(){
		//set number to be the position number
		number = findPosition();
		//Checks to see if the robot is facing the beacon but too far away
    if(SensorValue(batInput) > justRight && number == 0){
		// turn the too far LED on
        SensorValue[tooFarLED] = LIGHT_ON;
		// turn the just right LED off
        SensorValue[justRightLED] = LIGHT_OFF;
		// turn the too close LED off
        SensorValue[tooCloseLED] = LIGHT_OFF;
    }
    // Checks to see if the robot is facing the beacon and is the proper distance away
    else if(SensorValue(batInput) <= justRight && number == 0){
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
		// Start of switch
        switch(position){

            // The started case
            case(started):
                // Stop the motor
                halt();
                // Reset the encoders
                resetMotorEncoder(clawMotor);
                resetMotorEncoder(speedMotorL);
                // When the button is pushed switch state
                if ( button_pushed ) {
                    // If button pushed, change state
                	resetMotorEncoder(speedMotorL);
						// While the IR sensors are not sencing anything and the encoder is less then 400
                		while(SensorValue[infraFrontL] > IR_SENSOR_THRESHOLD && SensorValue[infraFrontL] > IR_SENSOR_THRESHOLD && getMotorEncoder(speedMotorL) < 400){
                			// Turn the robot to the right
							goRight();
							lightLED();
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
				//Check to see if an LED should turn on
                lightLED();
			//break out of the started case
            break;

			//The stopped case
            case(stopped):
				// stop the motor
            	halt();
            	// Reset the encoders
            	resetMotorEncoder(clawMotor);
            	resetMotorEncoder(speedMotorL);
				//Check to see if an LED should turn on
            	lightLED();
				//find the position
            	findPosition();
			// break out of the stopped case
            break;

			// The straight case
            case(straight):
            	// Reset the encoder
				resetMotorEncoder(speedMotorL);
				//While there there is no wall of beacon sensed
            	while(SensorValue(batInput) >= wallDistance || SensorValue(batInput) == tooFar || SensorValue[infraFrontL] > IR_SENSOR_THRESHOLD) {
                    //Check to see is an LED should turn on
					lightLED();
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
              	while(SensorValue(batInput) >= justRight || SensorValue(batInput) == tooFar){
					//Check to see if an LED should turn on (The too far LED should turn on)
                	lightLED();
					// go straight
            	    goStraight();
          		}
          		halt();
				// If the front IR sensor is still within the threshold
          		if (SensorValue(batInput) >= justRight){
					//Check to see if an LED should turn on (The just right LED should turn on)
           	     	lightLED();
					//Wait a few seconds
                	wait1Msec(2000);
					//Make the connection
					connection();
    				//Change the position back to started so that the robot can do it again
                	position = started;
              	}
              	findPosition();

				// break out of the found case
                break;

				//The wall case
                case(wall):
                	//Check to see if an LED should turn on
                	lightLED();
					//Backup a little
                	goBack();
                	wait1Msec(1000);
					//go left
                	resetMotorEncoder(speedMotorL);
					// while the robot is not facing the beacon and only rotates up to the rotate constant
    				while(SensorValue[infraFrontL] > IR_SENSOR_THRESHOLD && getMotorEncoder(speedMotorL) >= -rotate){
						//turn left
						goLeft();
					}
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
