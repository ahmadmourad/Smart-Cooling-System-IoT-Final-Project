#include <WiFi.h>               //allows the ESP32 to connect to Wi-Fi networks
#include <WiFiClientSecure.h>   //provides a secure connection over Wi-Fi using SSL/TLS
#include <PubSubClient.h>       //used for MQTT communication.
#include <ESP32Servo.h>         // used for Servo
#include <HardwareSerial.h>     //used to manage hardware serial communication on the ESP32
#include <Wire.h>               //I2C communication
#include <LiquidCrystal_I2C.h>  // used to control an LCD display with an I2C interface
#include "secrets.h"

// defining uart pins used for transmission
#define RXp2 16
#define TXp2 17

LiquidCrystal_I2C lcd(0x27, 16, 2);  // initialize the Address and dimensions of LCD
Servo myServo;                       //initialize object From Servo Library

// define the Pins of Our Components
const int servoPin = 13;
const int ledPin = 2;
const int buzzerPin = 15;
const int fan1 = 27;
const int fan2 = 14;

//defining required variables For Our Sensors , fan and air pump
float temperature, humidity;
int GasMQ5Percentage, MQ2SmokePercentage, Distance_cm, flameSensorValue, fan1State, fan2State;
char keyPressed = ' ';
bool AutoMode = true;  // Start system in automatic mode
bool autoModeflutterFan1 = true;
bool autoModeflutterFan2 = true;

//create an instance of the HardwareSerial class, specifically for serial communication on the ESP32.
HardwareSerial mySerial(1);

// Initialize Wi-Fi and MQTT client objects
WiFiClientSecure wifiClient;
PubSubClient client(wifiClient);

// Wi-Fi credentials
const char* ssid = YOUR_SSID;
const char* password = YOUR_PASS;

// MQTT Broker details
const char* mqtt_broker = MQTT_BROKER;
const char* mqtt_username = MQTT_USERNAME;
const char* mqtt_password = MQTT_PASS;
const int mqtt_port = 8883;


