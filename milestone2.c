#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in1,    infraFrontR,    sensorReflection)
#pragma config(Sensor, in2,    infraFrontL,    sensorReflection)
#pragma config(Sensor, in3,    infraRight,     sensorReflection)
#pragma config(Sensor, in4,    infraLeft,      sensorReflection)
#pragma config(Sensor, in5,    infraBack,      sensorReflection)
#pragma config(Sensor, dgtl1,  button,        sensorTouch)
#pragma config(Sensor, I2C_1,  turningI2C,     sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port1,           speedMotorR,   tmotorVex393_HBridge, openLoop)
#pragma config(Motor,  port2,           speedMotorL,   tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port3,           turningMotor,  tmotorVex393_MC29, openLoop, encoderPort, I2C_1)
#pragma config(Motor,  port4,           craneMotor,    tmotorVex393_MC29, openLoop, encoderPort, I2C_1)

const   int IR_SENSOR_THRESHOLD = 1000 ;

bool button_pushed;


enum t_wheelState {
    straight = 0,
    left,
    right,
    stop
};

enum t_position {
    straight = 0,
    left,
    right,
    back,
};

t_position position;


t_wheelState wheelState;

void monitorInput(){
    if(SensorValue(button) && !button_pushed){
        button_pushed = true;
    }
}

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

task main(){
    while (true){
        monitorInput();
        switch(wheelState){
            case(stop):
                motor[speedMotorR] = 0;
                motor[speedMotorL] = 0;
                if ( button_pushed ) {
                    // If button1 pushed, change to the MOTOR_RUNNING state.
                    findPosition();
                    // Clear flag to indicate button 1 processed.
                    button_pushed = false;
                }
                break;
            case(straight):
                motor[speedMotorR] = 50;
                motor[speedMotorL] = 50;
                if (getMotorEncoder(speedMotorR) >= 3000 && getMotorEncoder(speedMotorL) >= 3000){
                    wheelState = stop;
                }
                break;
            case(right):
                motor[turningMotor] = 50;
                while (getMotorEncoder(turningMotor) <= 3000){
                    motor[turningMotor] = 50;
                }
                motor[speedMotorR] = 50;
                motor[speedMotorL] = 50;
                if (getMotorEncoder(speedMotorR) >= 3000 && getMotorEncoder(speedMotorL) >= 3000){
                    motor[speedMotorR] = 0;
                    motor[speedMotorL] = 0;
                    motor[turningMotor] = -50;
                    while (getMotorEncoder(turningMotor) <= -3000){
                        motor[turningMotor] = -50;
                    }
                    wheelState = stop;
                }
                break;
            case(left):
                motor[turningMotor] = -50;
                while (getMotorEncoder(turningMotor) <= -3000){
                    motor[turningMotor] = -50;
                }
                motor[speedMotorR] = 50;
                motor[speedMotorL] = 50;
                if (getMotorEncoder(speedMotorR) >= 3000 && getMotorEncoder(speedMotorL) >= 3000){
                    motor[speedMotorR] = 0;
                    motor[speedMotorL] = 0;
                    motor[turningMotor] = 50;
                    while (getMotorEncoder(turningMotor) <= 3000){
                        motor[turningMotor] = 50;
                    }
                    wheelState = stop;
                }
                break;

        }
    }
}
