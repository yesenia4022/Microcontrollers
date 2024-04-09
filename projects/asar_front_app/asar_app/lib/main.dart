import 'package:asar_app/screens/device_screen.dart';
import 'package:asar_app/screens/login_screen.dart';
import 'package:asar_app/screens/central_screen.dart';
import 'package:flutter/material.dart';
import 'package:firebase_core/firebase_core.dart';
import 'package:firebase_auth/firebase_auth.dart';
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

class MyApp extends StatefulWidget {
  const MyApp({super.key});
  @override
  MyAppState createState() => MyAppState();
}

class MyAppState extends State<MyApp> {
  @override
  void initState() {
    super.initState();
    
    // Listen to the authentication state changes
    FirebaseAuth.instance.authStateChanges().listen((User? user) {
      if (user == null) {
        print("User is currently signed out!");
        navigatorKey.currentState?.pushReplacementNamed('/');
      } else {
        print("User is signed in!");
        navigatorKey.currentState?.pushReplacementNamed('/devices');
      }
    });
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'ASAR App',
      navigatorKey: navigatorKey, // Assign the navigator key
      initialRoute: '/',
      routes: {
        '/': (context) => const LoginScreen(),
        '/devices': (context) => DeviceScreen(),
      },
      onGenerateRoute: (RouteSettings settings){
        if(settings.name == '/central'){
          final argu = settings.arguments as CentralScreenArguments;
          return MaterialPageRoute(builder: ((context) => CentralScreen(deviceName: argu.deviceName))); 
        }
      },
    );
  }
}
