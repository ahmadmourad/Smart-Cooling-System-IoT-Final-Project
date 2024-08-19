#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#define DHTTYPE DHT11

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
Servo myServo;
const int servoPin = 13;
const int irSensorPin = 34;
const int DHTSensorPin = 23;
const int flameSensorPin = 26;
const int SmokeSensorPin = 33;
const int gazSensorPin = 32;
const int ledPin = 5;
const int buzzerPin = 15;

// DHT Sensor setup
DHT dht(DHTSensorPin, DHTTYPE);

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Initialize DHT Sensor
  dht.begin();

  // Initialize LCD
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.print("System Initializing");

  // Initialize GPIO pins
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  digitalWrite(buzzerPin, LOW);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    lcd.setCursor(0, 1);
    lcd.print("Connecting to WiFi");
  }
  Serial.println("Connected to WiFi");
  lcd.clear();
  lcd.print("WiFi Connected");

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
      Serial.println(client.state());
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

  // Handle sensors and MQTT publishing
  handleSensors();
}

// Function to handle sensor readings, publishing, and actuation
void handleSensors() {
  // Read the IR sensor value
  int irvalue = analogRead(irSensorPin);
  int Distance_cm = map(irvalue, 0, 4095, 0, 12);
  Serial.print("IR Sensor Value: ");
  Serial.println( Distance_cm );

// Read and classify Flame Sensor value
int flameSensorValue = digitalRead(flameSensorPin); 
String flameStatus = flameSensorValue == HIGH ? "High" : "Low";  
Serial.print("Flame Sensor Status: ");
Serial.println(flameStatus);

  int SmokeSensorValue = analogRead(SmokeSensorPin);
  int SmokePercentage = map(SmokeSensorValue, 0, 4095, 0, 100);
  Serial.print("Smoke Sensor Value: ");
  Serial.print(SmokePercentage);
  Serial.println("%");

  int gazSensorValue = analogRead(gazSensorPin);
  int gazPercentage = map(gazSensorValue, 0, 4095, 0, 100);
  Serial.print("Gas Sensor Value: ");
  Serial.print(gazPercentage);
  Serial.println("%");

  // Read temperature and humidity
  float temperature = dht.readTemperature();
  Serial.print("Temperature Value: ");
  Serial.print(temperature);
  Serial.println(" °C");

  float humidity = dht.readHumidity();
  Serial.print("Humidity Value: ");
  Serial.print(humidity);
  Serial.println(" %");

  // Publish sensor data to MQTT topics
  client.publish("esp32/irSensor", String(Distance_cm).c_str());
  client.publish("esp32/flameSensor", flameStatus.c_str());
  client.publish("esp32/SmokeSensor", String(SmokePercentage).c_str());
  client.publish("esp32/gasSensor", String(gazPercentage).c_str());
  client.publish("esp32/TemperatureSensor", String(temperature).c_str());
  client.publish("esp32/HumiditySensor", String(humidity).c_str());

  // Check IR sensor to open door (servo) if a person is detected
  if (Distance_cm < 5) {
    Serial.println("Person detected, opening door");
    lcd.clear();
    lcd.print("Person detected");
    myServo.write(180);  // Move servo to open door
    delay(5000);        // Keep door open for 5 seconds
    myServo.write(0);    // Close door
  }

  // Check for fire, gas, or smoke detection
  if (flameStatus == "High" || SmokePercentage > 50 || gazPercentage > 50) {
    Serial.println("Alert: Fire/Smoke/Gas detected");
    lcd.clear();
    lcd.print("Alert Detected!");
    digitalWrite(ledPin, HIGH);
    digitalWrite(buzzerPin, HIGH);
    client.publish("esp32/alerts", "Fire/Smoke/Gas detected!");
    delay(10000); // Keep the alert for 10 seconds
    digitalWrite(ledPin, LOW);
    digitalWrite(buzzerPin, LOW);
  }

  // Display temperature and humidity on LCD
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(humidity);
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
      Serial.println(client.state());
      delay(2000);
    }
  }
}


