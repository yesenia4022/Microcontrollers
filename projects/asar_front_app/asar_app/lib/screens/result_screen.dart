import 'package:flutter/material.dart';
import 'package:firebase_database/firebase_database.dart';

class ResultScreen extends StatefulWidget {
  // Constructor to receive the device name or any other necessary data
  const ResultScreen({Key? key}) : super(key: key);

  @override
  ResultScreenState createState() => ResultScreenState();
}

class ResultScreenState extends State<ResultScreen> {
  late DatabaseReference _databaseReference;
  late Stream<DatabaseEvent> _dataStream;

  @override
  void initState() {
    super.initState();
    _databaseReference = FirebaseDatabase.instance.reference().child('sensors');
    _dataStream = _databaseReference.onValue;
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        backgroundColor: Colors.lightBlue[50],
        title: const Text(
          'Graph Output',
        ),
      ),

      body: StreamBuilder(
        stream: _dataStream,
        builder: (context, AsyncSnapshot<DatabaseEvent> snapshot) {
          if (snapshot.connectionState == ConnectionState.waiting) {
            return Center(child: CircularProgressIndicator());
          } else if (snapshot.hasError) {
            return Center(child: Text('Error: ${snapshot.error}'));
          } else {
            // Process the data from the snapshot and display it in your UI
            final data = snapshot.data?.snapshot.value;
            // For example, you can display the data in a Text widget
            return Center(child: Text('Received data: $data'));
          }
        },
      ),

    );
  }
}
