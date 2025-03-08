#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL2r-agn4HR"
#define BLYNK_TEMPLATE_NAME "Practica 1"
#define BLYNK_AUTH_TOKEN "zWajz8tDCN-ayO92Iw61cOs0-NUkCKHw"
#define SFH7779 0x39
#define SENSORREADING 8
#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <XNODE.h>
#include "Adafruit_SHT4x.h"
Adafruit_SHT4x sht4 = Adafruit_SHT4x();
uint8_t SensorValueCatch[6]; 
XNODE xnode(&Serial2);
#define LED 4
float TEMP, HUM, LUM;
int PROX, switchState = 0;
char ssid[] = "Gudiño";
char pass[] = "01123581321";

void setup()
{
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.begin(115200);
  Serial2.begin(115200);
  while (!Serial)
    delay(10);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  Serial.println("Adafruit SHT4x test");
  if (! sht4.begin())
  {
    Serial.println("Couldn't find SHT4x");
    while (1) delay(1);
  }
  Serial.println("Found SHT4x sensor");
  Serial.print("Serial number 0x");
  Serial.println(sht4.readSerial(), HEX);
  sht4.setPrecision(SHT4X_HIGH_PRECISION);
  switch (sht4.getPrecision()) 
  {
     case SHT4X_HIGH_PRECISION: 
       Serial.println("High precision");
       break;
     case SHT4X_MED_PRECISION: 
       Serial.println("Med precision");
       break;
     case SHT4X_LOW_PRECISION: 
       Serial.println("Low precision");
       break;
  }
  sht4.setHeater(SHT4X_NO_HEATER);
  switch (sht4.getHeater())
  {
     case SHT4X_NO_HEATER: 
       Serial.println("No heater");
       break;
     case SHT4X_HIGH_HEATER_1S: 
       Serial.println("High heat for 1 second");
       break;
     case SHT4X_HIGH_HEATER_100MS: 
       Serial.println("High heat for 0.1 second");
       break;
     case SHT4X_MED_HEATER_1S: 
       Serial.println("Medium heat for 1 second");
       break;
     case SHT4X_MED_HEATER_100MS: 
       Serial.println("Medium heat for 0.1 second");
       break;
     case SHT4X_LOW_HEATER_1S: 
       Serial.println("Low heat for 1 second");
       break;
     case SHT4X_LOW_HEATER_100MS: 
       Serial.println("Low heat for 0.1 second");
       break;
  }
  Wire.begin();
  while (1)
  {
    Wire.beginTransmission(SFH7779);
    byte error = Wire.endTransmission();
    if (error == 0) 
    {
      break;
    }
  }
  Wire.beginTransmission(SFH7779);
  Wire.write(0x41);
  Wire.write(0x09);
  Wire.write(0x2B);
  Wire.endTransmission();
}

BLYNK_WRITE(V4) 
{
    switchState = param.asInt();
}

void ReadSensor(void) 
{
  Wire.beginTransmission(SFH7779);
  Wire.write(0x44);
  Wire.endTransmission();
  Wire.requestFrom(SFH7779, 6);
  uint8_t count1 = 0;
  while (Wire.available()) 
  {
    SensorValueCatch[count1] = Wire.read();
    count1++;
  }
}

float Lux_Value(uint16_t VIS, uint16_t IR)
{
  float LUX;
  float Div = float(IR) / float(VIS);
  if (Div < 0.109) 
  {
    LUX = (1.534 * VIS) - (3.759 * IR);
  } 
  else if (Div < 0.429) 
  {
    LUX = (1.339 * VIS) - (1.972 * IR);
  } 
  else if (Div < (1.3775)) 
  {
    LUX = (0.701 * VIS) - (0.483 * IR);
  } 
  else if (Div < (2.175)) 
  {
    LUX = (1.402 * VIS) - (.56994 * IR);
  } 
  else if (Div < (3.625))
   {
    LUX = (2.804 * VIS) - (.64239 * IR);
  } 
  else
  {
    LUX = (5.608 * VIS);
  }
  LUX = LUX / 64;
  return (LUX);
}

void loop()
{
  Blynk.run();
  if(switchState == 1)
  {
    sensors_event_t humidity, temp;
    uint16_t error, prox;
    uint32_t timestamp = millis();
    float lum;
    ReadSensor();
    lum = (float)(Lux_Value((SensorValueCatch[3] << SENSORREADING) | SensorValueCatch[2], (SensorValueCatch[5] << SENSORREADING) | SensorValueCatch[4]));
    prox = (uint16_t)((SensorValueCatch[1] << SENSORREADING) | SensorValueCatch[0]);
    sht4.getEvent(&humidity, &temp);
    timestamp = millis() - timestamp;
    Blynk.virtualWrite(V0, temp.temperature);
    Blynk.virtualWrite(V1, humidity.relative_humidity);
    Blynk.virtualWrite(V2, lum);
    Blynk.virtualWrite(V3, prox);
    if (lum > 100)
    {
      Blynk.logEvent("luminosidad_alta");
    }
  }
  else
  {
    Blynk.virtualWrite(V0, 0);
    Blynk.virtualWrite(V1, 0);
    Blynk.virtualWrite(V2, 0);
    Blynk.virtualWrite(V3, 0);
  }
  delay(1000);
}