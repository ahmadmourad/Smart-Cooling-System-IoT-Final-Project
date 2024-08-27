import 'package:firebase_core/firebase_core.dart';
import 'package:flutter/material.dart';
import 'SensorScreen.dart';
import 'FanControlScreen.dart';
import 'MQTT.dart';
import 'signIn.dart';
import 'signUp.dart';
import 'home.dart';

// Global MQTTClientWrapper instance for general use
MQTTClientWrapper mqttClientWrapper = MQTTClientWrapper();

void main() async {
  WidgetsFlutterBinding.ensureInitialized();
  await Firebase.initializeApp();
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
      debugShowCheckedModeBanner: false, // Remove debug banner
      initialRoute: '/', // Set initial route to launching the app
      routes: {
        '/': (context) => HomeScreen(), // Set routes for each screen
        '/sign-in': (context) => SignInScreen(),
        '/sign-up': (context) => SignUpScreen(),
        '/sensor': (context) =>
            SensorScreen(mqttClientWrapper: mqttClientWrapper),
        '/fan-control': (context) =>
            FanControlScreen(mqttClientWrapper: mqttClientWrapper),
      },
    );
  }
}
