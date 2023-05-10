#include <Stepper.h>
#include "DHT.h"
#include <LiquidCrystal.h>
#include "uRTCLib.h"

const int stepsPerRevolution = 200;

Stepper myStepper = Stepper(stepsPerRevolution, 10, 12, 11, 13);
int motorPin = 9;

const int buttonDisable = 19;
const int buttonPinClock = 38;
const int buttonPinCounter = 39;
int buttonStateClock = 0;
int buttonStateCounter = 0;
int buttonStateDisable = 0;
volatile int flag = 0;
int idleInt = 0;
int change = 0;

int waterLevel = 0;                            
int Spin = A0;                                                      

LiquidCrystal lcd(3, 4, 5, 6, 7, 8);                            
DHT dht(2, DHT11); 
uRTCLib rtc(0x68);

int ledPinYellow = 17;                                         
int ledPinGreen = 14;                                          
int ledPinRed = 15;                                            
int ledPinBlue = 16;

int temperatureC = 0;
int temperatureF = 0;
int humidity = 0;

void recordings(){
  temperatureC = dht.readTemperature();  
  humidity = dht.readHumidity();
  waterLevel = analogRead(Spin);
}

void idleState(){
  digitalWrite(ledPinYellow, LOW);
  digitalWrite(ledPinBlue, LOW);
  digitalWrite(ledPinGreen, HIGH);
  digitalWrite(ledPinRed, LOW);
  if (change != 2) {
    Serial.println("The system is now in idle state");
    Serial.println("Current time:");
    rtc.refresh();
    Serial.print(rtc.hour());
    Serial.print(':');
    Serial.print(rtc.minute());
    Serial.print(':');
    Serial.println(rtc.second());
    idleLCD();
  }
  change = 2;
}

void idleLCD(){
  lcd.clear();
  lcd.setCursor(0,0); 
  lcd.print("Temp: "); 
  lcd.print(temperatureC);
  lcd.print((char)223);                  
  lcd.print("C");
  lcd.setCursor(0,1); 
  lcd.print("Humidity: "); 
  lcd.print(humidity);
  lcd.print("%");
}

void errorState(){
  if (change != 3){
    Serial.println("The system is now in Error state");
    Serial.println("Current time:");
    Serial.print(rtc.hour());
    Serial.print(':');
    Serial.print(rtc.minute());
    Serial.print(':');
    Serial.println(rtc.second());
  }
  analogWrite(motorPin,LOW);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("ERROR: WATER");                             
  lcd.setCursor(0,1);
  lcd.print("LEVEL IS TOO LOW");
  digitalWrite(ledPinYellow, LOW);
  digitalWrite(ledPinBlue, LOW);
  digitalWrite(ledPinGreen, LOW);
  digitalWrite(ledPinRed, HIGH);
  change = 3;  
}

void runningState(){
  if (change != 1){
    Serial.println("The system is now in running state");
    Serial.println("Current time:");
    Serial.print(rtc.hour());
    Serial.print(':');
    Serial.print(rtc.minute());
    Serial.print(':');
    Serial.println(rtc.second());
  } 
  lcd.clear(); 
  digitalWrite(motorPin, HIGH);
  digitalWrite(ledPinYellow, LOW);
  digitalWrite(ledPinBlue, HIGH);
  digitalWrite(ledPinGreen, LOW);
  digitalWrite(ledPinRed, LOW);                                          
  lcd.setCursor(0,0);
  lcd.print("MODE: RUNNING");
  lcd.setCursor(0,1); 
  lcd.print("Temp: "); 
  lcd.print(temperatureC);
  lcd.print((char)223);                  
  lcd.print("C");   
  change = 1;
}

void interruptFunction(){
  flag = 1;
}

void disabledState() {
  lcd.clear();
  lcd.setCursor(0,0);                                             
  lcd.print("SWAMP COOLER");
  lcd.setCursor(0,1);
  lcd.print("MODE: DISABLED");
  do{
    rtc.refresh();
    digitalWrite(ledPinYellow, HIGH);
    digitalWrite(ledPinBlue, LOW);
    digitalWrite(ledPinGreen, LOW);
    digitalWrite(ledPinRed, LOW);
    analogWrite(motorPin, LOW);
    if (idleInt == 0){
      Serial.println("The system is now in disabled state");
      Serial.println("Current time:");
      Serial.print(rtc.hour());
      Serial.print(':');
      Serial.print(rtc.minute());
      Serial.print(':');
      Serial.println(rtc.second());
      idleInt++;
    }
  }while(flag == 1);
}
