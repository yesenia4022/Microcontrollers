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
 // List<double> _heatMapArray = [];
 List<double> _beforeTemperatures = [];
List<double> _afterTemperatures = [];

  @override
  void initState() {
    super.initState();
    _databaseReference = FirebaseDatabase.instance.ref().child('temperatures');
    fetchGET();
  }

  void fetchGET() async {
    print('BEFORE GET REQUEST');
    DataSnapshot pixelData = await _databaseReference.get();

  // DataSnapshot pixelData = await _databaseReference.get();
      // Checks if the data within the database exists (reference)
    if (pixelData.exists) {
        print('IT EXISTS');
       // List<double> ArrayTemp = [];

        // Update the specific child keys to the specified values.
        // By use of Map
        Map<dynamic, dynamic> chunkedData = pixelData.value as Map<dynamic, dynamic>;
        int numberOfSegments = chunkedData.keys.length;
        print('NUMBER OF SEGS $numberOfSegments');

        List<dynamic> orderedKeys = chunkedData.keys.toList()..sort();
        int segmentsToProcess = numberOfSegments > 12 ? 12 : numberOfSegments;
        List<double> latestTemperatures = [];
      // // Start from the end to get the most recent segments
        for (var i = orderedKeys.length - segmentsToProcess; i < orderedKeys.length; i++) {
          List<dynamic> chunk = List.from(chunkedData[orderedKeys[i]]);
          latestTemperatures.addAll(chunk.map((value) => double.parse(value.toString())));
       }

       if (latestTemperatures.length != 768 * 2) {
           print('Unexpected number of temperature values: ${latestTemperatures.length}');
           return;
       }

      List<double> beforeTempies = latestTemperatures.sublist(0, 768); //[];
      List<double> afterTempies = latestTemperatures.sublist(768); //[];
      // // Split the temperatures into before and after
      //beforeTempies = latestTemperatures.sublist(0, 6 * 32); // Each segment is 6 chunks of data
      print('BEFORE TEMP LENGTH');
      print(beforeTempies.length);
      //if (segmentsToProcess == 12) {
         //afterTempies = latestTemperatures.sublist(6 * 32);
         print('AFTER TEMP LENGTH');
         print(afterTempies.length);
      //}

        setState(() {
          _beforeTemperatures = beforeTempies;
          //if (afterTempies.isNotEmpty) {
             _afterTemperatures = afterTempies;
          //}
        });
      }
  }

@override
Widget build(BuildContext context) {
  return Scaffold(
    appBar: AppBar(
      backgroundColor: Colors.lightBlue[50],
      title: const Text('Temperature Data Output'),
    ),
    body: ListView(
      padding: EdgeInsets.all(0),
      children: [
        const Padding(
          padding: const EdgeInsets.symmetric(vertical: 8.0, horizontal: 16.0),
          child: Text("Before", style: TextStyle(fontSize: 24, fontWeight: FontWeight.bold)),
        ),
        Container(
          height: (768 / 32) * 10.0,
          child: TemperatureHeatmap(temperatures: _beforeTemperatures),
        ),
        if (_afterTemperatures.isNotEmpty) ...[
         // SizedBox(height: 5), // Adds space between the graphs
          const Padding(
            padding: const EdgeInsets.symmetric(vertical: 8.0, horizontal: 16.0),
            child: Text("After", style: TextStyle(fontSize: 24, fontWeight: FontWeight.bold)),
          ),
          Container(
          height: (768 / 32) * 10.0,
          child: TemperatureHeatmap(temperatures: _afterTemperatures),
        ),
        ],
      ],
    ),
  );
}
}