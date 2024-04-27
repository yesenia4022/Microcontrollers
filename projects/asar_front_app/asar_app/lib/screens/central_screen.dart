import 'package:flutter/material.dart';
import 'package:new_flutter_icons/new_flutter_icons.dart';

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

      // The bottom bar navigation
      bottomNavigationBar: BottomAppBar(
        shape: const CircularNotchedRectangle(),
        child: Row(
          mainAxisAlignment: MainAxisAlignment.spaceAround,
          children: <Widget>[
            IconButton(
              icon: const Icon(Ionicons.cafe),
              onPressed: () {},
            ),

            IconButton(
              icon: const Icon(AntDesign.areachart),
              onPressed: () {},
            ),

            IconButton(
              icon: const Icon(AntDesign.camera),
              onPressed: () {},
            ),
          ],
        ),
      ),
    );
  }
}
