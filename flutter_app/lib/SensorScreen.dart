import 'package:flutter/material.dart';
import 'package:syncfusion_flutter_charts/charts.dart';
import 'MQTT.dart';
import 'FanControlScreen.dart'; // Import the FanControlScreen

class SensorScreen extends StatefulWidget {
  final MQTTClientWrapper mqttClientWrapper;

  SensorScreen({required this.mqttClientWrapper});

  @override
  _SensorScreenState createState() => _SensorScreenState();
}

class _SensorScreenState extends State<SensorScreen> {
  Map<String, List<_SensorData>> sensorDataMap = {
    'IR Sensor': [],
    'Temperature Sensor': [],
    'Flame Sensor': [],
    'Gas Sensor': [],
    'Smoke Sensor': [],
    'Humidity Sensor': [],
  };

  final Map<String, IconData> sensorIcons = {
    'IR Sensor': Icons.wifi_tethering,
    'Temperature Sensor': Icons.thermostat,
    'Flame Sensor': Icons.local_fire_department,
    'Gas Sensor': Icons.cloud,
    'Smoke Sensor': Icons.smoke_free,
    'Humidity Sensor': Icons.water_drop,
  };

  final Map<String, Color> sensorColors = {
    'IR Sensor': Colors.red,
    'Temperature Sensor': Colors.orange,
    'Flame Sensor': Colors.yellow,
    'Gas Sensor': Colors.green,
    'Smoke Sensor': Colors.blue,
    'Humidity Sensor': Colors.purple,
  };

  @override
  void initState() {
    super.initState();
    widget.mqttClientWrapper.onMessageReceived = (message) {
      setState(() {
        var sensorType = _getSensorTypeFromMessage(message);
        if (sensorType != null) {
          var sensorList = sensorDataMap[sensorType]!;
          sensorList.add(_SensorData(DateTime.now(), double.parse(message)));
          if (sensorList.length > 20) {
            sensorList.removeAt(0);
          }
        }
      });
    };
    widget.mqttClientWrapper.prepareMqttClient();
  }

  String? _getSensorTypeFromMessage(String message) {
    if (message.contains('irSensor')) return 'IR Sensor';
    if (message.contains('temperatureSensor')) return 'Temperature Sensor';
    if (message.contains('flameSensor')) return 'Flame Sensor';
    if (message.contains('gasSensor')) return 'Gas Sensor';
    if (message.contains('smokeSensor')) return 'Smoke Sensor';
    if (message.contains('humiditySensor')) return 'Humidity Sensor';
    return null;
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Sensor Readings'),
        backgroundColor: Colors.blueGrey,
        actions: [
          IconButton(
            icon: Icon(Icons.settings),
            onPressed: () {
              Navigator.push(
                context,
                MaterialPageRoute(
                    builder: (context) => FanControlScreen(
                        mqttClientWrapper: widget.mqttClientWrapper)),
              );
            },
          ),
        ],
      ),
      body: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          children: sensorDataMap.entries.map((entry) {
            return Expanded(
              child: Column(
                children: [
                  Row(
                    children: [
                      Icon(sensorIcons[entry.key], size: 24),
                      SizedBox(width: 8),
                      Text(
                        '${entry.key} Reading',
                        style: TextStyle(fontSize: 18),
                      ),
                    ],
                  ),
                  SizedBox(height: 10),
                  Expanded(
                    child: SfCartesianChart(
                      primaryXAxis: DateTimeAxis(),
                      series: <CartesianSeries>[
                        LineSeries<_SensorData, DateTime>(
                          dataSource: entry.value,
                          xValueMapper: (_SensorData data, _) => data.time,
                          yValueMapper: (_SensorData data, _) => data.reading,
                          color: sensorColors[entry.key],
                        ),
                      ],
                    ),
                  ),
                ],
              ),
            );
          }).toList(),
        ),
      ),
    );
  }
}

class _SensorData {
  _SensorData(this.time, this.reading);
  final DateTime time;
  final double reading;
}
