#include <SoftwareSerial.h>
#include <ArduinoJson.h>
SoftwareSerial s(10, 11);
#define TRIGGER 5
#define ECHO    4

int flwPin = 2;   // Digital pin 2 assign to flow sensor
int soilPin1 = A0;   // Analog pin A0 assign to soil moisture sensor 1
int soilPin2 = A1;
int motorPin = 7;   // Digital upin 7 assign to motor/pump
int msvalue1 = 0;    // Soil Moisture Value for 1
int msvalue2 = 1;   //  Soil Moisture Value for 2
int valvePin1 = 6;   // Valve pin for valve 1
int valvePin2 = 9;   // Valve pin for valve 2
volatile int flow_frequency;    // Measures flow sensor pulses
float l_hour = 0;   // Calculated litres/hour
unsigned long currentTime;
int p = 0;
int led = 8; // Digital pin 8 assign to LED light
unsigned long cloopTime;
int per;
int per1;
int motorState = 0;
int valve1State = 0;
int valve2State = 0;
int BSM1 = 0;
int BSM2 = 0;
int ASM1 = 0;
int ASM2 = 0;
void flow () // Interrupt function
{
  flow_frequency++;
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(led, OUTPUT);
  pinMode(valvePin1, OUTPUT);
  pinMode(valvePin2, OUTPUT);
  pinMode(soilPin1, INPUT); //set soilPin as INPUT for reading soil moisture
  pinMode(soilPin2, INPUT);
  s.begin(9600);

  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);

  pinMode(flwPin, INPUT);// set flwPin as INPUT for reading flow of frequency
  pinMode(motorPin, OUTPUT); // set motorPin as OUTPUT to give command for on or off water pump
  digitalWrite(flwPin, HIGH); // Optional Internal Pull-Up
  Serial.begin(9600);
  attachInterrupt(0, flow, RISING); // Setup Interrupt
  sei(); // Enable interrupts
  currentTime = millis();
  cloopTime = currentTime;
  //Serial.println("The system is started");
}

void loop()
{
  //// COde for water level
  long duration, distance;
  digitalWrite(TRIGGER, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER, LOW);
  duration = pulseIn(ECHO, HIGH);
  distance = (duration / 2) / 29.1; per1 = 100;
  // put your main code here, to run repeatedly:
  msvalue1 = analogRead(soilPin1); //read the soil moisture sensor1
  msvalue2 = analogRead(soilPin2); //read the soil moisture sensor2
  Serial.println(msvalue2);
  
  if (distance >= 13)
  {
    //Serial.println("Low water");
    digitalWrite(motorPin, HIGH); // water pump will turn off.
    motorState = 0;
    delay(1000);
    digitalWrite(led, HIGH);
    digitalWrite(valvePin1, HIGH);
    digitalWrite(valvePin2, LOW);
    valve1State = 0;
    valve2State = 0;
    //Serial.println("pump is off.");
  }
  else
  {
    digitalWrite(led, LOW);
    if (msvalue1 > 500 || msvalue2 > 500)
    {
      if (msvalue1 > 500)
      {
        BSM1 = msvalue1;
        //Serial.println("block 1 on");
        digitalWrite(valvePin1, LOW);
        valve1State = 1;
        delay(1000);
        digitalWrite(motorPin, LOW);
        motorState = 1;
      }
      else
      {
        digitalWrite(valvePin1, HIGH);
        valve1State = 0;
        ASM1 = msvalue1;
      }
      if (msvalue2 > 500)
      {
        //Serial.println("block 2 on");
        digitalWrite(valvePin2, HIGH);
        valve2State = 1;
        BSM2 = msvalue2;
        //Serial.println("pump on");
      }
      else
      {
        digitalWrite(valvePin2, LOW);
        valve2State = 0;
        ASM2 = msvalue2;
      }

      currentTime = millis();
      // Every second, calculate and print litres/hour
      if (currentTime >= (cloopTime + 3000))
      {
        cloopTime = currentTime; // Updates cloopTime
        // Pulse frequency (Hz) = 7.5Q, Q is flow rate in L/min.
        //l_hour = (flow_frequency * 60 / 7.5); // (Pulse frequency x 60 min) / 7.5Q = flowrate in L/hour
        l_hour=800;
        flow_frequency = 0; // Reset Counter
        
      }
    }
    else
    {
      digitalWrite(valvePin1, HIGH);
      valve1State = 0;
      digitalWrite(valvePin2, LOW);
      valve2State = 0;
      digitalWrite(motorPin, HIGH); // water pump will turn off.
      motorState = 0;
      //Serial.println("no need");
    }
  }
  Serial.println("---------------------------------------------------------");
  StaticJsonBuffer<1000> staticJsonBuffer;
  JsonObject& root = staticJsonBuffer.createObject();






  

  root["d1"] = motorState;
  root["d2"] = valve1State;
root["d3"] = valve2State;
  root["d4"] = msvalue1;
  root["d5"] = msvalue2;

  root["d10"] = distance;
  root["d11"]=l_hour;
  root.printTo(s);
 
  Serial.println();
  Serial.println(sizeof(root));
  Serial.println(root.size());
  Serial.println();
  if(s.overflow())
  {
    Serial.println("Overflow");
   }
  Serial.println("---------------------------------------------------------");
  delay(1000);
  s.flush();
}
