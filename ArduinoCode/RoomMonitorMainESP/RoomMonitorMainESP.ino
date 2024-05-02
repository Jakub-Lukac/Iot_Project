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

void setBacklightColour(float input, int min, int mid, int mid_max, int max) {
  if (input <= min) {
    int r = 0;
    int g = 0;
    int b = 255;
  } 

  else if (input >= max) {
    int r = 255;
    int g = 0;
    int b = 0;
  } 

  else {
    float x[] = {min, mid, mid_max, max};

    int y_r[] = {0, 0, 255, 255};
    int y_g[] = {0, 255, 255, 0};
    int y_b[] = {255, 0, 0, 0};

    int r = constrain(map(input, min, max, 0, 255), 0, 255);
    int g = constrain(map(input, min, max, 0, 255), 0, 255);
    int b = constrain(map(input, min, max, 0, 255), 0, 255);
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

void printValueToLCD(int value, String title, String symbol, int min, int mid, int mid_max, int max) {
  lcd.clear();
  setBacklightColour(value, min, mid, mid_max, max);
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

  if (lcdState == LOW) { printValueToLCD(celcius, "Temperature", "\x03", -10, 17, 25, 50); } 
  else { printValueToLCD(airQuality, "Air Quality", " AQI", 0, 100, 150, 600); }
}

void connectToWifi() {
  int status = WL_IDLE_STATUS;
  Serial.print("\nConnecting to network: ");
  Serial.println(WIFI_SSID);
  Serial.println(getWifiStatus(status));
  WiFi.disconnect(true);  // Disconnect from Wi-Fi to set new Wi-Fi connection
  WiFi.mode(WIFI_STA);  // Set Wi-Fi mode
  WiFi.begin(WIFI_SSID, WPA2_AUTH_PEAP, EAP_ANONYMOUS_IDENTITY, EAP_IDENTITY, EAP_PASSWORD); 
  // WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int counter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    if (counter++ >= 60) { ESP.restart(); }
    status = WiFi.status();
    Serial.print(getWifiStatus(status));
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
  connectToWifi();
  pinMode(BUTTON_PIN, INPUT);

  lcd.begin(16, 2, 0);
  lcd.setRGB(0, 255, 0);
  lcd.createChar(3, degreeSymbol);

  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASSWORD);
  currentButtonState = digitalRead(BUTTON_PIN);
  timer.setInterval(1000L, mainTimer);
}

void loop() {
  Blynk.run();
  timer.run();
}
