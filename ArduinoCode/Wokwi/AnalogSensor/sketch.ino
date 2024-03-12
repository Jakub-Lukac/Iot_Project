#include "LiquidCrystal_I2C.h"

LiquidCrystal_I2C lcd(0x27, 20, 4);

const float BETA = 3950;

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
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.createChar(3, degreesCelcius);
  lcd.setCursor(2, 0);
  lcd.print("Temperature");
}

void loop() {
  int analog = analogRead(A0);
  float celcius = 1 / (log(1 / (1023. / analog - 1)) / BETA + 1.0 / 298.15) - 273.15;
  celcius = round(celcius * 10) / 10.0;

  lcd.setCursor(2, 1);
  lcd.print(celcius);

  lcd.setCursor(8, 1);
  lcd.print("\x03");
}
