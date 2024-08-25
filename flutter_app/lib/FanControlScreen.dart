import 'package:flutter/material.dart';
import 'MQTT.dart';

class FanControlScreen extends StatelessWidget {
  final MQTTClientWrapper mqttClientWrapper;

  FanControlScreen({required this.mqttClientWrapper});

  void _toggleDevice(String device, bool turnOn) {
    mqttClientWrapper.publishMessage('esp32/$device', turnOn ? 'ON' : 'OFF');
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Control System'),
        backgroundColor: Color.fromARGB(124, 150, 18, 157),
      ),
      body: Center(
        child: Padding(
          padding: const EdgeInsets.all(16.0),
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              // Fan Control Section
              Row(
                mainAxisAlignment: MainAxisAlignment.center,
                children: [
                  Icon(Icons.air, size: 30, color: Colors.blue),
                  SizedBox(width: 10),
                  Text(
                    'Fan Control',
                    style: TextStyle(fontSize: 24),
                  ),
                ],
              ),
              SizedBox(height: 10),
              ElevatedButton(
                onPressed: () => _toggleDevice('fan1', true),
                style: ElevatedButton.styleFrom(
                  backgroundColor: Colors.green, // Green color for ON
                ),
                child: Text(
                  'ON',
                  style: TextStyle(color: Colors.white), // Text color for ON
                ),
              ),
              ElevatedButton(
                onPressed: () => _toggleDevice('fan1', false),
                style: ElevatedButton.styleFrom(
                  backgroundColor: Colors.red, // Red color for OFF
                ),
                child: Text(
                  'OFF',
                  style: TextStyle(color: Colors.white), // Text color for OFF
                ),
              ),
              SizedBox(height: 30), // Space between sections

              // Air Pump System Section
              Row(
                mainAxisAlignment: MainAxisAlignment.center,
                children: [
                  Icon(Icons.water_damage, size: 30, color: Colors.green),
                  SizedBox(width: 10),
                  Text(
                    'Air Pump System',
                    style: TextStyle(fontSize: 24),
                  ),
                ],
              ),
              SizedBox(height: 10),
              ElevatedButton(
                onPressed: () => _toggleDevice('airPump', true),
                style: ElevatedButton.styleFrom(
                  backgroundColor: Colors.green, // Green color for ON
                ),
                child: Text(
                  'ON',
                  style: TextStyle(color: Colors.white), // Text color for ON
                ),
              ),
              ElevatedButton(
                onPressed: () => _toggleDevice('airPump', false),
                style: ElevatedButton.styleFrom(
                  backgroundColor: Colors.red, // Red color for OFF
                ),
                child: Text(
                  'OFF',
                  style: TextStyle(color: Colors.white), // Text color for OFF
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }
}
