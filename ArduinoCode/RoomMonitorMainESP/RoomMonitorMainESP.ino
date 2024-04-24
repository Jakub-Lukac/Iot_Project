#define BLYNK_PRINT Serial 
// #define ROTARY_ANGLE_SENSOR A2

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include "secrets.h"
#include "esp_wpa2.h"
#include "rgb_lcd.h"
rgb_lcd lcd;
BlynkTimer timer;

uint8_t degreeSymbol[8] = { 0b11100, 0b10100, 0b11100, 0b00000, 0b00111, 0b00100, 0b00100, 0b00111 };
int celcius;
int humidity;
int airQuality;
int rotary = 0;

void myTimerEvent() {
  Blynk.virtualWrite(V2, celcius);
  Blynk.virtualWrite(V3, humidity);
  Blynk.virtualWrite(V4, airQuality);
}

String getWifiStatus(int status) {
  switch(status) {
    case WL_IDLE_STATUS: return "WL_IDLE_STATUS";
    case WL_SCAN_COMPLETED: return "WL_SCAN_COMPLETED";
    case WL_NO_SSID_AVAIL: return "WL_NO_SSID_AVAIL";
    case WL_CONNECT_FAILED: return "WL_CONNECT_FAILED";
    case WL_CONNECTION_LOST: return "WL_CONNECTION_LOST";
    case WL_CONNECTED: return "WL_CONNECTED";
    case WL_DISCONNECTED: return "WL_DISCONNECTED";
  }
}

void setup() {
  Serial.begin(115200);
  delay(10);
  int status = WL_IDLE_STATUS;
  Serial.println();
  Serial.print(F("Connecting to network: "));
  Serial.println(WIFI_SSID);
  Serial.println(getWifiStatus(status));
  WiFi.disconnect(true);  // disconnect from wi-fi to set new wi-fi connection
  WiFi.mode(WIFI_STA);  // init wi-fi mode
  WiFi.begin(WIFI_SSID, WPA2_AUTH_PEAP, EAP_ANONYMOUS_IDENTITY, EAP_IDENTITY, EAP_PASSWORD); 

  int counter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    if (counter++ >= 60) {  
      ESP.restart();  
    }

    status = WiFi.status();
    Serial.print(getWifiStatus(status));
    Serial.println("...");
    
    delay(500);
  }

  Serial.println("");
  Serial.println(F("Wi-Fi connected!"));
  Serial.println(F("Local ESP32 IP: "));
  Serial.println(WiFi.localIP());

  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, "");
  timer.setInterval(1000L, myTimerEvent);
    
  // pinMode(ROTARY_ANGLE_SENSOR, INPUT);
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
  lcd.clear();
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

  // rotary = analogRead(ROTARY_ANGLE_SENSOR);
  rotary = 70;

  // celcius = tempToCelcius(analogRead(A0));
  celcius = 18;
  Serial.print("Celcius: ");
  Serial.println(celcius);

  // humidity = analogRead(A1);
  humidity = 54;
  Serial.print("Humidity: ");
  Serial.println(humidity);

  // airQuality = analogRead(A2);
  airQuality = 122;
  Serial.print("Air Quality: ");
  Serial.println(airQuality);

  if (rotary < 80) {
    printValueToLCD(celcius, "Temperature", "\x03", -10, 15, 40);
    Serial.println("LCD Displaying: Celcius \n");
  } else if (rotary <= 160) {
    printValueToLCD(humidity, "Humidity", "%", 0, 50, 100);
    Serial.println("LCD Displaying: Humidity \n");
  } else if (rotary > 160) {
    printValueToLCD(airQuality, "Air Quality", "AQI", 0, 250, 500);
    Serial.println("LCD Displaying: Air Quality \n");
  }

  delay(1000);
}
