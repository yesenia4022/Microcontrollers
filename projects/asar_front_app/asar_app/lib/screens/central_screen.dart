import 'package:flutter/material.dart';

class CentralScreen extends StatefulWidget {
  final String deviceName;

  // Constructor to receive the device name or any other necessary data
  CentralScreen({Key? key, required this.deviceName}) : super(key: key);

  @override
  _CentralScreenState createState() => _CentralScreenState();
}

class _CentralScreenState extends State<CentralScreen> {
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        backgroundColor: Colors.lightBlue[50],
        title: Text(
          'Device Status for ${widget.deviceName}',
        ),
      ),
    );
  }
}
