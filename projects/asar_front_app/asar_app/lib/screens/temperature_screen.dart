import 'package:asar_app/screens/heatmap_screen.dart';
import 'package:flutter/material.dart';
import 'package:firebase_database/firebase_database.dart';
import 'temperature_screen.dart';

class TemperatureScreen extends StatefulWidget {
  // Constructor to receive the device name or any other necessary data
  const TemperatureScreen({Key? key}) : super(key: key);

  @override
  TemperatureScreenState createState() => TemperatureScreenState();
}

class TemperatureScreenState extends State<TemperatureScreen> {
  late DatabaseReference _databaseReference;
  List<double> _heatMapArray = [];

  @override
  void initState() {
    super.initState();
    _databaseReference = FirebaseDatabase.instance.ref().child('temperatures');
    fetchGET();
  }

  void fetchGET() async {
      DataSnapshot pixelData = await _databaseReference.get();
  
      // Checks if the data within the database exists (reference)
      if(pixelData.exists) {
         List<double> ArrayTemp = [];

        // Update the specific child keys to the specified values.
        // By use of Map
        Map<dynamic, dynamic> chunkedData = pixelData.value as Map<dynamic, dynamic>;
        List<dynamic> orderedKeys = chunkedData.keys.toList()..sort();

        for (var key in orderedKeys) {
          List<dynamic> chunk = List.from(chunkedData[key]);
          ArrayTemp.addAll(chunk.map((value) => double.parse(value.toString())));
        }
        setState(() {
          _heatMapArray = ArrayTemp;
        });
        print(ArrayTemp);
      }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        backgroundColor: Colors.lightBlue[50],
        title: const Text(
          'Temperature Data Output',
        ),
      ),
      body: _heatMapArray.isEmpty
          ? const Center(child: CircularProgressIndicator())
          : HeatMapGraph(temperatures: _heatMapArray),
    );
  }
}
