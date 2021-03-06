#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in1,    infraFrontR,    sensorReflection)
#pragma config(Sensor, in2,    infraFrontL,    sensorReflection)
#pragma config(Sensor, in3,    infraRight,     sensorReflection)
#pragma config(Sensor, in4,    infraLeft,      sensorReflection)
#pragma config(Sensor, in5,    infraBack,      sensorReflection)
#pragma config(Sensor, dgtl1,  straightButton,        sensorTouch)
#pragma config(Sensor, dgtl2,  turnButton,        sensorTouch)
#pragma config(Sensor, I2C_1,  turningI2C,     sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port1,           speedMotorR,   tmotorVex393_HBridge, openLoop)
#pragma config(Motor,  port2,           speedMotorL,   tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port3,           turningMotor,  tmotorVex393_MC29, openLoop, encoderPort, I2C_1)
#pragma config(Motor,  port4,           craneMotor,    tmotorVex393_MC29, openLoop, encoderPort, I2C_1)

/*
This is the code for milestone 2
I really suck at naming things so if you think of better ones let me know
I also often dont do things as efficiently as possible so if you think of a better way please let me know
I put comments to indicate wether it is relevent or not to milestone 2
*/


//I also made a function to find the beacon so this is so it only runs the milestone 2 part
#define MILESTONE 2


// This is not used in milestone 2
const   int IR_SENSOR_THRESHOLD = 1000;

// These ones are
// We will have to test and fiddle with the values
const   int rotate = 300;
const   int meterForward = 3000;
const   int turnForward = 627;
const   int movingSpeed = 50;
const   int turningSpeed = 25;


bool straightButton_pushed;
bool turnButton_pushed;

// An enumeration for what direction the wheels are facing.
enum t_wheelState {
    straight = 0,
    left,
    right,
    stop
};

// An enumeration for what position the robot is in.
enum t_position {
    straight = 0,
    left,
    right,
    back,
};

// Initialize the enumerations
t_position position;
t_wheelState wheelState;


// Function to see if a button is pushed.
void monitorInput(){
    if(SensorValue(straightButton) && !straightButton_pushed){
        straightButton_pushed = true;
    }
    if(SensorValue(turnButton) && !turnButton_pushed){
        turnButton_pushed = true;
    }
}

// You guys can ignore this function
// Function finds the position of the robot
void findPosition(){
    if (SensorValue[infraFrontR] < IR_SENSOR_THRESHOLD || SensorValue[infraFrontL] < IR_SENSOR_THRESHOLD){
            position = straight;
            wheelState = straight;
        }
    if (SensorValue[infraRight] < IR_SENSOR_THRESHOLD){
          position = right;
          wheelState = right;
        }
    if (SensorValue[infraLeft] < IR_SENSOR_THRESHOLD){
          position = left;
          wheelState = left;
        }
    if (SensorValue[infraBack] < IR_SENSOR_THRESHOLD)
        {
          position = back;
          wheelState = left;
        }

}

// Stops the motor
void halt(){
    motor[speedMotorR] = 0;
    motor[speedMotorL] = 0;
}

// Starts the motor
void goStraight(){
    motor[speedMotorR] = movingSpeed;
    motor[speedMotorL] = movingSpeed;
}

// Turns the wheels to the left
void turnLeft(){
    resetMotorEncoder(turningMotor);
    motor[turningMotor] = -turningSpeed;
        while (getMotorEncoder(turningMotor) <= -rotate){
            motor[turningMotor] = -turningSpeed;
        }
}

//Turns the wheels to the right
void turnRight(){
    resetMotorEncoder(turningMotor);
    motor[turningMotor] = turningSpeed;
        while (getMotorEncoder(turningMotor) <= rotate){
            motor[turningMotor] = turningSpeed;
        }
}

