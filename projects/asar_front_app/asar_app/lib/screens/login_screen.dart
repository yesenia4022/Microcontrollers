import 'package:flutter/material.dart';
import 'package:firebase_auth/firebase_auth.dart';
import 'other/my_text.dart';
import 'other/sign_button.dart';
import 'other/my_square.dart';
//import 'package:google_sign_in/google_sign_in.dart';

class LoginScreen extends StatefulWidget {
  const LoginScreen({super.key});

  @override
  LoginScreenState createState() => LoginScreenState();
}

class LoginScreenState extends State<LoginScreen> {
  final TextEditingController emailController = TextEditingController();
  final TextEditingController passwordController = TextEditingController();

  @override
  void dispose() {
    emailController.dispose();
    passwordController.dispose();
    super.dispose();
  }

  @override
    Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: Colors.lightBlue[50],
      body: SafeArea(
        child: SingleChildScrollView(
          child: Center(
            // Start the column
            child: Column(
              mainAxisSize: MainAxisSize.min,
              // Handles the icon of our application
              children: [
                Image.asset(
                  'lib/images/body_back.png',
                  width: 200,
                  height: 200,
                ),
                const SizedBox(height: 50),
                // Greeting the user
                const Text(
                  'Welcome back!',
                  style: TextStyle(
                    fontSize: 16,
                    color: Color.fromARGB(255, 0, 0, 0),
                  ),
                ),
                // 25 Pixels before text
                const SizedBox(height: 25),
                MyTextFormField(
                  controller: emailController,
                  hintText: 'Email',
                  obscureText: false,
                ),
                const SizedBox(height: 10),
                MyTextFormField(
                  controller: passwordController,
                  hintText: 'Password',
                  obscureText: true,
                ),  
                const SizedBox(height: 25),
                const Text("Forgot password?"),
                const SizedBox(height: 25),
                SignButton(
                  text: 'Sign In',
                  onPressed: signInLogic,
                ),
                const SizedBox(height: 25),
                const Padding(
                  padding: EdgeInsets.symmetric(horizontal: 25.0),
                  child: Row(
                    children: [
                      Expanded(
                        child: Divider(
                          thickness: 0.5,
                          color: Color.fromARGB(255, 0, 0, 0),
                        ),
                      ),
                      Padding(
                        padding: EdgeInsets.symmetric(horizontal: 25.0),
                        child: Text("or continue with"),
                      ),
                      Expanded(
                        child: Divider(
                          thickness: 0.5,
                          color: Color.fromARGB(255, 0, 0, 0),
                        ),
                      ),
                    ],
                  ),
                ),
                const SizedBox(height: 25),
                MySquare(
                  imagePath: 'lib/images/google.png',
                  onPressed: () {},
                ),
              ],
            ),
          ),
        ),
      ),
    );
  }

  // Create our function for onPressed: to connect the fire-base operation
Future<void> signInLogic() async {
  final auth = FirebaseAuth.instance;
  final String email = emailController.text;
  final String password = passwordController.text;

  if (email.isEmpty || password.isEmpty) {
    // Ideally, show an error message to the user, indicating email/pass must contain something
    ScaffoldMessenger.of(context).showSnackBar(
      const SnackBar(
        content: Text('Email and password cannot be empty'),
        backgroundColor: Colors.red,
      ),
    );
    return;
  }

    //auth.signInWithEmailAndPassword(email: email, password: password);
  try {
    await auth.signInWithEmailAndPassword(email: email, password: password);
    // Sign-in successful, navigate to the next page or perform other actions
  } catch (e) {
    // Handle sign-in errors, such as invalid credentials or network issues
    print('Error signing in: $e');
    // Optionally, display an error message to the user
  }
}

/*
  Future<void> signInLogicGoogle() async {
    // code
    final GoogleSignInAccount? googleUser = await GoogleSignIn().signIn();
    if(googleUser != null) {
      final GoogleSignInAuthentication googleAuth = await googleUser.authentication;
      final OAuthCredential credential = GoogleAuthProvider.credential(
          accessToken: googleAuth.accessToken,
          idToken: googleAuth.idToken,
        );
      // Sign in to Firebase with the Google user credentials
      await FirebaseAuth.instance.signInWithCredential(credential);
      // Successful authentication actions (e.g., navigate to home screen)
    }
  }
  */
}