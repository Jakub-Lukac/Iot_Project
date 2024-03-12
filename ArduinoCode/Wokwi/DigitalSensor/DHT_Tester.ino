#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#include "LiquidCrystal_I2C.h"
LiquidCrystal_I2C lcd(0x27, 20, 4);

uint8_t deg[8] = {
  0b11100,
  0b10100,
  0b11100,
  0b00000,
  0b00111,
  0b00100,
  0b00100,
  0b00111,
};

uint8_t percent[8] = {
  0b11000,
  0b11001,
  0b00010,
  0b00100,
  0b01000,
  0b10011,
  0b00011,
  0b00000,
};

void setup() {
  Serial.begin(9600);
  Serial.println(F("DHTxx test!"));
  dht.begin();
  
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Celcius:");
  lcd.setCursor(0, 1);
  lcd.print("Humidity:");
}

void loop() {
  delay(2000);

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);

  float hif = dht.computeHeatIndex(f, h);
  float hic = dht.computeHeatIndex(t, h, false);

  uint8_t deg2[8] = {0};
  uint8_t percent2[8] = {0};
  for (int i = 0; i < 8; i++) {
    deg2[i] = deg[i];
    lcd.createChar(3, deg2);
    percent2[i] = percent[i];
    lcd.createChar(4, percent2);
    delay(100);
  }

  delay(500);

  lcd.setCursor(10, 0);
  lcd.print(t);
  lcd.setCursor(14, 0);
  lcd.print("\x03");

  lcd.setCursor(10, 1);
  lcd.print(h);
  lcd.setCursor(14, 1);
  lcd.print("\x04");
}