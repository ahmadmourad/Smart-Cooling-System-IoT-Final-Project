#include <WiFi.h>
#include <Firebase_ESP_Client.h>
//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"
#include <HardwareSerial.h>  //used to manage hardware serial communication on the ESP32
#include <DHT.h>             // used to manage the DHT Sensor
#include <Keypad.h>
#include "secrets.h"

// DHT Sensor setup
#define DHTTYPE DHT11

// define pins for our Sensors
const int irSensorPin = 34;
const int DHTSensorPin = 23;
const int flameSensorPin = 35;
const int MQ5GasPin = 33;
const int MQ2SmokePin = 32;

DHT dht(DHTSensorPin, DHTTYPE);  // initialize object for DHT Sensor

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte rowPins[ROWS] = { 13, 12, 14, 27 };
byte colPins[COLS] = { 5, 18, 19, 21 };
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// defining needed variables
int irvalue, Distance_cm, flameSensorValue, GasMQ5SensorValue, GasMQ5Percentage, MQ2SmokeSensorValue, MQ2SmokePercentage;
float temperature, humidity;
char keyPressed = ' ';
bool AutoMode = true;  // Start system in automatic mode

HardwareSerial mySerial(1);  // used to create an instance of the HardwareSerial class, specifically for serial communication on the ESP32.

const char* ssid = YOUR_SSID;
const char* password = YOUR_PASS;

FirebaseData Fbdata;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
unsigned long retrieveDataPrevMillis = 0;
bool firebaseReady = false;


void readSensors() {
  // Read the IR sensor value
  irvalue = analogRead(irSensorPin);
  Distance_cm = map(irvalue, 0, 4095, 0, 12);  // mapping IR Sensor Values to CM from 0 to 12 cm

  // Read the Flame Sensor value (analog)
  flameSensorValue = analogRead(flameSensorPin);

  // Read Gas Sensor values (analog)
  GasMQ5SensorValue = analogRead(MQ5GasPin);
  GasMQ5Percentage = map(GasMQ5SensorValue, 0, 10000, 0, 100);  // mapping IR Gas Sensor Values from 0 to 100

  // Read Smoke and Gas Sensor values (analog)
  MQ2SmokeSensorValue = analogRead(MQ2SmokePin);
  MQ2SmokePercentage = map(MQ2SmokeSensorValue, 300, 10000, 0, 100);  // mapping IR Smoke Sensor Values from 0 to 100

  // Read temperature and humidity
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
}

void readKeypad() {
  // Read the key from the keypad
  char key = keypad.getKey();

  if (key) {           // If a key is pressed
    if (key == 'A') {  // Switch to automatic mode
      AutoMode = true;
      keyPressed = ' ';
    } else {  // Switch to manual mode and store the key
      AutoMode = false;
      keyPressed = key;
    }
  }
}

void transmit_sensors_keypad_reading() {
  mySerial.print(AutoMode ? "A" : "M");  // Send 'A' for automatic mode or 'M' for manual mode
  mySerial.print(temperature);           // Sends the temperature value followed by a comma
  mySerial.print(",");
  mySerial.print(humidity);  // Sends the Humidity value followed by a comma
  mySerial.print(",");
  mySerial.print(GasMQ5Percentage);  // Sends the Gas value followed by a comma
  mySerial.print(",");
  mySerial.print(MQ2SmokePercentage);  // Sends the Smoke value followed by a comma
  mySerial.print(",");
  mySerial.print(Distance_cm);  // Sends the Distance value followed by a comma
  mySerial.print(",");
  mySerial.print(flameSensorValue);  // Sends the Flame value followed by a comma
  mySerial.print(",");
  mySerial.println(keyPressed);
}


void sendSensorReadingsToFirebase(float temperature, float humidity, int Distance_cm, int flameSensorValue, float GasMQ5Percentage, float MQ2SmokePercentage) {
  // Send temperature to Firebase
  if (Firebase.RTDB.setFloat(&Fbdata, "/sensorReadings/temperature", temperature)) {
    Serial.println("Temperature data sent successfully");
  } else {
    Serial.println(Fbdata.errorReason());
  }

  // Send humidity to Firebase
  if (Firebase.RTDB.setFloat(&Fbdata, "/sensorReadings/humidity", humidity)) {
    Serial.println("Humidity data sent successfully");
  } else {
    Serial.println(Fbdata.errorReason());
  }

  // Send distance to Firebase
  if (Firebase.RTDB.setInt(&Fbdata, "/sensorReadings/distance_cm", Distance_cm)) {
    Serial.println("Distance data sent successfully");
  } else {
    Serial.println(Fbdata.errorReason());
  }

  // Send flame sensor value to Firebase
  if (Firebase.RTDB.setInt(&Fbdata, "/sensorReadings/flameSensorValue", flameSensorValue)) {
    Serial.println("Flame sensor data sent successfully");
  } else {
    Serial.println(Fbdata.errorReason());
  }

  // Send gas sensor percentage to Firebase
  if (Firebase.RTDB.setFloat(&Fbdata, "/sensorReadings/gasMQ5Percentage", GasMQ5Percentage)) {
    Serial.println("Gas sensor data sent successfully");
  } else {
    Serial.println(Fbdata.errorReason());
  }

  // Send smoke sensor percentage to Firebase
  if (Firebase.RTDB.setFloat(&Fbdata, "/sensorReadings/MQ2SmokePercentage", MQ2SmokePercentage)) {
    Serial.println("Smoke sensor data sent successfully");
  } else {
    Serial.println(Fbdata.errorReason());
  }
}


void setup() {
  Serial.begin(115200);
  // Initialize Serial Monitor
  mySerial.begin(115200, SERIAL_8N1, 16, 17);  // UART1: RX on GPIO16, TX on GPIO17

  // Initialize DHT Sensor
  dht.begin();
  // Initialize GPIO pins for Our Sensors
  pinMode(flameSensorPin, INPUT);
  pinMode(irSensorPin, INPUT);
  pinMode(MQ5GasPin, INPUT);
  pinMode(MQ2SmokePin, INPUT);
  pinMode(DHTSensorPin, INPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");


  // Assigning API key
  config.api_key = YOUR_GOOGLE_APIKEY;
  // Assigning RTDB URL
  config.database_url = DB_URL;
  // Assigning user authentication credentials
  auth.user.email = FIREBASE_EMAIL;
  auth.user.password = FIREBASE_PASS;
  // Initializing Firebase
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  if (auth.token.uid.length() > 0) {
    Serial.println("Signed in to firebase successfully");
    firebaseReady = true;
  } else {
    Serial.println("Failed to sign in");
  }
  config.token_status_callback = tokenStatusCallback;
}

void loop() {
  readSensors();
  readKeypad();
  transmit_sensors_keypad_reading();
  sendSensorReadingsToFirebase(temperature, humidity, Distance_cm, flameSensorValue, GasMQ5Percentage, MQ2SmokePercentage);
}
