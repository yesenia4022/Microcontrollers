import 'package:flutter/material.dart';
import 'package:firebase_auth/firebase_auth.dart';

class DeviceScreen extends StatefulWidget {
  @override
  _DeviceScreenState createState() => _DeviceScreenState();
}

class _DeviceScreenState extends State<DeviceScreen> {
  // This list holds the names of the devices
  final List<String> _devices = [];

  void _addDevice() {
    setState(() {
      _devices.add('Device ${_devices.length + 1}');
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Center(child: Text('Your Devices')),
        automaticallyImplyLeading: false,
      ),
      

      body: ListView.builder(
        itemCount: _devices.length,
        itemBuilder: (context, index) {
          return Card(
            margin: EdgeInsets.all(10),
            child: InkWell(
              onTap: () {
                // Handle the device tap here
                print('Tapped on ${_devices[index]}');
                // Navigate to device details or perform other actions
              },
              child: Padding(
                padding: EdgeInsets.all(20),
                child: Text(
                  _devices[index],
                  style: TextStyle(fontSize: 18),
                ),
              ),
            ),
          );
        },
      ),

      bottomNavigationBar: BottomAppBar(
        shape: CircularNotchedRectangle(),
        child: Row(
          mainAxisAlignment: MainAxisAlignment.spaceBetween,
          children: <Widget>[
            IconButton(
              icon: Icon(Icons.exit_to_app),
              onPressed: () async {
                // Logic that causes the user to sign out
                print("Logging out...");
                await FirebaseAuth.instance.signOut();
                // Log user out (login page :3)
                print("Logged out");
              },
            ),
            // This will push the logout button to the left and the add button to the right
            Spacer(),

            IconButton(
              onPressed: _addDevice, 
              icon: Icon(Icons.add),
              tooltip: 'Add Device',
            ), 
          ],
        ),
      ),
    );
  }
}