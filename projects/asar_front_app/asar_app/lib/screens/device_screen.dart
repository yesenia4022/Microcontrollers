import 'package:flutter/material.dart';
import 'package:firebase_auth/firebase_auth.dart';

final GlobalKey<NavigatorState> navigatorKey = GlobalKey<NavigatorState>();

class DeviceScreen extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Center(child: Text('Your Devices')),
        actions: <Widget>[
          IconButton(
            icon: Icon(Icons.exit_to_app),
            tooltip: 'Log Out',
            onPressed: () async {
              // Logic that causes the user to sign out
              await FirebaseAuth.instance.signOut();

              // Log user out (login page :3)
              navigatorKey.currentState?.pushReplacementNamed('/');
            },
          ),
        ],
        automaticallyImplyLeading: false,
      ),
    );
  }
}