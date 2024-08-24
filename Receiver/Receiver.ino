#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>
#include <HardwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// defining uart pins used for transmission 
#define RXp2 16
#define TXp2 17

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo myServo;
const int servoPin = 13;
const int ledPin = 2;
const int buzzerPin = 15;
const int fan1 = 27;
const int fan2 = 14;

//defining required variables
float temperature , humidity;
int GasMQ5Percentage,MQ2SmokePercentage,Distance_cm,flameSensorValue;


HardwareSerial mySerial(1);


// Wi-Fi credentials
const char* ssid = "realme9i";
const char* password = "12345678";

// MQTT Broker details
const char* mqtt_broker = "b1a69793bf6e48eca7ab350e88d63dd4.s1.eu.hivemq.cloud";
const char* mqtt_username = "hivemq.webclient.1724491871036";
const char* mqtt_password = "yA:P!ef4.#RU0kWB18ra";
const int mqtt_port = 8883;

// Initialize Wi-Fi and MQTT client objects
WiFiClientSecure wifiClient;
PubSubClient client(wifiClient);

// Callback function to handle messages received on subscribed topics
void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
}
// Reconnect function to maintain MQTT connection
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String client_id = "esp32-client-" + String(WiFi.macAddress());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Connected to MQTT Broker");
    } else {
      Serial.print("Failed with state ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}




void recieveSensors_Actuate(){

if (mySerial.available()) { 
  
  String receivedData = mySerial.readStringUntil('\n');

  int commaIndex = receivedData.indexOf(',');
  temperature = receivedData.substring(0, commaIndex).toFloat();
  receivedData = receivedData.substring(commaIndex + 1);
  
  commaIndex = receivedData.indexOf(',');
  humidity = receivedData.substring(0, commaIndex).toFloat();
  receivedData = receivedData.substring(commaIndex + 1);

  commaIndex = receivedData.indexOf(',');
  GasMQ5Percentage = receivedData.substring(0, commaIndex).toInt();
  receivedData = receivedData.substring(commaIndex + 1);

  commaIndex = receivedData.indexOf(',');
  MQ2SmokePercentage = receivedData.substring(0, commaIndex).toInt();
  receivedData = receivedData.substring(commaIndex + 1);
  
  commaIndex = receivedData.indexOf(',');
  Distance_cm = receivedData.substring(0, commaIndex).toInt();

  flameSensorValue = receivedData.substring(commaIndex + 1).toInt();


if (Distance_cm < 4) {
    Serial.println("Person detected, opening door");
    lcd.clear();
    lcd.print("Person detected");
    myServo.write(180);  // Move servo to open door
  client.publish("esp32/servo","180");

    delay(1000);        
    myServo.write(0);    // Close door
      client.publish("esp32/servo","0");

  }

  // Check temperature and control fan one
  if (temperature >= 30) {
    Serial.print("Temperature = ");
    Serial.print(temperature);
    Serial.println(" Temp >= 30 ,fan 1 is on");
    digitalWrite(fan1, HIGH);
  } else {
    digitalWrite(fan1, LOW);
  }

  // Check for fire, gas, or smoke detection and control fan two(exhaust fan), LED, and buzzer
  int flameThreshold = 1000; 
  if (flameSensorValue < flameThreshold) {
    Serial.println("Alert: Fire Detected");
    lcd.clear();
    lcd.print("Fire Detected!");
    digitalWrite(ledPin, HIGH);
    digitalWrite(buzzerPin, HIGH);
    digitalWrite(fan2, HIGH);
    digitalWrite(fan1, LOW);
    client.publish("esp32/alerts", "Fire detected!");

  } else if (GasMQ5Percentage > 10) {
    Serial.println("Alert: Gas Detected, turning on exhaust fan!");
    lcd.clear();
    lcd.print("Gas Detected!");
    digitalWrite(ledPin, HIGH);
    digitalWrite(buzzerPin, HIGH);
    digitalWrite(fan2, HIGH);
    digitalWrite(fan1, LOW);
    client.publish("esp32/alerts", "Smoke detected!");

  } else if (MQ2SmokePercentage > 25) {
    Serial.println("Alert: Smoke Detected turning on exhaust fan!");
    lcd.clear();
    lcd.print("Smoke Detected!");
    digitalWrite(ledPin, HIGH);
    digitalWrite(buzzerPin, HIGH);
    digitalWrite(fan1, LOW);
    digitalWrite(fan2, HIGH);
    client.publish("esp32/alerts", "Gas detected!");
  } else {
    digitalWrite(ledPin, LOW);
    digitalWrite(buzzerPin, LOW);
    digitalWrite(fan2, LOW);
  }

// Publish sensor readings to MQTT topics
  client.publish("esp32/irSensor", String(Distance_cm).c_str());
  client.publish("esp32/flameSensor", String(flameSensorValue).c_str());
  client.publish("esp32/gasSensor", String(GasMQ5Percentage).c_str());
  client.publish("esp32/smokeSensor", String(MQ2SmokePercentage).c_str());
  client.publish("esp32/temperatureSensor", String(temperature).c_str());
  client.publish("esp32/humiditySensor", String(humidity).c_str());


 // Display temperature and humidity on LCD
  delay(700);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print(" C");
  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(humidity);
  lcd.print("%");

}
}

void print_sensors_reading(){
  Serial.print("IR Sensor Value: ");
  Serial.println(Distance_cm);
  Serial.print("Flame Sensor Value: ");
  Serial.println(flameSensorValue);
  Serial.print("Gas Sensor Value: ");
  Serial.print(GasMQ5Percentage);
  Serial.println("%");
  Serial.print("Smoke Sensor Value: ");
  Serial.print(MQ2SmokePercentage);
  Serial.println("%");
  Serial.print("Temperature Value: ");
  Serial.print(temperature);
  Serial.println(" °C");
  Serial.print("Humidity Value: ");
  Serial.print(humidity);
  Serial.println(" %");
}



void setup() {

  Serial.begin(115200);
  mySerial.begin(115200, SERIAL_8N1, RXp2, TXp2);  // UART1: RX on GPIO16, TX on GPIO17    

  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(fan1, OUTPUT);
  pinMode(fan2, OUTPUT);
  pinMode(ledPin, OUTPUT);
  myServo.attach(servoPin);

  //setting up lcd 
  Wire.begin(26, 25);
  lcd.begin(16, 2);
  lcd.backlight();

  digitalWrite(fan1, LOW);
  digitalWrite(fan2, LOW);
  digitalWrite(buzzerPin, LOW);
  myServo.write(0);  // Set servo to 0 degrees




  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    lcd.setCursor(0, 0);
    lcd.println("Connecting to    ");
    lcd.setCursor(0,1);
    lcd.print("WiFi");
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
    } else {
      Serial.print("Failed with state ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}




void loop() {

//  Ensure the client stays connected and processes incoming messages
if (!client.connected()) {
    reconnect();
  }
client.loop();

recieveSensors_Actuate();
print_sensors_reading();


}
