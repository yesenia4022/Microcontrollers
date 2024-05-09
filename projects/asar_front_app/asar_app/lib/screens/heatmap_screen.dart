import 'package:flutter/material.dart';

class TemperatureHeatmap extends StatelessWidget {
  final List<double> temperatures;

  const TemperatureHeatmap({Key? key, required this.temperatures}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    int width = 32; // Width of the grid
    int height = 24; // Height of the grid

    return GridView.builder(
      gridDelegate: SliverGridDelegateWithFixedCrossAxisCount(
        crossAxisCount: width,
      ),
      itemCount: temperatures.length,
      itemBuilder: (context, index) {
        return Container(
          color: getColorForTemperature(temperatures[index]),
          alignment: Alignment.center,
        );
      },
    );
  }

  Color getColorForTemperature(double temperature) {
    // Adjust these thresholds and colors based on your specific data range
    if (temperature < 20) {
      return Colors.blue[300]!;
    } else if (temperature < 25) {
      return Colors.green[300]!;
    } else if (temperature < 30) {
      return Colors.yellow[700]!;
    } else {
      return Colors.red[800]!;
    }
  }
}
