import 'package:flutter/material.dart';
import 'MQTT.dart';

class FanControlScreen extends StatelessWidget {
  final MQTTClientWrapper mqttClientWrapper;

  FanControlScreen({required this.mqttClientWrapper});

  // Publishes a message to the MQTT broker to toggle a device on or off

  // The device name is added to the 'esp32/' topic and the message is published to the resulting topic

  /// [device] is the name of the device to toggle like 'fan1' or 'fan2'
  /// [message] is the message to publish to the MQTT broker, e.g. 'ON' or 'OFF' or 'AUTO'
  void _toggleDevice(String device, String message) {
    mqttClientWrapper.publishMessage('esp32/$device', message);
  }

  @override

  /// Building the UI for the FanControlScreen widget

  /// This returns a Scaffold widget with an AppBar and a body containing a vertical column
  /// with a fan control section, an air pump control section, and ElevatedButton widgets
  /// for controlling the fan and air pump.
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Control System'), // Screen title
        backgroundColor: Color.fromARGB(124, 150, 18, 157), // Appbar color
      ),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center, // Vertical Axis
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
              onPressed: () {
                _toggleDevice(
                    'fan1', 'ON'); // Publishes 'ON' message to 'esp32/fan1'
              },
              child: Text('Turn On'),
            ),
            ElevatedButton(
              onPressed: () {
                _toggleDevice(
                    'fan1', 'OFF'); // Publishes 'OFF' message to 'esp32/fan1'
              },
              child: Text('Turn Off'),
            ),
            ElevatedButton(
              onPressed: () {
                _toggleDevice(
                    'fan1', 'AUTO'); // Publishes 'AUTO' message to 'esp32/fan1'
              },
              child: Text('Auto Mode'),
            ),
            SizedBox(height: 20),

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
                _toggleDevice(
                    'fan2', 'ON'); // Publishes 'ON' message to 'esp32/'
              },
              child: Text('Turn On'),
            ),
            ElevatedButton(
              onPressed: () {
                _toggleDevice('fan2',
                    'OFF'); // Publishes 'OFF' message to 'esp32/airPump'
              },
              child: Text('Turn Off'),
            ),
            ElevatedButton(
              onPressed: () {
                _toggleDevice('fan2',
                    'AUTO'); // Publishes 'AUTO' message to 'esp32/airPump'
              },
              child: Text('Auto Mode'),
            ),
          ],
        ),
      ),
    );
  }
}
