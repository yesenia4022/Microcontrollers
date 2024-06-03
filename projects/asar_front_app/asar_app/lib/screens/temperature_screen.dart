import 'package:asar_app/screens/heatmap_screen.dart';
import 'package:flutter/material.dart';
import 'package:firebase_database/firebase_database.dart';

class TemperatureScreen extends StatefulWidget {
  // Constructor to receive the device name or any other necessary data
  const TemperatureScreen({Key? key}) : super(key: key);

  @override
  TemperatureScreenState createState() => TemperatureScreenState();
}

class TemperatureScreenState extends State<TemperatureScreen> {
  late DatabaseReference _databaseReference;
  List<double> _beforeTemperatures = [];
  List<double> _afterTemperatures = [];

  // Cleaning "again" should be initialized to false
  bool shouldASARClean = false; 

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
        // Update the specific child keys to the specified values.
        // By use of Map
        Map<dynamic, dynamic> chunkedData = pixelData.value as Map<dynamic, dynamic>;
        int numberOfSegments = chunkedData.keys.length;
        print('NUMBER OF SEGS $numberOfSegments');

        List<dynamic> orderedKeys = chunkedData.keys.toList()..sort();
        int segmentsToProcess = numberOfSegments > 12 ? numberOfSegments : 12;
        print('segmentsToProcess: $segmentsToProcess');
        List<double> latestTemperatures = [];
        // Start from the end to get the most recent segmentsq
        for (var i = orderedKeys.length - segmentsToProcess; i < orderedKeys.length; i++) {
            List<dynamic> chunk = List.from(chunkedData[orderedKeys[i]]);
            latestTemperatures.addAll(chunk.map((value) => double.parse(value.toString())));
        }

        if (latestTemperatures.length != 768 * 2) {
            print('Unexpected number of temperature values: ${latestTemperatures.length}');
        }

        List<double> beforeTempies = latestTemperatures.sublist(0, 768); //[];
        List<double> afterTempies = latestTemperatures.sublist(768); //[];

        print('BEFORE TEMP LENGTH');
        print(beforeTempies.length);
        print('AFTER TEMP LENGTH');
        print(afterTempies.length);

        // Logic that will do the calculations to determine if we need to clean or not...
        double averageBeforeTemp = beforeTempies.isNotEmpty
          ? beforeTempies.reduce((a, b) => a + b) / beforeTempies.length
          : 0.0;
        double averageAfterTemp = afterTempies.isNotEmpty
          ? afterTempies.reduce((a, b) => a + b) / afterTempies.length
          : 0.0;

        setState(() {
          _beforeTemperatures = beforeTempies;
          _afterTemperatures = afterTempies;
        });

        if ((averageBeforeTemp - averageAfterTemp) >= 3.0) {
            shouldASARClean = false; // Do not clean again
        } else {
            shouldASARClean = true; // Clean again
        }

        // POST the indicator to the database
        await postIndicator(shouldASARClean);

      }
  }

Future<void> postIndicator(bool indicateClean) async {
  DatabaseReference indicatorRef = FirebaseDatabase.instance.ref().child('clean_indicator');
  await indicatorRef.set({'CleanIndicator': indicateClean});
}

@override
Widget build(BuildContext context) {
    double averageBeforeTemp = _beforeTemperatures.isNotEmpty 
        ? _beforeTemperatures.reduce((a, b) => a + b) / _beforeTemperatures.length 
        : 0.0;
    double averageAfterTemp = _afterTemperatures.isNotEmpty 
        ? _afterTemperatures.reduce((a, b) => a + b) / _afterTemperatures.length 
        : 0.0;

  return Scaffold(
    appBar: AppBar(
      backgroundColor: Colors.lightBlue[50],
      title: const Text('Temperature Data Output'),
    ),
    body: ListView(
      padding: const EdgeInsets.all(0),
      children: [
        Align(
            alignment: Alignment.centerLeft,
            child: Padding(
              padding: const EdgeInsets.symmetric(vertical: 8.0, horizontal: 16.0),
              child: Card(
                shape: RoundedRectangleBorder(
                  borderRadius: BorderRadius.circular(15.0),
                ),
                elevation: 5,
                color: Colors.blue[50],
                child: Padding(
                  padding: const EdgeInsets.all(16.0),
                  child: Text(
                    "Before: ${averageBeforeTemp.toStringAsFixed(2)}°C",
                    style: const TextStyle(fontSize: 24, fontWeight: FontWeight.bold),
                  ),
                ),
              ),
            ),
          ),
        Container(
          height: (768 / 32) * 10.0,
          child: TemperatureHeatmap(temperatures: _beforeTemperatures),
        ),
        if (_afterTemperatures.isNotEmpty) ...[
         // SizedBox(height: 5), // Adds space between the graphs
        Align(
            alignment: Alignment.centerLeft,
            child: Padding(
            padding: const EdgeInsets.symmetric(vertical: 8.0, horizontal: 16.0),
            child: Card(
                shape: RoundedRectangleBorder(
                borderRadius: BorderRadius.circular(15.0),
                ),
               elevation: 5,
                color: Colors.blue[50],
                child: Padding(
                    padding: const EdgeInsets.all(16.0),
                    child: Text(
                      "After: ${averageAfterTemp.toStringAsFixed(2)}°C",
                      style: const TextStyle(fontSize: 24, fontWeight: FontWeight.bold),
                    ),
                  ),
                ),
              ),
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