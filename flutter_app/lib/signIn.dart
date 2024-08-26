import 'package:firebase_auth/firebase_auth.dart';
import 'package:flutter/material.dart';
import 'signUp.dart';
import 'sensorScreen.dart';
import 'MQTT.dart';

class SignInScreen extends StatefulWidget {
  @override
  _SignInScreenState createState() => _SignInScreenState();
}

class _SignInScreenState extends State<SignInScreen> {
  final TextEditingController _emailController = TextEditingController();
  final TextEditingController _passwordController = TextEditingController();
  final FirebaseAuth _auth = FirebaseAuth.instance;
  final MQTTClientWrapper mqttClientWrapper =
      MQTTClientWrapper(); // Initialize the MQTT client wrapper

  void _signIn() async {
    try {
      UserCredential userCredential = await _auth.signInWithEmailAndPassword(
        email: _emailController.text.trim(), // Trim the email (whitespaces)
        password: _passwordController.text,
      );

      print("User signed in successfully"); // User signed in

      // Navigate to the SensorScreen after successful sign-in
      Navigator.push(
        context,
        MaterialPageRoute(
          builder: (context) =>
              SensorScreen(mqttClientWrapper: mqttClientWrapper),
        ),
      );
    } on FirebaseAuthException catch (e) {
      // Handle errors in the sign-in process
      showDialog(
        context: context,
        builder: (BuildContext context) {
          return AlertDialog(
            title: Text('Error'),
            content: Text(
                'An error occurred: ${e.message}'), // Display error message
            actions: [
              TextButton(
                child: Text('OK'),
                onPressed: () {
                  Navigator.of(context).pop();
                },
              ),
            ],
          );
        },
      );
    }
  }

  @override
  // Builds the sign-in screen.

  // This screen allows users to sign in to their account using their email and password

  // If the user does not have an account, they can navigate to the sign-up screen
  // by clicking on the "Don't have an account? Sign Up" button.
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text("Sign In"),
      ),
      body: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          children: [
            // Email text field
            TextField(
              controller: _emailController,
              decoration: InputDecoration(labelText: "Email"),
            ),
            // Password text field
            TextField(
              controller: _passwordController,
              decoration: InputDecoration(labelText: "Password"),
              obscureText: true,
            ),
            SizedBox(height: 20),
            // Sign-in button
            ElevatedButton(
              onPressed: _signIn,
              child: Text("Sign In"),
            ),
            // Sign-up button
            TextButton(
              onPressed: () {
                Navigator.push(
                  context,
                  MaterialPageRoute(builder: (context) => SignUpScreen()),
                );
              },
              child: Text("Don't have an account? Sign Up"),
            ),
          ],
        ),
      ),
    );
  }
}
