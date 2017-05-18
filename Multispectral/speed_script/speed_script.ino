// Adafruit Motor shield library
// copyright Adafruit Industries LLC, 2009
// this code is public domain, enjoy!

#include <AFMotor.h>

float targetWheelSpeed = 2.2;
int motorspeed = 0;
float numDiskSlots = 16;
float numDiskRevPerWheelRev = 33;
float lastSpeed = 0.0;
int lastChange = 0;
int MAX_INCREASE = 40;
AF_DCMotor motor(4);

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Motor test!");

  // turn on motor
  motor.setSpeed(00);
 
  // Setup encoders A/B
  pinMode(14,INPUT);
  pinMode(15,INPUT);
  pinMode(16,INPUT);
  pinMode(17,INPUT);
  pinMode(18,INPUT);
  pinMode(19,INPUT);
  motor.run(RELEASE);
}

void loop() {
  uint8_t i, j;
  char* line;
  
  Serial.println("=====Start loop=====");

  float timePerGap = getTime(1);
  float timePerDiskRev = timePerGap * numDiskSlots;
  float timePerWheelRev = timePerDiskRev * numDiskRevPerWheelRev;
  float wheelRevsPerSec;
  if(timePerWheelRev == 0)
    wheelRevsPerSec = 0;
  else
    wheelRevsPerSec= 1/timePerWheelRev;
  Serial.print("Current timePerGap/timePerDiskRev/timePerWheelRev/wheelRevsPerSec are: (");
  Serial.print(timePerGap,10);
  Serial.print("/");  
  Serial.print(timePerDiskRev,10);
  Serial.print("/");  
  Serial.print(timePerWheelRev,10);
  Serial.print("/");  
  Serial.print(wheelRevsPerSec,10);
  Serial.println(")");

  motorspeed = calcSpeed(wheelRevsPerSec);
  motor.run(FORWARD);
  motor.setSpeed(motorspeed);
  Serial.print("Changing motorspeed to ");
  Serial.print(motorspeed);
  Serial.println("");
  
  delay(1000);
}

float getTime(float maxSecondsRuntime) {
  float MAX_SAMPLE_TIME = maxSecondsRuntime * 1000000;
  float timeStart = micros();
  Serial.print("Reading values: ");
  int reading = digitalRead(16);
  Serial.print(reading);
  float timeFront = micros();
  float diff = timeFront - timeStart;
  while(reading == 0 && diff < MAX_SAMPLE_TIME) {
    reading = digitalRead(16);
    Serial.print(reading);
    timeFront = micros();
    diff = timeFront - timeStart;
    /*Serial.print("Runtime: ");
    Serial.println(diff);*/
  }
  if(diff >= MAX_SAMPLE_TIME) {
    Serial.println("] Passed max sample time, return 0");
    return 0.0;
  }
  timeFront = micros();
  reading = digitalRead(16);
  Serial.print(reading);
  while(reading == 1 && diff < MAX_SAMPLE_TIME) {
    timeFront = micros();
    reading = digitalRead(16);
    Serial.print(reading);
    diff = timeFront - timeStart;
  }
  if(diff >= MAX_SAMPLE_TIME) {
    Serial.println("] Passed max sample time, return 0");
    return 0.0;
  }
  reading = digitalRead(16);
  Serial.print(reading);
  float timeCurr = micros();
  while(reading == 0 && diff < MAX_SAMPLE_TIME) {
    timeCurr = micros();
    reading = digitalRead(16);
    Serial.print(reading);
    diff = timeCurr - timeStart;
  }
  if(diff >= MAX_SAMPLE_TIME) {
    Serial.println("] Passed max sample time, return 0");
    return 0.0;
  }
  float timeBack = micros();
  while(reading == 1 && diff < MAX_SAMPLE_TIME) {
    timeBack = micros();
    reading = digitalRead(16);
    Serial.print(reading);
    diff = timeBack - timeStart;
  }
  if(diff >= MAX_SAMPLE_TIME) {
    Serial.println("] Passed max sample time, return 0");
    return 0.0;
  }
  
  Serial.println("]");
  Serial.print("End/Beginning: ");
  Serial.print(timeBack);
  Serial.print("/");
  Serial.print(timeFront);
  Serial.println(")");
  return (timeBack - timeFront)/1000000;
}

int calcSpeed(float currentSpeed) {  
  float speedDiff = currentSpeed - lastSpeed;
  float potentialSpeed = speedDiff + currentSpeed;
  float speedDisparity = targetWheelSpeed - currentSpeed;
  Serial.print("SpeedDiff/potentialSpeed/lastChange/speedDisparity are: ");
  Serial.print(speedDiff,6);
  Serial.print("(");
  Serial.print(currentSpeed,6);
  Serial.print(" - ");
  Serial.print(lastSpeed,6);
  Serial.print(")/");
  Serial.print(potentialSpeed,6);
  Serial.print("/");
  Serial.print(lastChange);
  Serial.print("/");
  Serial.print(speedDisparity);
  Serial.println("");
  if(abs(speedDisparity) > 0.3) {
    if(speedDisparity > 0) {
      Serial.println("SpeedDisparity > 0.3, adding 30");
      lastChange = 30;
    }
    else {
      Serial.println("SpeedDisparity < -0.3, subtracting 30");
      lastChange = -30;
    }
  }
  else {
    float percentageP = 0.0;
    float percentageD = 0.0;
    int changeP = 0;
    int changeD = 0;
    percentageP = (1.0 - (currentSpeed/targetWheelSpeed));
    percentageD = (1.0 - (potentialSpeed/targetWheelSpeed));
    changeP = 40 * percentageP;
    changeD = 10 * percentageD;
    lastChange = changeP + changeD;
    Serial.print("percentageP/percentageD/changeP/changeD/lastChange are: (");
    Serial.print(percentageP,6);
    Serial.print("/");
    Serial.print(percentageD,6);
    Serial.print("/");
    Serial.print(lastSpeed,6);
    Serial.print("/");
    Serial.print(changeP);
    Serial.print("/");
    Serial.print(changeD);
    Serial.print("/");
    Serial.print(lastChange);
    Serial.println(")");
  }
  lastSpeed = currentSpeed;
  return motorspeed + lastChange;  
}
