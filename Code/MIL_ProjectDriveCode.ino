#include "HUSKYLENS.h"
#include "SoftwareSerial.h"
#include <AFMotor.h>


AF_DCMotor motor1(1, MOTOR12_1KHZ);
AF_DCMotor motor2(2, MOTOR12_1KHZ);

const int in1 = 7;
const int in2 = 6;
const int ena = 9;


String voice = "living room";

HUSKYLENS huskylens;
SoftwareSerial mySerial(10, 11); // RX, TX
//HUSKYLENS green line >> Pin 10; blue line >> Pin 11
void printResult(HUSKYLENSResult result);

void setup() {
    Serial.begin(115200);
    mySerial.begin(9600);
    while (!huskylens.begin(mySerial))
    {
        Serial.println(F("Begin failed!"));
        Serial.println(F("1.Please recheck the \"Protocol Type\" in HUSKYLENS (General Settings>>Protocol Type>>Serial 9600)"));
        Serial.println(F("2.Please recheck the connection."));
        delay(100);
    }
}

void loop() {
    if (!huskylens.request()) Serial.println(F("Fail to request data from HUSKYLENS, recheck the connection!"));
    else if(!huskylens.isLearned()) Serial.println(F("Nothing learned, press learn button on HUSKYLENS to learn one!"));
    else if(!huskylens.available()) Serial.println(F("No block or arrow appears on the screen!"));
    else
    {
        Serial.println(F("###########"));
        while (huskylens.available())
        {
            HUSKYLENSResult result = huskylens.read();
            printResult(result);
        }    
    }
}

void printResult(HUSKYLENSResult result){
    if (result.command == COMMAND_RETURN_BLOCK){
        Serial.println(String()+F("Block:xCenter=")+result.xCenter+F(",yCenter=")+result.yCenter+F(",width=")+result.width+F(",height=")+result.height+F(",ID=")+result.ID+F(" livingroom"));
        driveBot(result);
    }
    else if (result.command == COMMAND_RETURN_ARROW){
        Serial.println(String()+F("Arrow:xOrigin=")+result.xOrigin+F(",yOrigin=")+result.yOrigin+F(",xTarget=")+result.xTarget+F(",yTarget=")+result.yTarget+F(",ID=")+result.ID+F("livingroom"));
    }
    else{
        Serial.println("Object unknown!");
    }
}

void follow(HUSKYLENSResult result)
{
  if(result.xCenter <= 140)
  {
   Serial.println(String()+F("left"));
   left();
  }
  else if (result.xCenter >= 200)
  {
   Serial.println(String()+F("right"));
   right();
  }
  else if ((result.xCenter>=140)&&(result.xCenter<=200))
  {   
    if(result.width<=40)
    {
      Serial.println(String()+F("follow"));
     forward();
    }
    else if(result.width>40)
    {
     Serial.println(String()+F("stop"));
     stop();
    }
  }
}

void driveBot(HUSKYLENSResult result)
{
  if(voice == "living room")
  {
    if(result.ID == 1)
    {
      Serial.println(String()+F("living room"));
      follow(result);
    }
  }
  if(voice == "my room")
  {
    int idtag = 0;
    Serial.println(String()+F("myroom"));
    if(result.ID == 1)
    {
      follow(result);
    }
    else if(result.width > 40)
    {
      int goR = 0;
      int goL = 0;

      Serial.println(String()+F("lookr"));
      goR = lookRight(result);
      delay(400);
      Serial.println(String()+F("lookl"));
      goL = lookLeft(result);
      delay(400);

      if(goR == 2)
      {
        idtag = 1;
      }
      else if (goL == 2)
      {
        idtag = 2;
      }
    }
    if (idtag == 1)
    {
      Serial.println(String()+F("1"));
      right();
      follow(result);
    }
    else if (idtag == 2)
    {
      Serial.println(String()+F("2"));
      left();
      follow(result);
    }
  }
}

int lookRight(HUSKYLENSResult result)
{
  int ID = result.ID;
  right();
  delay(400);
  return ID;
}

int lookLeft(HUSKYLENSResult result)
{
  int ID = result.ID;
  left();
  delay(400);
  return ID;
}

void forward()
{
  analogWrite(ena, 125);
  digitalWrite (in1, HIGH);
  digitalWrite (in2, LOW);
}

void right()
{
  analogWrite(ena, 125);
  digitalWrite (in1, HIGH);
  digitalWrite (in2, LOW);
  motor1.run(FORWARD);
  motor1.setSpeed(255);
  motor2.run(BACKWARD);
  motor2.setSpeed(255);
  delay(400);
  motor1.run(RELEASE);
  motor2.run(RELEASE);
}

void left()
{
  analogWrite(ena, 125);
  digitalWrite (in1, LOW);
  digitalWrite (in2, HIGH);
  motor1.run(BACKWARD);
  motor1.setSpeed(255);
  motor2.run(FORWARD);
  motor2.setSpeed(255);
  delay(400);
  motor1.run(RELEASE);
  motor2.run(RELEASE);
}

void stop()
{
  digitalWrite (in1, LOW);
  digitalWrite (in2, LOW);
}
