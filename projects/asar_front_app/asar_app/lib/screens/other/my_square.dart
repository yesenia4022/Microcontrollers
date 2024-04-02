import 'package:flutter/material.dart';

class MySquare extends StatelessWidget {
  final String imagePath;
  final VoidCallback onPressed;

  const MySquare({
    super.key,
    required this.imagePath,
    required this.onPressed,
  });

  @override
  Widget build(BuildContext context) {
    return ElevatedButton.icon(
      icon: Image.asset(imagePath, height: 24), // Use the imagePath parameter
      label: const Text('Sign in with Google'),
      onPressed: onPressed,
      style: ElevatedButton.styleFrom(
        backgroundColor: Colors.white, // Button color
        foregroundColor: Colors.black, // Text color
        textStyle: const TextStyle(
          fontSize: 16,
        ),
        padding: const EdgeInsets.symmetric(horizontal: 12, vertical: 8),
        shape: RoundedRectangleBorder(
          borderRadius: BorderRadius.circular(8),
          side: const BorderSide(color: Colors.grey), // Border color
        ),
      ),
    );
  }
}