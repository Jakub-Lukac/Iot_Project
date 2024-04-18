#define BLYNK_TEMPLATE_ID           "TMPL4fnh6fOfH"
#define BLYNK_TEMPLATE_NAME         "TESTING"
#define BLYNK_AUTH_TOKEN            "euvMzHZrgcq9EjXGBV3cYtoN-rjmSNgo"

// #define BLYNK_TEMPLATE_ID "TMPL4rY8eAPdy"
// #define BLYNK_TEMPLATE_NAME "RoomMonitorTest"  
// #define BLYNK_AUTH_TOKEN "FbhVJkaUNkTJu4R2nffi3DF-MKS8qQ5M"

#define BLYNK_PRINT Serial

#define ROTARY_ANGLE_SENSOR A2

#include <Bridge.h>
#include <BlynkSimpleYun.h>
#include <Process.h>

#include <Wire.h>
#include "rgb_lcd.h"

BlynkTimer timer;

rgb_lcd lcd;

uint8_t degreeSymbol[8] = {
  0b11100,
  0b10100,
  0b11100,
  0b00000,
  0b00111,
  0b00100,
  0b00100,
  0b00111,
};

int celcius;
int humidity;
int rotary = 0;


BLYNK_WRITE(V0) {
  int value = param.asInt();
  Blynk.virtualWrite(V1, value);
}

void myTimerEvent() {
  Blynk.virtualWrite(V2, celcius);
  // Blynk.virtualWrite(V3, humidity);
}


void setup() {
    pinMode(ROTARY_ANGLE_SENSOR, INPUT);
    Serial.begin(115200);

    Bridge.begin();  // make contact with the linux processor
    Blynk.begin(BLYNK_AUTH_TOKEN);  // You can also specify server:

    timer.setInterval(1000L, myTimerEvent);
    
    lcd.begin(16, 2, 0);
    lcd.setRGB(0, 255, 0);
    lcd.createChar(3, degreeSymbol);
    Serial.println("SETUP DONE");
}

void setBacklightColour(float input, int min, int mid, int max) {
    int r, g, b;

    if (input <= min) {
        r = 0;
        g = 0;
        b = 255;
    } else if (input <= mid) {
        r = 0;
        g = map(input, min, mid, 0, 255);
        b = 255 - g;
    } else {
        r = 255;
        g = map(input, mid, max, 255, 0);
        b = 0;
    }

    lcd.setRGB(r, g, b);
}

int tempToCelcius(float temp) {
  const int BETA = 3975;
  float resistance = (float)(1023 - temp) * 10000 / temp;  // get sensor's resistance
  return 1 / (log(resistance / 10000) / BETA + 1 / 298.15) - 273.15;  // convert to temperature via datasheet&nbsp;
}

void printValueToLCD(int value, String title, String symbol, int min, int mid, int max) {
    setBacklightColour(value, min, mid, max);
    lcd.setCursor(0, 0);
    lcd.print(title);
    lcd.setCursor(2, 1);
    lcd.print(value);
    lcd.setCursor(5, 1);
    lcd.print(symbol);
}

void loop() {
    Blynk.run();
    timer.run();

    rotary = analogRead(ROTARY_ANGLE_SENSOR);
    Serial.println(rotary);

    if (rotary < 150) {
        celcius = tempToCelcius(analogRead(A0));
        printValueToLCD(celcius, "Temperature", "\x03", -10, 15, 40);
        
        
    }
    else if (rotary >= 150) {
        // int humidity = analogRead(A1);
        humidity = 54;
        printValueToLCD(humidity, "Humidity", "%", 0, 50, 100);
    }
        delay(1000);
        lcd.clear();
}
