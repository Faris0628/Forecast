#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Ultrasonic.h>
// WiFi credentials
const char* ssid = "vivo V23 5G";
const char* password = "123456789";
// Google Script ID - deploy as web app and get the URL
const char* scriptURL = "https://script.google.com/macros/s/AKfycbz8tL6KiAsTixmGAz9g8FlHeyxLuQrTqSXmqcvMtZV-P2PpRFp7_V4ZFJyI7zOALtkV/exec";
// Ultrasonic sensor configuration
const int ultrasonicPin = 13; // Digital pin connected to ultrasonic sensor
Ultrasonic ultrasonic(ultrasonicPin);
// Data sending interval (in milliseconds)
const unsigned long sendInterval = 10000; // 10 seconds
unsigned long previousMillis = 0;
void setup() {
 // Initialize serial communication
 Serial.begin(115200);
 delay(1000);
 Serial.println("ESP32 Ultrasonic Ranger Data Logger");

 // Connect to WiFi
 WiFi.begin(ssid, password);
 Serial.print("Connecting to WiFi");
 while (WiFi.status() != WL_CONNECTED) {
 delay(500);
 Serial.print(".");
 }
 Serial.println();
 Serial.print("Connected to WiFi with IP: ");
 Serial.println(WiFi.localIP());
}
void loop() {
 unsigned long currentMillis = millis();

 // Check if it's time to send data
 if (currentMillis - previousMillis >= sendInterval) {
 previousMillis = currentMillis;

 // Read distance from ultrasonic sensor (in cm)
 long distance = ultrasonic.read();

 Serial.print("Distance: ");
 Serial.print(distance);
 Serial.println(" cm");

 // Send data to Google Sheets
 sendDataToGoogleSheets(distance);
 }
}
void sendDataToGoogleSheets(long distance) {
 // Check WiFi connection
 if (WiFi.status() != WL_CONNECTED) {
 Serial.println("WiFi not connected");
 return;
 }

 HTTPClient http;
 http.begin(scriptURL);
 http.addHeader("Content-Type", "application/json");

 // Create JSON data
 StaticJsonDocument<200> doc;
 doc["distance"] = distance;

 String jsonString;
 serializeJson(doc, jsonString);

 // Send HTTP POST request
 int httpResponseCode = http.POST(jsonString);

 if (httpResponseCode > 0) {
 String response = http.getString();
 Serial.println("HTTP Response code: " + String(httpResponseCode));
 Serial.println("Response: " + response);
 } else {
 Serial.print("Error on sending POST: ");
 Serial.println(httpResponseCode);
 }

 http.end();
}
