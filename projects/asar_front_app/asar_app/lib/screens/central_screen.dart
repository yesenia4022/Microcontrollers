import 'package:flutter/material.dart';

class CentralScreen extends StatefulWidget {
  final String deviceName;

  // Constructor to receive the device name or any other necessary data
  const CentralScreen({super.key, required this.deviceName});

  @override
  CentralScreenState createState() => CentralScreenState();
}

class CentralScreenState extends State<CentralScreen> {
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        backgroundColor: Colors.lightBlue[50],
        title: Text(
          'Status: ${widget.deviceName}',
        ),
      ),
    );
  }
}
