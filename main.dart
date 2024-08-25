import 'package:flutter/material.dart';
import 'SensorScreen.dart';
import 'FanControlScreen.dart';
import 'MQTT.dart';

// Global MQTTClientWrapper instance
MQTTClientWrapper mqttClientWrapper = MQTTClientWrapper();

void main() {
  runApp(MyApp());
}

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Sensor and Fan Control App',
      theme: ThemeData(
        primarySwatch: Colors.blue,
      ),
      initialRoute: '/sensor',
      routes: {
        '/sensor': (context) =>
            SensorScreen(mqttClientWrapper: mqttClientWrapper),
        '/fan-control': (context) =>
            FanControlScreen(mqttClientWrapper: mqttClientWrapper),
      },
    );
  }
}
