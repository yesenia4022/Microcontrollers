import 'package:flutter/material.dart';

class TemperatureHeatmap extends StatelessWidget {
  final List<double> temperatures;

  const TemperatureHeatmap({Key? key, required this.temperatures}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    int width = 32; // Width of the grid
      return LayoutBuilder(
      builder: (context, constraints) {
        return GridView.builder(
          physics: NeverScrollableScrollPhysics(),
          gridDelegate: SliverGridDelegateWithFixedCrossAxisCount(
            crossAxisCount: width,
            childAspectRatio: constraints.maxWidth / constraints.maxHeight,
          ),
          itemCount: temperatures.length,
          itemBuilder: (context, index) {
            return Container(
              color: getColorForTemperature(temperatures[index]),
              alignment: Alignment.center,
            );
          },
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
