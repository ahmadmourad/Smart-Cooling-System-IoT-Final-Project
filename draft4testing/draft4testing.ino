#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#define DHTTYPE DHT11


// Servo setup
Servo myServo;
const int servoPin = 13;
const int irSensorPin = 34;
const int DHTSensorPin = 23;
const int flameSensorPin = 35;
const int smokeSensorPin = 33;
const int gasSensorPin = 32;
const int ledPin = 2;
const int buzzerPin = 15;
const int fan1 = 26;
const int fan2 = 14;

// DHT Sensor setup
DHT dht(DHTSensorPin, DHTTYPE);

// LCD setup
// LiquidCrystal_I2C lcd(0x27, 16, 2);

// // Wi-Fi credentials
// const char* ssid = "LYNX";
// const char* password = "ahmed393";

// // MQTT Broker details
// const char* mqtt_broker = "30dacc27eccc41338d1d3d410bc82c17.s1.eu.hivemq.cloud";
// const char* mqtt_username = "hivemq.webclient.1724148755114";
// const char* mqtt_password = "oa1B#zV9T3CJyN6.%gv,";
// const int mqtt_port = 8883;

// // Initialize Wi-Fi and MQTT client objects
// WiFiClientSecure wifiClient;
// PubSubClient client(wifiClient);

// // Callback function to handle messages received on subscribed topics
// void callback(char* topic, byte* payload, unsigned int length) {
//   String message;
//   for (int i = 0; i < length; i++) {
//     message += (char)payload[i];
//   }

//   if (String(topic) == "esp32/servoControl") {
//     int angle = message.toInt();
//     Serial.print("Setting servo to: ");
//     Serial.println(angle);
//     myServo.write(angle);  // Write angle to servo
//   }
// }

// // Reconnect function to maintain MQTT connection
// void reconnect() {
//   while (!client.connected()) {
//     Serial.print("Attempting MQTT connection...");
//     String client_id = "esp32-client-" + String(WiFi.macAddress());
//     if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
//       Serial.println("Connected to MQTT Broker");
//       client.subscribe("esp32/servoControl");  // Resubscribe to the topic
//     } else {
//       Serial.print("Failed with state ");
//       Serial.println(client.state());
//       delay(2000);
//     }
//   }
// }








