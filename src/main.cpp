#include <Arduino.h>

//--- PWM Configuration ---

int motor_L1 = 21;
int motor_L2 = 19;
int motor_R1 = 5;
int motor_R2 = 18;

const int freq = 120;
const int resolution = 8;

const int ledChannel_L1 = 1;
const int ledChannel_L2 = 2;
const int ledChannel_R1 = 3;
const int ledChannel_R2 = 4;

int dutyCycle = 255; // Maximum speed

// Sensor pins
int s_Left = 33;
int s_FrontLeft = 32;
int s_FrontRight = 35;
int s_Right = 34;
int ls_Left = 25;
int ls_Right = 26;

int starter = 23;

bool wentForward = false;

byte sensorState;

void setup()
{
  Serial.begin(115200);

  // Motor configuration
  pinMode(motor_L1, OUTPUT);
  ledcSetup(ledChannel_L1, freq, resolution);
  ledcAttachPin(motor_L1, ledChannel_L1);

  pinMode(motor_L2, OUTPUT);
  ledcSetup(ledChannel_L2, freq, resolution);
  ledcAttachPin(motor_L2, ledChannel_L2);

  pinMode(motor_R1, OUTPUT);
  ledcSetup(ledChannel_R1, freq, resolution);
  ledcAttachPin(motor_R1, ledChannel_R1);

  pinMode(motor_R2, OUTPUT);
  ledcSetup(ledChannel_R2, freq, resolution);
  ledcAttachPin(motor_R2, ledChannel_R2);

  // Sensor configuration
  pinMode(s_Left, INPUT);
  pinMode(s_FrontLeft, INPUT);
  pinMode(s_FrontRight, INPUT);
  pinMode(s_Right, INPUT);
  pinMode(ls_Left, INPUT);
  pinMode(ls_Right, INPUT);
  pinMode(starter, INPUT);
}

void stop()
{
  ledcWrite(ledChannel_L1, 0);
  ledcWrite(ledChannel_L2, 0);
  ledcWrite(ledChannel_R1, 0);
  ledcWrite(ledChannel_R2, 0);
}

void movement(String direction, int dutyCycle, uint32_t workTime = 0)
{
  if (direction == "forward")
  {
    ledcWrite(ledChannel_L1, dutyCycle);
    ledcWrite(ledChannel_L2, 0);

    ledcWrite(ledChannel_R1, dutyCycle);
    ledcWrite(ledChannel_R2, 0);
  }
  else if (direction == "left")
  {
    ledcWrite(ledChannel_L1, 0);
    ledcWrite(ledChannel_L2, dutyCycle);

    ledcWrite(ledChannel_R1, dutyCycle);
    ledcWrite(ledChannel_R2, 0);
  }
  else if (direction == "right")
  {
    ledcWrite(ledChannel_L1, dutyCycle);
    ledcWrite(ledChannel_L2, 0);

    ledcWrite(ledChannel_R1, 0);
    ledcWrite(ledChannel_R2, dutyCycle);
  }
  else if (direction == "backward")
  {
    ledcWrite(ledChannel_L1, 0);
    ledcWrite(ledChannel_L2, dutyCycle);

    ledcWrite(ledChannel_R1, 0);
    ledcWrite(ledChannel_R2, dutyCycle);
  }

  if (workTime > 0)
  {
    delay(workTime);
    stop();
  }
}

byte detect()
{
  // Read sensor states and pack them into a byte
  byte sensorStates = 0;

  // if (digitalRead(s_Left) == HIGH)
  //   sensorStates |= 0b10000000;

  if (digitalRead(s_FrontLeft) == HIGH)
    sensorStates |= 0b01000000;

  if (digitalRead(s_FrontRight) == HIGH)
    sensorStates |= 0b00100000;

  if (digitalRead(s_Right) == HIGH)
    sensorStates |= 0b00010000;

  // For debugging
  Serial.print("Sensor states: ");
  Serial.println(sensorStates, BIN);

  return sensorStates;
}

void loop()
{
start:
  if (digitalRead(starter) == HIGH)
  {
    if (digitalRead(ls_Left) == LOW)
    {
      movement("backward", 255, 130);
      Serial.println("LLL Izquierda ");
      goto start;
    }
    else if (digitalRead(ls_Right) == LOW)
    {
      movement("backward", 255, 130);
      Serial.println("LLL Derecha ");
      goto start;
    }

    sensorState = detect();
    if (sensorState == 0b01100000)
    {
      if (wentForward == false)
      {
        for (size_t i = 50; i < 200; i += 15)
        {
          movement("forward", i);
          wentForward = true;
        }
      }
      movement("forward", 255);
      goto start;
    }
    else if (sensorState == 0b01000000)
    {
      movement("left", 70);
      wentForward = false;
      goto start;
    }
    else if (sensorState == 0b00100000)
    {
      movement("right", 60);
      wentForward = false;
      goto start;
    }
    else if (sensorState == 0b10000000)
    {
      movement("left", 120);
      wentForward = false;
      goto start;
    }
    else if (sensorState == 0b00010000)
    {
      movement("right", 120);
      wentForward = false;
      goto start;
    }
    else
    {
      movement("forward", 13);
      wentForward = false;
      goto start;
    }
  }
  else
  {
    stop();
    wentForward = false;
  }
}

/*

Possible Directions:
  "forward"
  "left"
  "right"
  "backward"

Examples of using the movement function:
  movement("forward", 100, 2000);
  movement("right", 255);

  The third parameter is the work time and is not mandatory, but you can specify it if you need.
  After the work time, the robot will stop.

*/
