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

float celcius;

// This function is called every time the Virtual Pin 0 state changes
BLYNK_WRITE(V0)
{
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();

  // Update state
  Blynk.virtualWrite(V1, value);
}

// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V2, celcius);
}

void setup() {
    Serial.begin(115200);

    Bridge.begin();  // make contact with the linux processor
    Blynk.begin(BLYNK_AUTH_TOKEN);
    // You can also specify server:
    //Blynk.begin(BLYNK_AUTH_TOKEN, "blynk.cloud", 80);
    //Blynk.begin(BLYNK_AUTH_TOKEN, IPAddress(192,168,240,1), 8080);

    // Setup a function to be called every second
    timer.setInterval(1000L, myTimerEvent);
    
    lcd.begin(16, 2, 0);
    lcd.setRGB(0, 255, 0);
    lcd.createChar(3, degreeSymbol);
    lcd.setCursor(0, 0);
    lcd.print("Temperature");
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

float analogToCelcius(int analog) {
  float BETA = 3950;
  return 1 / (log(1 / (1023. / analog - 1)) / BETA + 1.0 / 298.15) - 273.15;
}

void loop() {
    Blynk.run();
    timer.run();
    // You can inject your own code or combine it with other sketches.
    // Check other examples on how to communicate with Blynk. Remember
    // to avoid delay() function!

    int analogValue = analogRead(A0);
    celcius = analogToCelcius(analogValue);
    celcius = round(celcius * 10) / 10.0;  // Round to 1 decimal point

    lcd.setCursor(2, 1);
    lcd.print(celcius);

    lcd.setCursor(8, 1);
    lcd.print("\x03");

    setBacklightColour(celcius);
}
