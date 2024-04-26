#include <Bridge.h>
#include <Process.h>

#include <Wire.h>
#include "rgb_lcd.h"

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
int button = 0;
const int buttonPin = 2; 

void setup() {
    pinMode(buttonPin, INPUT);
    Serial.begin(115200);

    lcd.begin(16, 2, 0);
    lcd.setRGB(0, 255, 0);
    lcd.createChar(3, degreeSymbol);

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
    Serial.println("Hi")
    int analogValue = analogRead(A0);
    celcius = analogToCelcius(analogValue);

    lcd.setCursor(2, 1);
    lcd.print(celcius);

    setBacklightColour(celcius);
}
