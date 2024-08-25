import 'package:flutter/material.dart';
import 'MQTT.dart';

class FanControlScreen extends StatelessWidget {
  final MQTTClientWrapper mqttClientWrapper;

  FanControlScreen({required this.mqttClientWrapper});

  void _toggleDevice(String device, String message) {
    mqttClientWrapper.publishMessage('esp32/$device', message);
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Control System'),
        backgroundColor: Color.fromARGB(124, 150, 18, 157),
      ),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
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
              onPressed: () {
                _toggleDevice('fan1', 'ON');
              },
              child: Text('Turn On'),
            ),
            ElevatedButton(
              onPressed: () {
                _toggleDevice('fan1', 'OFF');
              },
              child: Text('Turn Off'),
            ),
            ElevatedButton(
              onPressed: () {
                _toggleDevice('fan1', 'AUTO');
              },
              child: Text('Auto Mode'),
            ),
            SizedBox(height: 20), // add some space between sections

            // Air Pump System Section
            Row(
              mainAxisAlignment: MainAxisAlignment.center,
              children: [
                Icon(Icons.heat_pump, size: 30, color: Colors.green),
                SizedBox(width: 10),
                Text(
                  'Air Pump System',
                  style: TextStyle(fontSize: 24),
                ),
              ],
            ),
            SizedBox(height: 10),
            ElevatedButton(
              onPressed: () {
                _toggleDevice('airPump', 'ON');
              },
              child: Text('Turn On'),
            ),
            ElevatedButton(
              onPressed: () {
                _toggleDevice('airPump', 'OFF');
              },
              child: Text('Turn Off'),
            ),
            ElevatedButton(
              onPressed: () {
                _toggleDevice('airPump', 'AUTO');
              },
              child: Text('Auto Mode'),
            ),
          ],
        ),
      ),
    );
  }
}
