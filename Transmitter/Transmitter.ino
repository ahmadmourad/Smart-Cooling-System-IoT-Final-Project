#include <HardwareSerial.h>  //used to manage hardware serial communication on the ESP32
#include <DHT.h>             // used to manage the DHT Sensor

// DHT Sensor setup
#define DHTTYPE DHT11

// define pins for our Sensors
const int irSensorPin = 34; 
const int DHTSensorPin = 23;
const int flameSensorPin = 35;
const int MQ5GasPin = 33;
const int MQ2SmokePin = 32;

DHT dht(DHTSensorPin, DHTTYPE); // initialize object for DHT Sensor

// defining needed variables 
int irvalue, Distance_cm, flameSensorValue , GasMQ5SensorValue, GasMQ5Percentage , MQ2SmokeSensorValue, MQ2SmokePercentage;
float temperature, humidity;

HardwareSerial mySerial(1);// used to create an instance of the HardwareSerial class, specifically for serial communication on the ESP32.

void readSensors() {

  // Read the IR sensor value
  irvalue = analogRead(irSensorPin);
  Distance_cm = map(irvalue, 0, 4095, 0, 12);  // mapping IR Sensor Values to CM from 0 to 12 cm

  // Read the Flame Sensor value (analog)
  flameSensorValue = analogRead(flameSensorPin); // Read analog value

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

void transmit_sensors_reading(){

mySerial.print(temperature);   // Sends the temperature value followed by a comma
  mySerial.print(",");   
  mySerial.print(humidity);    // Sends the Humidity value followed by a comma
  mySerial.print(",");
  mySerial.print(GasMQ5Percentage);  // Sends the Gas value followed by a comma
  mySerial.print(",");  
  mySerial.print(MQ2SmokePercentage);  // Sends the Smoke value followed by a comma
  mySerial.print(",");
  mySerial.print(Distance_cm);       // Sends the Distance value followed by a comma 
  mySerial.print(",");
  mySerial.println(flameSensorValue);  // Sends the Flame value followed by a comma
  delay(1000);

}

void setup() {

  // Initialize Serial Monitor
  mySerial.begin(115200, SERIAL_8N1, 16, 17);  // UART1: RX on GPIO16, TX on GPIO17
  
  // Initialize DHT Sensor
  dht.begin();
  // Initialize GPIO pins for Our Sensors
  pinMode(flameSensorPin, INPUT);
  pinMode(irSensorPin,INPUT);
  pinMode(MQ5GasPin, INPUT);
  pinMode(MQ2SmokePin, INPUT);
  pinMode(DHTSensorPin, INPUT);

}

void loop() {
  readSensors(); 
  transmit_sensors_reading();
}