void handleSensors() {
  // Read the IR sensor value
  int irvalue = analogRead(irSensorPin);
  int Distance_cm = map(irvalue, 0, 4095, 0, 12);
  Serial.print("IR Sensor Value: ");
  Serial.println(Distance_cm);

// Read the Flame Sensor value (analog)
    int flameSensorValue = analogRead(flameSensorPin); // Read analog value
    int flameThreshold = 1000; // Set an appropriate threshold
    Serial.print("Flame Sensor Value: ");
    Serial.println(flameSensorValue);

  // Read Smoke and Gas Sensor values (analog)
  int smokeSensorValue = analogRead(smokeSensorPin);
  int smokePercentage = map(smokeSensorValue, 300, 10000, 0, 100);
  Serial.print("Smoke Sensor Value: ");
  Serial.print(smokePercentage);
  Serial.println("%");

  int gasSensorValue = analogRead(gasSensorPin);
  int gasPercentage = map(gasSensorValue, 300, 10000, 0, 100);
  Serial.print("Gas Sensor Value: ");
  Serial.print(gasPercentage);
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
  // client.publish("esp32/irSensor", String(Distance_cm).c_str());
  // client.publish("esp32/flameSensor", flameSensorValue > 2000 ? "Detected" : "Not Detected");
  // client.publish("esp32/smokeSensor", String(smokePercentage).c_str());
  // client.publish("esp32/gasSensor", String(gasPercentage).c_str());
  // client.publish("esp32/temperatureSensor", String(temperature).c_str());
  // client.publish("esp32/humiditySensor", String(humidity).c_str());


  // Check IR sensor to open door (servo) if a person is detected
  if (Distance_cm < 4) {
    // Serial.println("Person detected, opening door");
    // lcd.clear();
    // lcd.print("Person detected");
    myServo.write(180);  // Move servo to open door
    delay(5000);         // Keep door open for 5 seconds
    myServo.write(0);    // Close door
  }

  // Check temperature and control fan1
  if (temperature >= 25) {
    Serial.println("Temperature >= 25°C, turning on fan1");
    digitalWrite(fan1, HIGH);
  } else {
    digitalWrite(fan1, LOW);
  }

  // Check for fire, gas, or smoke detection and control fan2, LED, and buzzer
  if (flameSensorValue < flameThreshold) {
    Serial.println("Alert: Fire Detected");
    // lcd.clear();
    // lcd.print("Alert Detected!");
    digitalWrite(ledPin, HIGH);
    digitalWrite(buzzerPin, HIGH);
    digitalWrite(fan2, HIGH);
    digitalWrite(fan1, LOW);

  } else if (smokePercentage > 50) {
    Serial.println("Alert: Smoke Detected");
    // lcd.clear();
    // lcd.print("Alert Detected!");
    digitalWrite(ledPin, HIGH);
    digitalWrite(buzzerPin, HIGH);
    digitalWrite(fan2, HIGH);
    digitalWrite(fan1, LOW);

    // client.publish("esp32/alerts", "Smoke detected!");
  } else if (gasPercentage > 50) {
    Serial.println("Alert: Gas Detected");
    // lcd.clear();
    // lcd.print("Alert Detected!");
    digitalWrite(ledPin, HIGH);
    digitalWrite(buzzerPin, HIGH);
    digitalWrite(fan1, LOW);
    digitalWrite(fan2, HIGH);
    // client.publish("esp32/alerts", "Gas detected!");
  } else {
    digitalWrite(ledPin, LOW);
    digitalWrite(buzzerPin, LOW);
    digitalWrite(fan2, LOW);
  }


  // Display temperature and humidity on LCD
  // lcd.setCursor(0, 0);
  // lcd.print("Temp: ");
  // lcd.print(temperature);
  // lcd.setCursor(0, 1);
  // lcd.print("Humidity: ");
  // lcd.print(humidity);
  delay(1000);
}


void setup() {
  // Initialize Serial Monitor
Serial.begin(115200);
  // Initialize DHT Sensor
  dht.begin();

  // Initialize LCD
  // lcd.begin(16, 2);
  // lcd.backlight();

  // Initialize GPIO pins
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(flameSensorPin, INPUT);
  pinMode(irSensorPin,INPUT);
  pinMode(smokeSensorPin, INPUT);
  pinMode(gasSensorPin, INPUT);
  pinMode(DHTSensorPin, INPUT);

  pinMode(fan1, OUTPUT);
  pinMode(fan2, OUTPUT);
  digitalWrite(ledPin, LOW);
  digitalWrite(buzzerPin, LOW);
  myServo.attach(servoPin);

  // // Connect to Wi-Fi
  // WiFi.begin(ssid, password);
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(1000);
  //   Serial.println("Connecting to WiFi...");
  //   lcd.setCursor(0, 1);
  //   lcd.print("Connecting to WiFi");
  // }
  // Serial.println("Connected to WiFi");
  // lcd.clear();
  // lcd.print("WiFi Connected");

  // // Attach the certificate bundle to the secure client
  // wifiClient.setInsecure();

  // // Set up the MQTT client
  // client.setServer(mqtt_broker, mqtt_port);
  // client.setCallback(callback);

  // // Connect to the MQTT Broker
  // while (!client.connected()) {
  //   Serial.print("Connecting to MQTT Broker...");
  //   String client_id = "esp32-client-" + String(WiFi.macAddress());
  //   if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
  //     Serial.println("Connected to MQTT Broker");
  //     client.subscribe("esp32/servoControl");  // Subscribe to the servo control topic
  //   } else {
  //     Serial.print("Failed with state ");
  //     Serial.println(client.state());
  //     delay(2000);
  //   }
  // }

  // Attach the servo motor to the specified pin
}

void loop() {
  // Ensure the client stays connected and processes incoming messages
  // if (!client.connected()) {
  //   reconnect();
  // }
  // client.loop();

  handleSensors();  // Call the function to handle sensor data
}
