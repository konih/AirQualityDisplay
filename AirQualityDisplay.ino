#include "RotaryEncoder.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SFE_BMP180.h>
#include <dht.h>
#include "MQ135.h"
#include "MQ7.h"
#include <DS1302RTC.h>
#include <TimeLib.h>
#define BUZZER 8
#define ROTPINA 2
#define ROTPINB 3
#define MQ135PIN 15
#define MQ7PIN 16
#define MQ3PIN 17
#define PHOTOCELLPIN 14

// Humidity Sensor
dht DHT;
#define DHT11_PIN 10

// Pressure Sensor
SFE_BMP180 pressure;
#define ALTITUDE 355.0

// Real Time Clock
DS1302RTC RTC(7, 6, 5);

// LCD Display
LiquidCrystal_I2C lcd(0x3F, 20, 4);

// Rotary Encode
RotaryEncoder* RotaryEncoder::instance = new RotaryEncoder(2, 3);

// Gas Sensors
MQ135 mq135 = MQ135(MQ135PIN);
MQ7 mq7(A2, 5.0);

int position = 0;
//Most Recent Readings
double humidity = 0.0;
double temperatureDHT = 0.0;
double temperatureBMP = 0.0;
double absolutePressure = 0.0; //mb
double pressureSeaLevelCompensated = 0.0; //mb
double altitude = 0.0; //meters
float mq135ppm = 0.0;
float mq7ppm = 0.0;
int   mq3value = 0;
int   photocellReading = 0;
time_t t;

void lcdPrintTime() {
  // Display time centered on the upper line
  lcd.setCursor(5, 0);
  print2digits(hour());
  lcd.print(":");
  print2digits(minute());
  lcd.print(":");
  print2digits(second());
  lcd.setCursor(0,1);
  lcd.print(year());
  lcd.print("-");
  print2digits(month());
  lcd.print("-");
  print2digits(day());
  lcd.print(" ");
  lcd.print(dayShortStr(weekday()));

  
  // Warning!
  if (timeStatus() != timeSet) {
    lcd.setCursor(0, 1);
    lcd.print(F("RTC ERROR: SYNC!"));
  }
}

void lcdDisplayScreens(int position) {
  lcd.clear();
  lcd.setCursor(0, 0);
  switch (position) {
    case 0:
      lcdPrintTime();
      lcd.setCursor(0, 2);
      lcd.print(temperatureBMP);
      lcd.print((char)223);
      lcd.print("C, ");
      lcd.print(humidity);
      lcd.print(" %H20 ");
      lcd.setCursor(0, 3);
      lcd.print("Pressure:  ");
      lcd.print(absolutePressure);
      lcd.print(" mb");
      lcd.setCursor(0, 3);
      break;
    case 1:
      lcd.print("MQ135(CO2):");
      lcd.print(mq135ppm);
      lcd.print(" ppm");
      lcd.setCursor(0, 1);

      lcd.print("MQ7(C0):   ");
      lcd.print(mq7ppm);
      lcd.print(" ppm");
      lcd.setCursor(0, 2);

      lcd.print("MQ3(Alk.): ");
      lcd.print(mq3value);
      lcd.print("");
      lcd.setCursor(0, 3);

      lcd.print("Light Lvl: ");
      lcd.print(photocellReading);
      break;
    default:
      break;
  }
}

void serialPrintSensors() {
  if (false) {
    Serial.print("Humditiy ");
    Serial.print(humidity);
    Serial.println(" %");
  
    Serial.print("Temperature from DHT ");
    Serial.print(temperatureDHT);
    Serial.println(" C");
  
    Serial.print("Temperature from BMP ");
    Serial.print(temperatureBMP);
    Serial.println(" C");
  
    Serial.print("Absolute Pressure ");
    Serial.print(absolutePressure);
    Serial.println(" mb");
  
    Serial.print("Pressure Sea Level Compensated ");
    Serial.print(pressureSeaLevelCompensated);
    Serial.println(" mb");
  
    Serial.print("Altitude ");
    Serial.print(altitude);
    Serial.println(" m");
  
    Serial.print("MQ135 ");
    Serial.print(mq135ppm);
    Serial.println(" ppm");
  
    Serial.print("MQ7 ");
    Serial.print(mq7ppm);
    Serial.println(" ppm");
  
    Serial.print("MQ3 ");
    Serial.print(mq3value);
    Serial.println("");
  
    Serial.print("Photocellreading ");
    Serial.print(photocellReading);
    Serial.println("");
  }
}

void updateSensors() {
  updateHumidity();
  updatePressureReadings();
  mq135ppm = mq135.getPPM();
  mq7ppm   = mq7.getPPM();
  mq3value = analogRead(MQ3PIN);
  photocellReading = analogRead(PHOTOCELLPIN);
}

void updateHumidity() {
  int chk = DHT.read11(DHT11_PIN);
  temperatureDHT = DHT.temperature;
  humidity    = DHT.humidity;
}

void print2digits(int number) {
  // Output leading zero
  if (number >= 0 && number < 10) {
    lcd.write('0');
  }
  lcd.print(number);
}

void updatePressureReadings() {
  char status;

  status = pressure.startTemperature();
  if (status != 0) {
    delay(status);
    status = pressure.getTemperature(temperatureBMP);
  }
  status = pressure.startPressure(3);
  if (status != 0) {
    delay(status);
    status = pressure.getPressure(absolutePressure, temperatureBMP);
  }

  pressureSeaLevelCompensated = pressure.sealevel(absolutePressure, ALTITUDE);
  altitude = pressure.altitude(absolutePressure, pressureSeaLevelCompensated);
}

void setup() {
  Serial.begin(9600);
  lcd.init();
  pressure.begin();
  t = now();

  if (RTC.writeEN())
    lcd.print("Write allowed.");
  else
    lcd.print("Write protected.");

  // Print a message to the LCD.
  lcd.backlight();
  setSyncProvider(RTC.get);
}

void loop() {
  int change = RotaryEncoder::instance->getChange();
  lcdDisplayScreens(position);

  //Serial.println( RotaryEncoder::instance->encoderPos);
  if (change > 0) {
    position = position > 1 ? position : position + 1;
  }
  else if (change < 0)
  {
    position = position < 1 ? position : position - 1;
  }

  updateSensors();
  serialPrintSensors();
  delay ( 750 ); // Wait approx 1 sec
}
