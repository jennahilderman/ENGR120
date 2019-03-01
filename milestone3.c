#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in1,    infraFrontR,    sensorReflection)
#pragma config(Sensor, in2,    infraFrontL,    sensorReflection)
#pragma config(Sensor, in3,    infraRight,     sensorReflection)
#pragma config(Sensor, in4,    infraLeft,      sensorReflection)
#pragma config(Sensor, in5,    infraBack,      sensorReflection)
#pragma config(Sensor, dgtl1,  straightButton, sensorTouch)
#pragma config(Sensor, dgtl2,  turnButton,     sensorTouch)
#pragma config(Sensor, I2C_1,  movingI2C,      sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_2,  clawI2C,        sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port1,           speedMotorL,   tmotorVex393_HBridge, openLoop, encoderPort, I2C_1)
#pragma config(Motor,  port2,           speedMotorR,   tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port3,           clawMotor,     tmotorVex393_MC29, openLoop, encoderPort, I2C_2)


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
int findPosition(){
    if ((SensorValue[infraFrontR] <= (0.7*SensorValue[infraFrontL]) || SensorValue[infraFrontL] <= (0.7*SensorValue[infraFrontR])) && SensorValue[infraFrontL] < IR_SENSOR_THRESHOLD && SensorValue[infraFrontR] < IR_SENSOR_THRESHOLD){
        position = straight;
        wheelState = straight;
        return 0;
        }
    if (SensorValue[infraFrontL] >= (0.7*SensorValue[infraFrontR])) && SensorValue[infraFrontR] < IR_SENSOR_THRESHOLD){
        position = right;
        wheelState = right;
        return 1;
    }
    if (SensorValue[infraFrontR] >= (0.7*SensorValue[infraFrontL])) && SensorValue[infraFrontL] < IR_SENSOR_THRESHOLD){
        position = left;
        wheelState = left;
        return 2;
    }
    if (SensorValue[infraRight] < IR_SENSOR_THRESHOLD){
        position = right;
        wheelState = right;
        return 1;
        }
    if (SensorValue[infraLeft] < IR_SENSOR_THRESHOLD){
        position = left;
        wheelState = left;
        return 2;
        }
    if (SensorValue[infraBack] < IR_SENSOR_THRESHOLD){
        position = back;
        wheelState = left;
        return 2;
        }
    else{
        position = straight;
        wheelState = straight;
        return 0;
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
        while (findPosition() != 0){
            motor[turningMotor] = -turningSpeed;
        }
}

//Turns the wheels to the right
void turnRight(){
    resetMotorEncoder(turningMotor);
    motor[turningMotor] = turningSpeed;
        while (findPosition() != 0){
            motor[turningMotor] = turningSpeed;
        }
}


void useIRSensors(){
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


task main()
{



}
