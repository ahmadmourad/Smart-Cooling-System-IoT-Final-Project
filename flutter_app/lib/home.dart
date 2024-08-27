import 'package:flutter/material.dart';

class HomeScreen extends StatelessWidget {
  @override

  /// The home screen of our app using a scaffold widget and gradient background
  /// Contains a logo, a tagline, and ElevatedButton widgets for signing up and signing in.
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Container(
        // Add padding to the elements of the screen
        padding: EdgeInsets.symmetric(horizontal: 30),
        // Gradient background
        decoration: BoxDecoration(
          gradient: LinearGradient(
            colors: [Colors.blue.shade100, Colors.blue.shade300],
            begin: Alignment.topLeft,
            end: Alignment.bottomRight,
          ),
        ),
        // Vertical column with children
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          crossAxisAlignment: CrossAxisAlignment.center,
          children: [
            /// Used image asset for the logo
            Image.asset(
              'assets/smart_ac_logo.png',
              height: 120,
            ),
            SizedBox(height: 30),

            /// Tagline text
            Text(
              'Smart Climate Control: A Simple Solution for AC Automation',
              textAlign: TextAlign.center,
              style: TextStyle(
                fontSize: 22,
                fontWeight: FontWeight.bold,
                color: Colors.black87,
              ),
            ),
            SizedBox(height: 40),
            // Buttons
            /// Sign up button
            ElevatedButton(
              onPressed: () {
                Navigator.pushNamed(
                    context, '/sign-up'); // Navigate to sign up screen
              },
              style: ElevatedButton.styleFrom(
                backgroundColor: Colors.blueAccent, // Set background color
                foregroundColor: Colors.white, // Set text color
                padding: EdgeInsets.symmetric(horizontal: 50, vertical: 15),
                shape: RoundedRectangleBorder(
                  borderRadius: BorderRadius.circular(30),
                ),
              ),
              child: Text('Sign Up', style: TextStyle(fontSize: 18)),
            ),
            SizedBox(height: 20),

            /// Sign in button
            ElevatedButton(
              onPressed: () {
                Navigator.pushNamed(
                    context, '/sign-in'); // Navigate to sign in screen
              },
              style: ElevatedButton.styleFrom(
                backgroundColor: Colors.blueAccent,
                foregroundColor: Colors.white,
                padding: EdgeInsets.symmetric(horizontal: 50, vertical: 15),
                shape: RoundedRectangleBorder(
                  borderRadius: BorderRadius.circular(30),
                ),
              ),
              child: Text('Sign In', style: TextStyle(fontSize: 18)),
            ),
          ],
        ),
      ),
    );
  }
}