// Callback function to handle messages received on subscribed topics
//topic: A pointer to a character array (C-style string) that holds the name of the topic on which the message was received.
//byte* payload: A pointer to a byte array containing the message data received.
// int length: The length of the message (number of bytes in the payload)
void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  if (String(topic) == "esp32/fan1") {
    if (message == "AUTO") {
      autoModeflutterFan1 = true;
    } else {
      autoModeflutterFan1 = false;
      if (message == "ON") {
        digitalWrite(fan1, HIGH);
      } else if (message == "OFF") {
        digitalWrite(fan1, LOW);
      }
    }
  } else if (String(topic) == "esp32/fan2") {
    if (message == "AUTO") {
      autoModeflutterFan2 = true;
    } else {
      autoModeflutterFan2 = false;
      if (message == "ON") {
        digitalWrite(fan2, HIGH);
      } else if (message == "OFF") {
        digitalWrite(fan2, LOW);
      }
    }
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

//this is a function responsible to make the fans actuate automaticaly according to sensors readings
void controlFansAutomatically() {

  if (autoModeflutterFan1) {
    // Check temperature and control fan one
    if (temperature >= 30) {
      Serial.print("Temperature = ");
      Serial.print(temperature);
      Serial.println(" Temp >= 30 ,fan 1 is on");
      digitalWrite(fan1, HIGH);
    } else {
      digitalWrite(fan1, LOW);
    }
  }
  if (autoModeflutterFan2) {
    // Check for fire, gas, or smoke detection and control fan two(exhaust fan), LED, and buzzer
    int flameThreshold = 1000;
    if (flameSensorValue < flameThreshold) {   //---> Threshold For our Flame Sensor
      Serial.println("Alert: Fire Detected");  // Check Fire Detected if The flameSensorValue is Smaller than flameThreshold
      lcd.clear();
      lcd.print("Fire Detected!");
      digitalWrite(ledPin, HIGH);                        // turn on red led
      digitalWrite(buzzerPin, HIGH);                     //turn on buzzer
      digitalWrite(fan2, HIGH);                          //turn on air pump
      digitalWrite(fan1, LOW);                           // turn off fan
      client.publish("esp32/alerts", "Fire detected!");  // publish the Fire Detected message on MQTT broker

    } else if (GasMQ5Percentage > 5) {  // check if The GAS Sensor Value is bigger than 5
      Serial.println("Alert: Gas Detected, turning on exhaust fan!");
      lcd.clear();
      lcd.print("Gas Detected!");
      digitalWrite(ledPin, HIGH);                       // turn on red led
      digitalWrite(buzzerPin, HIGH);                    // turn on buzzer
      digitalWrite(fan2, HIGH);                         // turn on air pump
      digitalWrite(fan1, LOW);                          // turn off fan
      client.publish("esp32/alerts", "Gas detected!");  // publish the Gas Detected message on MQTT broker

    } else if (MQ2SmokePercentage > 20) {  // check if The Smoke Sensor Value is bigger than 20
      Serial.println("Alert: Smoke Detected turning on exhaust fan!");
      lcd.clear();
      lcd.print("Smoke Detected!");
      digitalWrite(ledPin, HIGH);     // turn on red led
      digitalWrite(buzzerPin, HIGH);  // turn on buzzer
      digitalWrite(fan1, LOW);        // turn off fan
      digitalWrite(fan2, HIGH);       // turn on air pump
      client.publish("esp32/alerts", "Smoke detected!");
    } else {
      digitalWrite(ledPin, LOW);     //turn off red led
      digitalWrite(buzzerPin, LOW);  // turn off buzzer
      digitalWrite(fan2, LOW);       // turn off air pump
    }
  }
}

// this function is responsible for manual control to the fans using keypad
void controlFansManually(char key) {
  switch (key) {
    case '1':
      digitalWrite(fan1, HIGH);
      break;
    case '2':
      digitalWrite(fan1, LOW);
      break;
    case '3':
      digitalWrite(fan2, HIGH);
      break;
    case '4':
      digitalWrite(fan2, LOW);
      break;
    case '5':
      digitalWrite(fan1, HIGH);
      digitalWrite(fan2, HIGH);
      break;
    case '6':
      digitalWrite(fan1, LOW);
      digitalWrite(fan2, LOW);
      break;
  }
}

void recieveSensors_Actuate() {

  if (mySerial.available()) {
    //if there is any data available to read from the serial connection mySerial, if data is available, it proceeds to read it.
    String receivedData = mySerial.readStringUntil('\n');  // reading the incoming data from the serial connection until it encounters a newline
    // Extract the mode ('A' or 'M')
    char mode = receivedData.charAt(0);
    AutoMode = (mode == 'A');
    receivedData = receivedData.substring(1);

    // Parse sensors data
    int commaIndex = receivedData.indexOf(',');
    temperature = receivedData.substring(0, commaIndex).toFloat();  //the data is assumed to be in CSV format we extract each sensor value by finding the position of the comma,
    receivedData = receivedData.substring(commaIndex + 1);          //splitting the string, and converting the extracted substring into the appropriate data type

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
    receivedData = receivedData.substring(commaIndex + 1);

    commaIndex = receivedData.indexOf(',');
    flameSensorValue = receivedData.substring(0, commaIndex).toInt();
    receivedData = receivedData.substring(commaIndex + 1);
    keyPressed = receivedData.charAt(0);

    if (AutoMode) {
      controlFansAutomatically();
    } else {
      controlFansManually(keyPressed);
    }

    // Condition For Open Door For Person
    if (Distance_cm < 4) {
      Serial.println("Person detected, opening door");
      lcd.clear();
      lcd.print("Person detected");
      myServo.write(180);  // Move servo to open door
      client.publish("esp32/servo", "180");
      delay(1000);
      myServo.write(0);  // Close door
      client.publish("esp32/servo", "0");
    }

    // Read and Publish fan state to MQTT topics
    fan1State = digitalRead(fan1);
    fan2State = digitalRead(fan2);
    if (fan1State == HIGH) {             //if fan is turned on
      client.publish("esp/fan1", "ON");  //publish ON message on MQTT broker
    } else {
      client.publish("esp/fan1", "OFF");  //publish OFF message on MQTT broker
    }

    if (fan2State == HIGH) {             //if air pump is turned on
      client.publish("esp/fan2", "ON");  //publish ON message on MQTT broker
    } else {
      client.publish("esp/fan2", "OFF");  //publish OFF message on MQTT broker
    }

    // Convert Signals From our Sensors to String and Publish them to MQTT broker
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

// print Readings
void print_sensors_reading() {
  Serial.print("Automatic Mode state: ");
  Serial.println(AutoMode);
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
  Serial.print("Pressed key: ");
  Serial.println(keyPressed);
  delay(400);
}

void setup() {

  Serial.begin(115200);                            // initalize The baud rate with 115200
  mySerial.begin(115200, SERIAL_8N1, RXp2, TXp2);  // UART1: RX on GPIO16, TX on GPIO17

  // define pin Mode
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(fan1, OUTPUT);
  pinMode(fan2, OUTPUT);
  pinMode(ledPin, OUTPUT);
  myServo.attach(servoPin);

  //setting up lcd
  Wire.begin(26, 25);  // Initialize I²C communication with custom SDA and SCL pins
  lcd.begin(16, 2);
  lcd.backlight();  //turn on the backlight of an LCD display

  // Define inital statue For Components
  digitalWrite(fan1, LOW);
  digitalWrite(fan2, LOW);
  digitalWrite(buzzerPin, LOW);
  myServo.write(0);  // Set servo to 0 degrees


  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {  // if we conncted to Wifi
    delay(1000);
    Serial.println("Connecting to WiFi...");
    lcd.setCursor(0, 0);
    lcd.println("Connecting to    ");
    lcd.setCursor(0, 1);
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
  while (!client.connected()) {  // if we not Connected with MQTT
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
  client.subscribe("esp32/fan1");
  client.subscribe("esp32/fan2");
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