// Ignore all of findTheBeacon
void findTheBeacon_1(){
    // Start
    wheelState = stop;
    while (true){
        monitorInput();
        switch(wheelState){
            // Start of switch

            case(stop):
                // Stop the motor
                halt();
                // Reset the encoders
                resetMotorEncoder(speedMotorR);
                resetMotorEncoder(speedMotorL);
                // When the button is pushed switch state
                if ( straightButton_pushed ) {
                    // If button pushed, change state
                    findPosition();
                    // Clear flag to indicate button processed.
                    straightButton_pushed = false;
                }
                break;

            case(straight):
                // When the robot if facing straight
                goStraight();
                // When the robot has moved a meter switched to the stopped state.
                if (getMotorEncoder(speedMotorR) >= meterForward && getMotorEncoder(speedMotorL) >= meterForward){
                    wheelState = stop;
                }
                break;

            case(right):
                // Turn the wheels to the right
                turnRight();
                // Start the motor
                goStraight();
                // When the robot is facing the beacon
                if (SensorValue[infraFrontR] < IR_SENSOR_THRESHOLD || SensorValue[infraFrontL] < IR_SENSOR_THRESHOLD){
                    // Stop the motor
                    halt();
                    // Turn the wheels back
                    turnLeft();
                    // Switch states
                    findPosition();
                }
                break;

            case(left):
                // Turn the wheels to the left
                turnLeft();
                // Start the motor
                goStraight();
                // When the robot is facing the beacon
                if (SensorValue[infraFrontR] < IR_SENSOR_THRESHOLD || SensorValue[infraFrontL] < IR_SENSOR_THRESHOLD){
                    // Stop the motor
                    halt();
                    // Turn the wheels back to the center
                    turnRight();
                    // Switch states
                    findPosition();
                }
                break;
						/*
            case(back):
                // Turn the wheels to the left
                turnLeft();
                // Start the motor
                goStraight();
                // When the robot is facing the beacon
                if (SensorValue[infraFrontR] < IR_SENSOR_THRESHOLD || SensorValue[infraFrontL] < IR_SENSOR_THRESHOLD){
                    // Stop the motor
                    halt();
                    // Turn the wheels back to the center
                    turnRight();
                    // Switch states
                    findPosition();
                }
                break;
                */
            // End of switch
        }
    }
}

// This is the milestone 2 code
void milestone_2(){
    // Start
    wheelState = stop;
    while (true){
        // Checks to see if a button is pushed
        monitorInput();
        switch(wheelState){
            // Start of switch
            case(stop):
                // Stop the motor
                halt();
                // Reset the encoders
                resetMotorEncoder(speedMotorR);
                resetMotorEncoder(speedMotorL);
                // When the button that makes it go straight is pushed
                if ( straightButton_pushed ) {
                    // Change to the straght state
                    wheelState = straight;
                    // Clear flag to indicate button processed.
                    straightButton_pushed = false;
                }
                // When the button that makes it turn is pushed
                if (turnButton_pushed){
                    // Change to the right state
                    wheelState = right;
                    // Clear flag to indicate button processed
                    turnButton_pushed = false;
                }
                break;

            case(straight):
                // When the robot if facing straight
                goStraight();
                // When the robot has moved a meter switched to the stopped state.
                if (getMotorEncoder(speedMotorR) >= meterForward && getMotorEncoder(speedMotorL) >= meterForward){
                    wheelState = stop;
                }
                break;

            case(right):
                // Turn the wheels to the right
                turnRight();
                // Start the motor
                goStraight();
                // When the robot is facing the beacon
                if (getMotorEncoder(speedMotorR) >= turnForward && getMotorEncoder(speedMotorL) >= turnForward){
                    // Stop the motor
                    halt();
                    // Turn the wheels back
                    turnLeft();
                    // Switch states
                    wheelState = stop;
                }
                break;
 		}
	}
}

task main()
{
    straightButton_pushed = turnButton_pushed = false;
    switch (MILESTONE){
        case 1:
            findTheBeacon_1();
            break;
        case 2:
            milestone_2();
            break;
        default: //should never get here.
    } // end switch

}// end main
