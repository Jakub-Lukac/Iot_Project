#define BLYNK_TEMPLATE_ID "TMPL4fnh6fOfH"
#define BLYNK_TEMPLATE_NAME "TESTING"
#define BLYNK_AUTH_TOKEN "euvMzHZrgcq9EjXGBV3cYtoN-rjmSNgo"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include <BlynkRpcClient.h>
#include <SPI.h>
#include <WiFi101.h>
#include <BlynkSimpleWiFiShield101.h>

#include <Wire.h>
#include "rgb_lcd.h"

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "DobryDen";
char pass[] = "DobryDen";

BlynkTimer timer;

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

rgb_lcd lcd;

const float BETA = 3950;

void setup() 
{
    Serial.begin(115200);
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

    timer.setInterval(1000L, myTimerEvent);

    lcd.begin(16, 2);
    lcd.setRGB(255, 0, 255);
    delay(1000);
}

void loop() 
{
    Blynk.run();
    timer.run();

    int analogValue = analogRead(A0);
    celcius = 1 / (log(1 / (1023. / analogValue - 1)) / BETA + 1.0 / 298.15) - 273.15;

    lcd.setCursor(0, 1);
    lcd.print(celcius);
}
