import 'package:flutter/material.dart';
import 'package:syncfusion_flutter_charts/charts.dart';
import 'MQTT.dart';
import 'FanControlScreen.dart';

class SensorScreen extends StatefulWidget {
  final MQTTClientWrapper mqttClientWrapper;

  SensorScreen({required this.mqttClientWrapper});

  @override
  _SensorScreenState createState() => _SensorScreenState();
}

class _SensorScreenState extends State<SensorScreen> {
  // a map to store data from all sensors
  Map<String, List<_SensorData>> sensorDataMap = {
    'IR Sensor': [],
    'Temperature Sensor': [],
    'Flame Sensor': [],
    'Gas Sensor': [],
    'Smoke Sensor': [],
    'Humidity Sensor': [],
  };

  // a map to store icons for all sensors
  final Map<String, IconData> sensorIcons = {
    'IR Sensor': Icons.wifi_tethering,
    'Temperature Sensor': Icons.thermostat,
    'Flame Sensor': Icons.local_fire_department,
    'Gas Sensor': Icons.cloud,
    'Smoke Sensor': Icons.smoke_free,
    'Humidity Sensor': Icons.water_drop,
  };

  // a map to store colors for all sensors
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

    // set a callback for when an MQTT message is received
    widget.mqttClientWrapper.onMessageReceived = (message) {
      setState(() {
        var sensorType = _getSensorTypeFromMessage(message);
        if (sensorType != null) {
          // add the message to the data list for the specified sensor
          var sensorList = sensorDataMap[sensorType]!;
          sensorList.add(_SensorData(DateTime.now(), double.parse(message)));
          // remove the oldest item if the list is too long
          if (sensorList.length > 20) {
            sensorList.removeAt(0);
          }
        }
      });
    };

    // prepare the MQTT client
    widget.mqttClientWrapper.prepareMqttClient();
  }

  // a helper function to determine which sensor a message belongs to
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

  /// This function builds a screen with a chart for each sensor reading
  /// The chart shows the last 20 readings for each sensor.
  /// The chart is updated in real time when a new MQTT message is received.
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
        // Add some padding to the entire screen
        padding: const EdgeInsets.all(16.0),
        child: Column(
          children: sensorDataMap.entries.map((entry) {
            return Expanded(
              child: Column(
                children: [
                  // A row for the sensor icon and the sensor name
                  Row(
                    children: [
                      // The icon for the sensor
                      Icon(sensorIcons[entry.key], size: 24),
                      SizedBox(width: 8),
                      // The name of the sensor
                      Text(
                        '${entry.key} Reading',
                        style: TextStyle(fontSize: 18),
                      ),
                    ],
                  ),
                  SizedBox(height: 10),
                  // A chart for the sensor readings
                  Expanded(
                    child: SfCartesianChart(
                      primaryXAxis: DateTimeAxis(),
                      series: <CartesianSeries>[
                        // A line series for the sensor readings
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
