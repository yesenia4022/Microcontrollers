import 'package:flutter/material.dart';
import 'package:firebase_auth/firebase_auth.dart';
import 'package:shared_preferences/shared_preferences.dart';


class DeviceScreen extends StatefulWidget {
  const DeviceScreen({super.key});
  @override
  DeviceScreenState createState() => DeviceScreenState();
}

class DeviceScreenState extends State<DeviceScreen> {
  // This list holds the names of the devices
  final List<String> _devices = [];
  @override
  void initState() {
    super.initState();
    _loadDevices();
  }

  Future<void> _loadDevices() async {
    // code
    final prefs = await SharedPreferences.getInstance();
    final devices = prefs.getStringList('devices') ?? [];
    setState(() {
      _devices.addAll(devices);
    });
  }

  Future<void> _saveDevices(List<String> devices) async {
    // code
    final prefs = await SharedPreferences.getInstance();
    await prefs.setStringList('devices', devices);
  }

  void _addDevice() {
    setState(() {
      _devices.add('Device ${_devices.length + 1}');
      _saveDevices(_devices);
    });
  }

  void _renameDevice(int index){
    // code
    String new_name = _devices[index];

    showDialog(
      context: context, 
      builder: (BuildContext contex){
        // code
        return  AlertDialog(
          title: const Text(
            'Rename Device',
            style:TextStyle(
              fontWeight: FontWeight.bold,
              fontSize: 20,
            ),
            ),
            content: TextField(
              onChanged: (value){
                new_name = value;
              },
              decoration: const InputDecoration(
                hintText: 'Enter new name'
                ),
            ),
            actions: <Widget>[
              // Cancel option
              TextButton(
                child: const Text('Cancel'),
                 onPressed: (){
                  print('Rename cancelled');
                  Navigator.of(context).pop();
                 },
              ),

              // Proceed with remove
              TextButton(
                child: const Text('Rename'), 
                onPressed: (){
                  // Rename!
                  setState(() {
                    _devices[index] = new_name; // Update the device name in the list
                  });
                  _saveDevices(_devices); // Save the updated list to storage
                  Navigator.of(context).pop();
                },
              ),
            ],
        );
       },
      );
  }

  void _removeDevice(int index) {
    showDialog(
    context: context,
    builder: (BuildContext context) {
      return AlertDialog(
        title: const Text(
          'Remove Device',
          style: TextStyle(
            fontWeight: FontWeight.bold,
            fontSize: 20,
          ),
        ),
        content: Text(
          'Are you sure you want to remove ${_devices[index]}?',
        ),
        actions: <Widget>[
          TextButton(
            child: const Text('Cancel'),
            onPressed: () {
              print('Remove cancelled');
              Navigator.of(context).pop(); // Close the dialog
            },
          ),
          TextButton(
            child: const Text('Remove'),
            onPressed: () {
              setState(() {
                _devices.removeAt(index); // Remove the device from the list
              });
              _saveDevices(_devices); // Save the updated list to storage
              Navigator.of(context).pop(); // Close the dialog
            },
          ),
        ],
      );
    },
  );
}
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        backgroundColor: Colors.lightBlue[50],
        title: const Center(
          child: Text(
            'Your Devices', 
            style: TextStyle(
              fontWeight: FontWeight.bold,
              fontSize: 20,
            ),
        )),
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
                Navigator.pushNamed(
                  context,
                  '/central',
                  arguments: CentralScreenArguments(deviceName: _devices[index]),
                );
              },
            
              child: Padding(
                padding: EdgeInsets.all(20),
                child: Row(
                mainAxisAlignment: MainAxisAlignment.spaceBetween,
                children: [
                      Text(
                        _devices[index],
                        style: TextStyle(fontSize: 18),
                      ),
                      DropdownButton<String>(
                        value: 'Options',
                        items: <String>['Options', 'Rename', 'Remove']
                        .map<DropdownMenuItem<String>>((String value) {
                          return DropdownMenuItem<String>(
                            value: value,
                             child: Text(
                              value,
                              style: value == 'Options' ? TextStyle(fontWeight: FontWeight.bold) : null,
                            ),
                          );
                        }).toList(),
                        onChanged: (String? newValue) {
                        if (newValue == 'Rename') {
                            _renameDevice(index);
                        } else if (newValue == 'Remove') {
                            _removeDevice(index);
                        }
                        },
                      ),
                  ],
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

class CentralScreenArguments {
  final String deviceName;

  CentralScreenArguments({required this.deviceName});
}