#define BLYNK_PRINT Serial 
// #define ROTARY_ANGLE_SENSOR A2

#include "secrets.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include "esp_wpa2.h"
#include <DHT.h>
#include "rgb_lcd.h"

#define MQ135PIN 36  // Analog pin for MQ-135 air quality sensor
#define DHTPIN 2   // Digital pin for DHT-11 temperature/humidity sensor
#define CONTROLLERPIN 39  // Analog pin for button/rotary controller to switch modes of LCD
DHT dht(DHTPIN, DHT11);  // Types: DHT11, DHT22 (AM2302, AM2321), DHT21 (AM2301)

rgb_lcd lcd;

BlynkTimer timer;

uint8_t degreeSymbol[8] = { 0b11100, 0b10100, 0b11100, 0b00000, 0b00111, 0b00100, 0b00100, 0b00111 };

int celcius;
int humidity;
int airQuality;
int rotary = 0;

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

void mainTimer() {
  // celcius = 15;
  celcius = dht.readTemperature();

  // humidity = 50;
  humidity = dht.readHumidity();

  // airQuality = 30;
  airQuality = analogRead(MQ135PIN);

  Blynk.virtualWrite(V2, celcius);
  Blynk.virtualWrite(V3, humidity);
  Blynk.virtualWrite(V4, airQuality);

  // rotary = 70;
  rotary = analogRead(CONTROLLERPIN);

  if (rotary < 80) {
    printValueToLCD(celcius, "Temperature", "\x03", -10, 15, 40);
  } else if (rotary <= 160) {
    printValueToLCD(humidity, "Humidity", "%", 0, 50, 100);
  } else if (rotary > 160) {
    printValueToLCD(airQuality, "Air Quality", "AQI", 0, 250, 500);
  }
}

void setup() {
  Serial.begin(115200);
  delay(10);
  int status = WL_IDLE_STATUS;
  Serial.print("\nConnecting to network: ");
  Serial.println(WIFI_SSID);
  Serial.println(getWifiStatus(status));
  WiFi.disconnect(true);  // Disconnect from Wi-Fi to set new Wi-Fi connection
  WiFi.mode(WIFI_STA);  // Set Wi-Fi mode
  WiFi.begin(WIFI_SSID, WPA2_AUTH_PEAP, EAP_ANONYMOUS_IDENTITY, EAP_IDENTITY, EAP_PASSWORD); 

  int counter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    if (counter++ >= 60) { ESP.restart(); }
    status = WiFi.status();
    Serial.print(getWifiStatus(status));
    Serial.println("...");
    delay(500);
  }

  Serial.println("");
  Serial.println(F("Connected!"));
  Serial.println(F("Local ESP32 IP: "));
  Serial.println(WiFi.localIP());
    
  pinMode(MQ135PIN, INPUT);
  pinMode(CONTROLLERPIN, INPUT);

  lcd.begin(16, 2, 0);
  lcd.setRGB(0, 255, 0);
  lcd.createChar(3, degreeSymbol);

  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, "");
  timer.setInterval(1000L, mainTimer);
}

void loop() {
  Blynk.run();
  timer.run();
}
