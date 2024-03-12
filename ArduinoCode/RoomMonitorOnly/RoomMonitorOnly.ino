#include <Wire.h>
#include "rgb_lcd.h"

rgb_lcd lcd;

uint8_t degreesCelcius[8] = {
  0b11100,
  0b10100,
  0b11100,
  0b00000,
  0b00111,
  0b00100,
  0b00100,
  0b00111,
};

void setup() {
    Serial.begin(115200);
    lcd.begin(16, 2, 0);
    lcd.setRGB(0, 255, 0);
    lcd.createChar(3, degreesCelcius);
    lcd.setCursor(0, 0);
    lcd.print("Temperature");
}

void setBacklightColour(float celcius) {
    int r, g, b;

    if (celcius <= 0) {
        r = 0;
        g = 0;
        b = 255;
    } else if (celcius <= 15) {
        r = map(celcius, 0, 15, 0, 255);
        g = map(celcius, 0, 15, 0, 255);
        b = 255 - g;
    } else {
        r = 255;
        g = map(celcius, 15, 30, 255, 0);
        b = 0;
    }

    lcd.setRGB(r, g, b);
}

float analogToCelcius(int analog) {
  float BETA = 3950;
  return 1 / (log(1 / (1023. / analog - 1)) / BETA + 1.0 / 298.15) - 273.15;
}

void loop() {
    int analogValue = analogRead(A0);
    float celcius = analogToCelcius(analogValue);
    celcius = round(celcius * 10) / 10.0;  // Round to 1 decimal point

    lcd.setCursor(2, 1);
    lcd.print(celcius);

    lcd.setCursor(8, 1);
    lcd.print("\x03");

    setBacklightColour(celcius);
}
