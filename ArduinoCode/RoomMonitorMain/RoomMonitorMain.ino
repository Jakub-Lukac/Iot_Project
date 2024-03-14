#define BLYNK_TEMPLATE_ID           "TMPL4fnh6fOfH"
#define BLYNK_TEMPLATE_NAME         "TESTING"
#define BLYNK_AUTH_TOKEN            "euvMzHZrgcq9EjXGBV3cYtoN-rjmSNgo"

#define BLYNK_PRINT Serial

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

BLYNK_WRITE(V0)  
{
  int value = param.asInt();
  Blynk.virtualWrite(V1, value);
}

void myTimerEvent()
{
  Blynk.virtualWrite(V2, celcius);
}

void setup() {
    Serial.begin(115200);

    Bridge.begin();  // make contact with the linux processor
    Blynk.begin(BLYNK_AUTH_TOKEN);  // You can also specify server:
    timer.setInterval(1000L, myTimerEvent);
    
    lcd.begin(16, 2, 0);
    lcd.setRGB(0, 255, 0);
    lcd.createChar(3, degreeSymbol);
    lcd.setCursor(0, 0);
    lcd.print("Temperature");
    lcd.setCursor(5, 1);
    lcd.print("\x03");
}

void setBacklightColour(float celcius) {
    int r, g, b;
    const int minTemp = -5;
    const int midTemp = 15;
    const int maxTemp = 35;

    if (celcius <= minTemp) {
        r = 0;
        g = 0;
        b = 255;
    } else if (celcius <= midTemp) {
        r = 0;
        g = map(celcius, minTemp, midTemp, 0, 255);
        b = 255 - g;
    } else {
        r = 255;
        g = map(celcius, midTemp, maxTemp, 255, 0);
        b = 0;
    }

    lcd.setRGB(r, g, b);
}

int analogToCelcius(int analog) {
  float BETA = 3975;
  float resistance = (float)(1023 - analog) * 10000 / analog;  //get the resistance of the sensor;
  return 1 / (log(resistance / 10000) / BETA + 1 / 298.15) - 273.15;  //convert to temperature via datasheet&nbsp;;
}

void loop() {
    Blynk.run();
    timer.run();

    int analogValue = analogRead(A0);
    celcius = analogToCelcius(analogValue);

    lcd.setCursor(2, 1);
    lcd.print(celcius);

    setBacklightColour(celcius);
}
