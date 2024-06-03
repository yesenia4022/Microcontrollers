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
          style: const TextStyle(
              fontWeight: FontWeight.bold,
              fontSize: 20,
            ),
        ),
        centerTitle: true,
        automaticallyImplyLeading: false,
      ),

      // The bottom bar navigation
      floatingActionButton: FloatingActionButton(
        onPressed: () {
          Navigator.pushNamed(context, '/temperature');
        },
        child: const Icon(AntDesign.areachart),
      ),
      floatingActionButtonLocation: FloatingActionButtonLocation.centerDocked,
      // The bottom bar navigation
      bottomNavigationBar: BottomAppBar(
        shape: const CircularNotchedRectangle(),
        notchMargin: 6.0,
        child: Row(
          mainAxisAlignment: MainAxisAlignment.spaceBetween,
          children: <Widget>[
            IconButton(
              icon: const Icon(Ionicons.arrow_back_circle_sharp),
              onPressed: () {
                Navigator.pop(context);
              },
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
