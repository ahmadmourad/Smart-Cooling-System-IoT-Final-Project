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
    delay(1000);        
    myServo.write(0);    // Close door
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

  int flameThreshold = 1000; 
  // Check for fire, gas, or smoke detection and control fan two(exhaust fan), LED, and buzzer
  if (flameSensorValue < flameThreshold) {
    Serial.println("Alert: Fire Detected");
    lcd.clear();
    lcd.print("Fire Detected!");
    digitalWrite(ledPin, HIGH);
    digitalWrite(buzzerPin, HIGH);
    digitalWrite(fan2, HIGH);
    digitalWrite(fan1, LOW);

  } else if (GasMQ5Percentage > 10) {
    Serial.println("Alert: Gas Detected, turning on exhaust fan!");
    lcd.clear();
    lcd.print("Gas Detected!");
    digitalWrite(ledPin, HIGH);
    digitalWrite(buzzerPin, HIGH);
    digitalWrite(fan2, HIGH);
    digitalWrite(fan1, LOW);

    // client.publish("esp32/alerts", "Smoke detected!");
  } else if (MQ2SmokePercentage > 30) {
    Serial.println("Alert: Smoke Detected turning on exhaust fan!");
    lcd.clear();
    lcd.print("Smoke Detected!");
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

}

void loop() {
recieveSensors_Actuate();
print_sensors_reading();

}
