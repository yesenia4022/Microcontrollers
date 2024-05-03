import 'package:fl_chart/fl_chart.dart';
import 'package:flutter/material.dart';

class HeatMapGraph extends StatelessWidget {
    final List<double> temperatures;

    HeatMapGraph({required this.temperatures});

    @override
    Widget build(BuildContext context){
      List<FlSpot> position = temperatures
        .asMap()
        .entries
        .map((e) => FlSpot(e.key.toDouble(), e.value))
        .toList();

        // Continue Logic
        return  Padding(
          padding: const EdgeInsets.all(8.0),
          child: LineChart(
            LineChartData(
              gridData: FlGridData(show: true),
              titlesData: FlTitlesData(
                bottomTitles: AxisTitles(
                  sideTitles: SideTitles(showTitles: true, reservedSize: 22, getTitlesWidget: bottomTitleWidgets)
                ),
                leftTitles: AxisTitles(
                  sideTitles: SideTitles(showTitles: true, reservedSize: 40, getTitlesWidget: leftTitleWidgets),
                ),
              ),
              borderData: FlBorderData(show: true),
              lineBarsData: [
              LineChartBarData(
                spots: position,
                isCurved: true,
                color: Theme.of(context).primaryColor,
                barWidth: 5,
                isStrokeCapRound: true,
                dotData: FlDotData(show: false),
                belowBarData: BarAreaData(show: true, color: Theme.of(context).primaryColor.withOpacity(0.3),),
            )
          ],
            ),
          ),
        );
    }

    Widget bottomTitleWidgets(double value, TitleMeta meta) {
    const style = TextStyle(
      color: Color(0xff68737d),
      fontWeight: FontWeight.bold,
      fontSize: 16,
    );
    Widget text = Text(value.toInt().toString(), style: style);
    return SideTitleWidget(
      axisSide: meta.axisSide,
      space: 16,
      child: text,
    );
  }

  Widget leftTitleWidgets(double value, TitleMeta meta) {
    const style = TextStyle(
      color: Color(0xff67727d),
      fontWeight: FontWeight.bold,
      fontSize: 15,
    );
    String text = '${value.toInt()}°';
    return SideTitleWidget(
      axisSide: meta.axisSide,
      space: 16,
      child: Text(text, style: style),
    );
  }
}
