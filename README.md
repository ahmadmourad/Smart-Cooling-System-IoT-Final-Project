# Smart-Cooling-System-IoT-Final-Project

The project involves building a Smart Air System working based on  a sensor monitoring and control system using an ESP32 microcontroller. The system reads data from various sensors (temperature, humidity, IR, flame, gas, and smoke) and the Fan and Air pump take appropriate actions based on the sensor readings. as well as we create a Flutter mobile Application to control the Fan and Air pump.

Fan 2 Control (Gas, Smoke, or Fire Detection)
Turns On: When any of the following conditions are met:
The gas sensor (MQ5) detects gas above a certain threshold.
The smoke sensor (MQ2) detects smoke above a certain threshold.
The flame sensor detects fire.
Turns Off: When all the following conditions are met:
The gas sensor detects gas below the threshold.
The smoke sensor detects smoke below the threshold.
The flame sensor does not detect fire.

## MQTT
Wi-Fi and MQTT Integration: The Two ESP32s also connect to a Wi-Fi network and communicate with an MQTT broker. It publishes sensor readings and control actions to specific MQTT topics and receives control commands via MQTT.


## Node-Red
We Connect The MQTT Serve With NODE RED by passing into MQTTin flow:-
1-Cluster URL 
2- Username
3- Password
4-Topic Name 
![image](https://github.com/user-attachments/assets/4f742314-8840-4dc9-b95b-7a6870e6c36c)

### After that we display the Signals that are coming From Sensors into Line Charts, Graphs, and Gauges.
![image](https://github.com/user-attachments/assets/ebe24ca7-50f0-40d3-98fa-670b257c70db)

## Mobile Application!

### Home Screen:-
The user can select sign in or sign up to access our App. After that, we make authenticated with Firebase with the email and passwords of the user

![WhatsApp Image 2024-08-26 at 03 46 19_f0a9816e](https://github.com/user-attachments/assets/53f1fbf9-034a-4cde-9f03-5b4f8ef79c86)


### Sign in /sign up Screens:-
### We connect The App with Firebase to Store the database of users 
![image](https://github.com/user-attachments/assets/03eaac55-1287-4148-b925-a0efdc789e63)

![image](https://github.com/user-attachments/assets/44859763-7248-4bf1-a1eb-1b726b39ec57)

### Sensors Dashboard That Contains Graphs For each Sensor to Show us the Signals That Coming from Our Sensors:-
![image](https://github.com/user-attachments/assets/e04d5f17-4571-4397-a9b3-4d04dc0111c4)

### We Can Control our Fan and Air pump through the MQTT Server where we Submit The statute of Actuators that we would prefer, and we can easily subscribe on The topic of Fan and Air Pump :-
![image](https://github.com/user-attachments/assets/e24db329-1fda-4b10-9294-a2795877178d)








