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

}

void loop() {
  readSensors(); 
  transmit_sensors_reading();
}