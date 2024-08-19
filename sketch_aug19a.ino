#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>  // Use ESP32-specific servo library

// Wi-Fi credentials
const char* ssid = "realme9i";
const char* password = "12345678";

// MQTT Broker details
const char* mqtt_broker = "b1a69793bf6e48eca7ab350e88d63dd4.s1.eu.hivemq.cloud";
const char* mqtt_username = "YoussefHelmy";
const char* mqtt_password = "Aa01122237213";
const int mqtt_port = 8883;

// Initialize Wi-Fi and MQTT client objects
WiFiClientSecure wifiClient;
PubSubClient client(wifiClient);

// Servo setup
Servo myServo;            // Servo object using ESP32Servo library
const int servoPin = 13;  // PWM pin for servo
const int irSensorPin = 34;
const int DHTSensorPin = 23;
const int flameSensorPin = 35;
const int SmokeSensorPin = 33;
const int gazSensorPin = 32;
const int ledpin = 2;
const int buzzerpin = 15;

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Attach the certificate bundle to the secure client
  wifiClient.setInsecure();

  // Set up the MQTT client
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);

  // Connect to the MQTT Broker
  while (!client.connected()) {
    Serial.print("Connecting to MQTT Broker...");
    String client_id = "esp32-client-" + String(WiFi.macAddress());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Connected to MQTT Broker");
      client.subscribe("esp32/servoControl");  // Subscribe to the servo control topic
    } else {
      Serial.print("Failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }

  // Attach the servo motor to the specified pin
  myServo.attach(servoPin);
}

void loop() {
  // Ensure the client stays connected and processes incoming messages
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Handle the IR sensor readings and MQTT publishing
  handleSensors();
}

// Function to handle IR sensor reading and publishing
void handleSensors() {
  // Read the IR sensor value
  int irValue = analogRead(irSensorPin);
  Serial.print("IR Sensor Value: ");
  Serial.println(irValue);

  int DHTValue = analogRead(DHTSensorPin);
  Serial.print("DHT Sensor Value: ");
  Serial.println(DHTValue);


  int flameSensorValue = analogRead(flameSensorPin);
  Serial.print("Flame Sensor Value: ");
  Serial.println(flameSensorValue);

  int SmokeSensorValue = analogRead(SmokeSensorPin);
  Serial.print("Smoke Sensor Value: ");
  Serial.println(SmokeSensorValue);

  int gazSensorValue = analogRead(gazSensorPin);
  Serial.print("GAZ Sensor Value: ");
  Serial.println(gazSensorValue);


  // Publish the sensors value to an MQTT topic
  String irPayload = String(irValue);
  client.publish("esp32/irSensor", irPayload.c_str());
    // Publish sensor data every 2 seconds

  String DHTPayload = String(DHTValue);
  client.publish("esp32/DHTSensor", DHTPayload.c_str());
 // Publish sensor data every 2 seconds

  String flamePayload = String(flameSensorValue);
  client.publish("esp32/flameSensor",flamePayload.c_str());
    // Publish sensor data every 2 seconds

  String SmokePayload = String(SmokeSensorValue);
  client.publish("esp32/SmokeSensor", SmokePayload .c_str());
   // Publish sensor data every 2 seconds

  String GAZPayload = String(gazSensorValue);
  client.publish("esp32/GAZSensor",GAZPayload.c_str());
    // Publish sensor data every 2 seconds
    delay(1000);
}

// Callback function to handle messages received on subscribed topics
void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  if (String(topic) == "esp32/servoControl") {
    int angle = message.toInt();
    Serial.print("Setting servo to: ");
    Serial.println(angle);
    myServo.write(angle);  // Write angle to servo
  }
}

// Reconnect function to maintain MQTT connection
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String client_id = "esp32-client-" + String(WiFi.macAddress());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Connected to MQTT Broker");
      client.subscribe("esp32/servoControl");  // Resubscribe to the topic
    } else {
      Serial.print("Failed with state ");
      Serial.print(client.state());
      delay(1000);
    }
  }
}
