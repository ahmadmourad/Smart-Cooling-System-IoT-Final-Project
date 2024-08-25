import 'package:flutter/material.dart';
import 'MQTT.dart';

class FanControlScreen extends StatelessWidget {
  final MQTTClientWrapper mqttClientWrapper;

  FanControlScreen({required this.mqttClientWrapper});

  void _toggleFan(String fan, bool turnOn) {
    mqttClientWrapper.publishMessage('esp32/$fan', turnOn ? 'ON' : 'OFF');
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Fan Control'),
      ),
      body: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            Text(
              'Control Fan 1 and Fan 2',
              style: TextStyle(fontSize: 24),
            ),
            SizedBox(height: 20),
            ElevatedButton(
              onPressed: () => _toggleFan('fan1', true),
              child: Text('Turn Fan 1 ON'),
            ),
            ElevatedButton(
              onPressed: () => _toggleFan('fan1', false),
              child: Text('Turn Fan 1 OFF'),
            ),
            SizedBox(height: 20),
            ElevatedButton(
              onPressed: () => _toggleFan('fan2', true),
              child: Text('Turn Fan 2 ON'),
            ),
            ElevatedButton(
              onPressed: () => _toggleFan('fan2', false),
              child: Text('Turn Fan 2 OFF'),
            ),
          ],
        ),
      ),
    );
  }
}
