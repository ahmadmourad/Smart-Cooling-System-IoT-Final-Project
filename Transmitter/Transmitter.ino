// #include <WiFi.h>
// #include <WiFiClientSecure.h>
// #include <PubSubClient.h>
#include <HardwareSerial.h>
#include <DHT.h>

// DHT Sensor setup
#define DHTTYPE DHT11

const int irSensorPin = 34;
const int DHTSensorPin = 23;
const int flameSensorPin = 35;
const int MQ5GasPin = 33;
const int MQ2SmokePin = 32;

DHT dht(DHTSensorPin, DHTTYPE);

// defining needed variables 
int irvalue, Distance_cm, flameSensorValue , GasMQ5SensorValue, GasMQ5Percentage , MQ2SmokeSensorValue, MQ2SmokePercentage;
float temperature, humidity;


HardwareSerial mySerial(1);


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




void readSensors() {

  // Read the IR sensor value
  irvalue = analogRead(irSensorPin);
  Distance_cm = map(irvalue, 0, 4095, 0, 12);

  // Read the Flame Sensor value (analog)
  flameSensorValue = analogRead(flameSensorPin); // Read analog value

  // Read Gas Sensor values (analog)
  GasMQ5SensorValue = analogRead(MQ5GasPin);
  GasMQ5Percentage = map(GasMQ5SensorValue, 0, 10000, 0, 100);

  // Read Smoke and Gas Sensor values (analog)
  MQ2SmokeSensorValue = analogRead(MQ2SmokePin);
  MQ2SmokePercentage = map(MQ2SmokeSensorValue, 300, 10000, 0, 100);

  // Read temperature and humidity
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

}

void transmit_sensors_reading(){

mySerial.print(temperature);
  mySerial.print(",");
  mySerial.print(humidity);
  mySerial.print(",");
  mySerial.print(GasMQ5Percentage);
  mySerial.print(",");
  mySerial.print(MQ2SmokePercentage);
  mySerial.print(",");
  mySerial.print(Distance_cm);
  mySerial.print(",");
  mySerial.println(flameSensorValue);

 
// Publish sensor data to MQTT topics
  // client.publish("esp32/irSensor", String(Distance_cm).c_str());
  // client.publish("esp32/flameSensor", flameSensorValue > 2000 ? "Detected" : "Not Detected");
  // client.publish("esp32/smokeSensor", String(GasMQ5Percentage).c_str());
  // client.publish("esp32/gasSensor", String(MQ2SmokePercentage).c_str());
  // client.publish("esp32/temperatureSensor", String(temperature).c_str());
  // client.publish("esp32/humiditySensor", String(humidity).c_str());

    delay(1000);

}

void setup() {

  // Initialize Serial Monitor
  mySerial.begin(115200, SERIAL_8N1, 16, 17);  // UART1: RX on GPIO16, TX on GPIO17
  
  // Initialize DHT Sensor
  dht.begin();
  // Initialize GPIO pins
  pinMode(flameSensorPin, INPUT);
  pinMode(irSensorPin,INPUT);
  pinMode(MQ5GasPin, INPUT);
  pinMode(MQ2SmokePin, INPUT);
  pinMode(DHTSensorPin, INPUT);


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

}

void loop() {
  // Ensure the client stays connected and processes incoming messages
  // if (!client.connected()) {
  //   reconnect();
  // }
  // client.loop();
  readSensors(); 
  transmit_sensors_reading();
}