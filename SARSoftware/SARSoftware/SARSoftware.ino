#include <Zumo32U4.h>

#define QTR_THRESHOLD     100  // microseconds

Zumo32U4LineSensors lineSensors;
Zumo32U4Motors motors;

#define NUM_SENSORS 3
unsigned int lineSensorValues[NUM_SENSORS];

void setup() {
  // put your setup code here, to run once:
  
  Serial1.begin(9600);
  lineSensors.initThreeSensors();
}

void loop() {
  bool autoMode;
  char incomingByte = 0; // for incoming serial data
  if (Serial1.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial1.read();
    
  // put your main code here, to run repeatedly:
  borderPathing();
}

String borderPathing(){
  lineSensors.read(lineSensorValues);
    if (lineSensorValues[0] < QTR_THRESHOLD)
  {
    // If leftmost sensor detects line, reverse and turn to the
    // right.
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(REVERSE_DURATION);
    motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
    delay(TURN_DURATION);
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  }
  else if (lineSensorValues[NUM_SENSORS - 1] < QTR_THRESHOLD)
  {
    // If rightmost sensor detects line, reverse and turn to the
    // left.
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(REVERSE_DURATION);
    motors.setSpeeds(-TURN_SPEED, TURN_SPEED);
    delay(TURN_DURATION);
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  }
  else
  {
    // Otherwise, go straight.
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  }
}
