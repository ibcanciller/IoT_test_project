#include <WiFiNINA.h>     // Include the WiFiNina library
#include <DHT.h>          // Include the DHT sensor library
#include <Servo.h>

#define DHTPIN 6          // Define Pin Number 6 as the DHT Pin
#define DHTTYPE DHT11     // Define DHT Type (DHT11, DHT22)

char ssid[] = "wifi_arduino";         // WiFi SSID
char pass[] = "qwerty123";               // WiFi Password
char server[] = "script.google.com";    // Server for Google Apps Script
char deviceName[] = "ARDUINO_MKR_1010";   // Device ID
char location[] = "1F_North_West";
char GScriptId[] = "AKfycbw6Z4Yxkxl-gcVznnoD89qEgAzRdbzch4d9GF--iCbFpHO3-ObpqP1b-28ujPq5rS0";  // Google Apps Script Deployment ID
char sensorTemp[] = "Temperature";
char sensorHumid[] = "Humidity";

float temp = 0.0;
float humid = 0.0;
int pos = 0;
int sensorCount = 2;

WiFiSSLClient client;       // Create an instance of the WiFiSSLClient Class
DHT dht(DHTPIN, DHTTYPE);   // Create an instance of the DHT Class
Servo myservo;

void setup() {
  // Initialize the DHT sensor
  dht.begin();
  myservo.attach(5);
  // Initialize WiFi
  WiFi.begin(ssid, pass);
  // Initialize Serial communication with computer at 9600 baud rate
  Serial.begin(9600);

  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.println(".");
  }
  Serial.println("Connected to WiFi!");
}

void loop() {

  // Read the Temperature and Humidity from the DHT Sensor
  temp = dht.readTemperature();
  delay(100);
  humid = dht.readHumidity();

  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.print("\n");
  Serial.print("Humidity: ");
  Serial.print(humid);
  Serial.println("\n");

  // Cast the floating point type values into String
  String tempValue = String(temp,2);
  String humidValue = String(humid,2);

  if (temp > 32) {
    for (pos; pos <= 180; pos += 1) {
      myservo.write(pos);
      delay(15);
    }
  } else {
    for (pos; pos >=0; pos -= 1) {
      myservo.write(pos);
      delay(15);
    }
  }
  
  // Send Temperature and Humidity data to Google Sheets via HTTPS GET Method
  for (int i = 1; i <= sensorCount; i++) {
    if (i == 1) {
      String url = "GET " + String("/macros/s/") + GScriptId + "/exec?location=" + location + "&devicename=" + deviceName + "&sensor=" + sensorTemp + "&value=" + tempValue + " HTTP/1.1";
      if (client.connectSSL(server, 443)) {
        client.println(url);
        client.println("Host: script.google.com");
        client.println("Connection: close");
        client.println();

        delay(1000);
      }
    } else if (i == 2) {
      String url = "GET " + String("/macros/s/") + GScriptId + "/exec?location=" + location + "&devicename=" + deviceName + "&sensor=" + sensorHumid + "&value=" + humidValue + " HTTP/1.1";
      if (client.connectSSL(server, 443)) {
        client.println(url);
        client.println("Host: script.google.com");
        client.println("Connection: close");
        client.println();

        delay(1000);
      }
    }
  }
  delay(8000);
}