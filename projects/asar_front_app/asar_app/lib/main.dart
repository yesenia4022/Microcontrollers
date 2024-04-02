import 'package:asar_app/screens/device_screen.dart';
import 'package:asar_app/screens/login_screen.dart';
import 'package:flutter/material.dart';
import 'package:firebase_core/firebase_core.dart';
import 'dart:io';

final GlobalKey<NavigatorState> navigatorKey = GlobalKey<NavigatorState>();

void main() async {
  WidgetsFlutterBinding.ensureInitialized();
  Platform.isAndroid 
  ? await Firebase.initializeApp(
    options: const FirebaseOptions(
      apiKey: 'AIzaSyC7UEZXEEHa9vRFYwvGswoQnZCq9WLgShc',
      appId: '1:91191462627:android:adfd00a1bde73723ce53d6', 
      messagingSenderId: '91191462627', 
      projectId: 'asar-cd8e5'
      ))
      : await Firebase.initializeApp();
  runApp(MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'ASAR App',
      navigatorKey: navigatorKey, // Assign the navigator key
      initialRoute: '/',
      routes: {
        '/': (context) => LoginScreen(),
        '/devices': (context) => DeviceScreen(),
      },
    );
  }
}