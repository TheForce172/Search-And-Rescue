#include <Wire.h>



#include <Zumo32U4.h>

#define QTR_THRESHOLD     100  // microseconds

Zumo32U4LineSensors lineSensors;
Zumo32U4Motors motors;
L3G gyro;

#define REVERSE_SPEED     200  // 0 is stopped, 400 is full speed
#define TURN_SPEED        200
#define FORWARD_SPEED     200
#define REVERSE_DURATION  200  // ms
#define TURN_DURATION     300  // ms
#define NUM_SENSORS 3
unsigned int lineSensorValues[NUM_SENSORS];
bool autoMode = false;

const int32_t turnAngle45 = 0x20000000;
int16_t gyroOffset;
uint16_t gyroLastUpdate = 0;
uint32_t turnAngle = 0;
int16_t turnRate;
void setup() {
  // put your setup code here, to run once:
  
  Serial1.begin(9600);
  turnSensorSetup();
  lineSensors.initThreeSensors();
    Serial1.println("Ping");
}

void loop() {
  int incomingByte = 0; // for incoming serial data
  if (Serial1.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial1.read();
    //Serial1.printlnln("Recived " + incomingByte);
    if(autoMode == false){
      manualOrder(incomingByte);
    }
    else {
        autoModeRun()
    }
  }
  // put your main code here, to run repeatedly:
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

void manualOrder(int order){
  switch(order){
    case 'F':
      motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
      Serial1.println('F');
      break;
      case 'L':
      turn('L');
      Serial1.println('L');
      break;
            case 'R':
      turn('R');
      Serial1.println('R');
      break;
      case 'S':
      motors.setSpeeds(0, 0);
      Serial1.println('S');
      break;
      case 'B':
      motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
      Serial1.println('B');
      break;
      case 'A':
      autoMode = true;
      Serial1.println('A');
      break;
      default:
       Serial1.println("Invalid Command");
       break;
  }
}

void autoModeRun(int order) {
  
}

//turn sensor setup and code from
//https://github.com/pololu/zumo-32u4-arduino-library/tree/master/examples/MazeSolver

  void turnSensorSetup()
{
  Wire.begin();
  gyro.init();

  // 800 Hz output data rate,
  // low-pass filter cutoff 100 Hz
  gyro.writeReg(L3G::CTRL1, 0b11111111);

  // 2000 dps full scale
  gyro.writeReg(L3G::CTRL4, 0b00100000);

  // High-pass filter disabled
  gyro.writeReg(L3G::CTRL5, 0b00000000);

  // Turn on the yellow LED in case the LCD is not available.
  ledYellow(1);

  // Delay to give the user time to remove their finger.
  // Calibrate the gyro.
  int32_t total = 0;
  for (uint16_t i = 0; i < 1024; i++)
  {
    // Wait for new data to be available, then read it.
    while(!gyro.readReg(L3G::STATUS_REG) & 0x08);
    gyro.read();

    // Add the Z axis reading to the total.
    total += gyro.g.z;
  }
  ledYellow(0);
  gyroOffset = total / 1024;

  // Display the angle (in degrees from -180 to 180) until the
  // user presses A.
  turnSensorReset();
}
// This should be called to set the starting point for measuring
// a turn.  After calling this, turnAngle will be 0.
void turnSensorReset()
{
  gyroLastUpdate = micros();
  turnAngle = 0;
}

// Read the gyro and update the angle.  This should be called as
// frequently as possible while using the gyro to do turns.
void turnSensorUpdate()
{
  // Read the measurements from the gyro.
  gyro.read();
  turnRate = gyro.g.z - gyroOffset;

  // Figure out how much time has passed since the last update (dt)
  uint16_t m = micros();
  uint16_t dt = m - gyroLastUpdate;
  gyroLastUpdate = m;

  // Multiply dt by turnRate in order to get an estimation of how
  // much the robot has turned since the last update.
  // (angular change = angular velocity * time)
  int32_t d = (int32_t)turnRate * dt;

  // The units of d are gyro digits times microseconds.  We need
  // to convert those to the units of turnAngle, where 2^29 units
  // represents 45 degrees.  The conversion from gyro digits to
  // degrees per second (dps) is determined by the sensitivity of
  // the gyro: 0.07 degrees per second per digit.
  //
  // (0.07 dps/digit) * (1/1000000 s/us) * (2^29/45 unit/degree)
  // = 14680064/17578125 unit/(digit*us)
  turnAngle += (int64_t)d * 14680064 / 17578125;
}

void turn(char dir)
{
  if (dir == 'S')
  {
    // Don't do anything!
    return;
  }

  turnSensorReset();


  switch(dir)
  {
  case 'B':
    // Turn left 125 degrees using the gyro.
    motors.setSpeeds(0, TURN_SPEED);
    while((int32_t)turnAngle < turnAngle45 * 3)
    {
      turnSensorUpdate();
    }
    break;

  case 'L':
    // Turn left 45 degrees using the gyro.
    motors.setSpeeds(0, TURN_SPEED);
    Serial1.println("Turning");
    while((int32_t)turnAngle < turnAngle45 * 2)
    {
      turnSensorUpdate();
    }
    break;

  case 'R':
    // Turn right 45 degrees using the gyro.
    motors.setSpeeds(TURN_SPEED, 0);
    Serial1.println("Turning");
    while((int32_t)turnAngle > -turnAngle45 * 2)
    {
      turnSensorUpdate();
    }
    break;

  default:
    // This should not happen.
    return;
  }

  // Turn the rest of the way using the line sensors.
  motors.setSpeeds(0, 0);
  Serial1.println("Turning complete");
  }
