//Sender's Code

// Including necessary libraries
#include <ArduinoMqttClient.h>
#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_AVR_UNO_WIFI_REV2)
#include <WiFiNINA.h>
#elif defined(ARDUINO_SAMD_MKR1000)
#include <WiFi101.h>
#elif defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#endif

// WiFi credentials
char ssid[] = "Keep Tryin";
char pass[] = "kyubtaupassword";

// Ultrasonic sensor pins
const int trigPin = 2;
const int echoPin = 3;

// Variables for measuring distance
float duration, distance;

// Creating a WiFi client and MQTT client instance
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

// MQTT broker details
const char broker[] = "mqtt-dashboard.com";
int port = 1883;
const char topic[] = "AshimaWave";

// Timing variables
const long interval = 1000;
unsigned long previousMillis = 0;

// Counter
int count = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  // Connecting details of the WiFi network
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.print(".");
    delay(5000);
  }

  Serial.println("You're connected to the network");
  Serial.println();

  // Connecting to the MQTT broker
  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();
}

void loop() {
  // Polling the MQTT client
  mqttClient.poll();

  // Getting the current time in milliseconds
  unsigned long currentMillis = millis();

  // Checking if it's time to send a message
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;

    // Triggering the ultrasonic sensor to measure distance
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Measuring the duration of the echo
    duration = pulseIn(echoPin, HIGH);
    
    // Calculating the distance based on the duration
    distance = (duration * 0.0343) / 2;
    
    // Checking if the distance is less than 10 units
    if (distance < 10)
    {
      // Begining an MQTT message
      mqttClient.beginMessage(topic);
      // Addings a message payload
      mqttClient.print(" Wave delivered");
      // Ending the MQTT message
      mqttClient.endMessage();
      // Adding Delay to avoid sending multiple messages quickly
      delay(1000);
    }

    Serial.println();

    // Incrementing the counter
    count++;
  }
}

