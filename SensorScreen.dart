import 'package:flutter/material.dart';
import 'package:syncfusion_flutter_charts/charts.dart';
import 'MQTT.dart';

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
      ),
      body: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          children: sensorDataMap.entries.map((entry) {
            return Expanded(
              child: Column(
                children: [
                  Text(
                    '${entry.key} Reading',
                    style: TextStyle(fontSize: 18),
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
