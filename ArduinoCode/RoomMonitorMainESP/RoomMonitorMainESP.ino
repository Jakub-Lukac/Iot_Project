#define BLYNK_PRINT Serial 
// #define ROTARY_ANGLE_SENSOR A2

#include "secrets.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "esp_wpa2.h"
#include <DHT.h>
#include <Wire.h>
#include "rgb_lcd.h"

#define MQ135PIN A4  // Analog pin for MQ-135 air quality sensor
#define DHTPIN A0   // Digital pin for DHT-11 temperature/humidity sensor
#define DHTTYPE DHT11

#define CONTROLLERPIN A27  // Analog pin for button/rotary controller to control the RGB LCD
DHT dht(DHTPIN, DHTTYPE);  // Types: DHT11, DHT22 (AM2302, AM2321), DHT21 (AM2301)

rgb_lcd lcd;

BlynkTimer timer;

uint8_t degreeSymbol[8] = { 0b11100, 0b10100, 0b11100, 0b00000, 0b00111, 0b00100, 0b00100, 0b00111 };

float celcius;
float humidity;
int airQuality;
int button = 0;

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

  delay(3000);

  // humidity = 50;
  humidity = dht.readHumidity();

  if (isnan(celcius) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
  }

  // airQuality = 30;
  airQuality = analogRead(MQ135PIN);

  Blynk.virtualWrite(V2, celcius);
  Blynk.virtualWrite(V3, humidity);
  Blynk.virtualWrite(V4, airQuality);

  Serial.print("Celcius: ");
  Serial.print(celcius);
  Serial.println(" 'C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Air Quality: ");
  Serial.print(airQuality);
  Serial.println(" AQI");

  button = 0;
  // button = analogRead(CONTROLLERPIN);

  if (button < 80) {
    printValueToLCD(celcius, "Temperature", "\x03", -10, 15, 50);
  } else if (button <= 160) {
    printValueToLCD(humidity, "Humidity", "%", 0, 50, 100);
  } else if (button > 160) {
    printValueToLCD(airQuality, "Air Quality", "AQI", 0, 250, 500);
  }
}

void connectToWifi() {
  int status = WL_IDLE_STATUS;
  Serial.print("\nConnecting to network: ");
  Serial.println(WIFI_SSID);
  Serial.println(getWifiStatus(status));
  WiFi.disconnect(true);  // Disconnect from Wi-Fi to set new Wi-Fi connection
  WiFi.mode(WIFI_STA);  // Set Wi-Fi mode
  // WiFi.begin(WIFI_SSID, WPA2_AUTH_PEAP, EAP_ANONYMOUS_IDENTITY, EAP_IDENTITY, EAP_PASSWORD); 
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int counter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    if (counter++ >= 60) { ESP.restart(); }
    status = WiFi.status();
    Serial.print(getWifiStatus(status));
    // pinMode(CONTROLLERPIN, INPUT);
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

  pinMode(DHTPIN, INPUT_PULLUP);
  // pinMode(CONTROLLERPIN, INPUT);

  lcd.begin(16, 2, 0);
  lcd.setRGB(0, 255, 0);
  lcd.createChar(3, degreeSymbol);

  dht.begin();

  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASSWORD);
  timer.setInterval(3000L, mainTimer);
}

void loop() {
  Blynk.run();
  timer.run();
}
