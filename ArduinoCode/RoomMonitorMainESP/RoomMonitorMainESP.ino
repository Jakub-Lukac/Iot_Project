#define BLYNK_PRINT Serial 

#include "secrets.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "esp_wpa2.h"
#include <DHT.h>
#include <Wire.h>
#include "rgb_lcd.h"
#include <math.h>

#define MQ135PIN A4  // Analog pin for MQ-135 air quality sensor
#define DHTPIN 34   // Digital pin for DHT-11 temperature/humidity sensor
// #define DHTTYPE DHT11

#define BUTTON_PIN 25  // Digital pin for button/rotary controller to control the RGB LCD
// DHT dht(DHTPIN, DHTTYPE);  // Types: DHT11, DHT22 (AM2302, AM2321), DHT21 (AM2301)

rgb_lcd lcd;
BlynkTimer timer;

uint8_t degreeSymbol[8] = { 0b11100, 0b10100, 0b11100, 0b00000, 0b00111, 0b00100, 0b00100, 0b00111 };

float analogTemp;
int celcius;
int airQuality;
int currentButtonState;
int lastButtonState;
int lcdState = LOW;

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

int tempToCelcius(float analog) {
  const int B = 4275;               // B value of the thermistor
  const int R0 = 100000;            // R0 = 100k

  float R = 4095.0 / analog - 1.0;
  R = R0 * R;

  return 1.0 / (log(R / R0) / B + 1 / 278.15) - 273.15;
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
  lastButtonState = currentButtonState;
  currentButtonState = digitalRead(BUTTON_PIN);

  analogTemp = analogRead(DHTPIN);
  celcius = tempToCelcius(analogTemp);
  airQuality = analogRead(MQ135PIN);

  Blynk.virtualWrite(V2, celcius);
  Blynk.virtualWrite(V4, airQuality);

  if (lastButtonState == HIGH && currentButtonState == LOW) {
    if (lcdState == LOW) { lcdState = HIGH; } 
    else { lcdState = LOW; }
  }

  if (lcdState == LOW) { printValueToLCD(celcius, "Temperature", "\x03", -10, 18, 50); } 
  else { printValueToLCD(airQuality, "Air Quality", " AQI", 0, 100, 1000); }
}

String getWifiStatus(int status) {
  switch(status) {
    case WL_IDLE_STATUS: return "Wi-Fi Idle";
    case WL_SCAN_COMPLETED: return "Scan Completed";
    case WL_NO_SSID_AVAIL: return "No SSID";
    case WL_CONNECT_FAILED: return "Wi-Fi Failed";
    case WL_CONNECTION_LOST: return "Wi-Fi Lost";
    case WL_CONNECTED: return "Wi-Fi Connected";
    case WL_DISCONNECTED: return "Wi-Fi Init Bruv";
  }
}

void connectToWifi() {
  int status = WL_IDLE_STATUS;

  Serial.print("\nConnecting to network: ");
  Serial.println(WIFI_SSID);
  Serial.println(getWifiStatus(status));

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting to:");
  lcd.setCursor(0, 1);
  lcd.print(WIFI_SSID);
  delay(500);

  WiFi.disconnect(true);  // Disconnect from Wi-Fi to set new Wi-Fi connection
  WiFi.mode(WIFI_STA);  // Set Wi-Fi mode
  WiFi.begin(WIFI_SSID, WPA2_AUTH_PEAP, EAP_ANONYMOUS_IDENTITY, EAP_IDENTITY, EAP_PASSWORD); 
  // WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int counter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    if (counter++ >= 60) { ESP.restart(); }
    status = WiFi.status();
    Serial.print(getWifiStatus(status));

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(getWifiStatus(status));

    pinMode(BUTTON_PIN, INPUT);
    Serial.println("...");
    delay(500);
  }

  Serial.println("");
  Serial.println(F("Connected!"));
  Serial.println(F("Local ESP32 IP: "));
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT);

  lcd.begin(16, 2, 0);
  lcd.setRGB(255, 255, 255);
  lcd.createChar(3, degreeSymbol);

  connectToWifi();
  lcd.clear();
  lcd.print("Blynking...");
  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASSWORD);
  lcd.clear();
  lcd.print("Blynked!");
  currentButtonState = digitalRead(BUTTON_PIN);
  timer.setInterval(1000L, mainTimer);
}

void loop() {
  Blynk.run();
  timer.run();
}
