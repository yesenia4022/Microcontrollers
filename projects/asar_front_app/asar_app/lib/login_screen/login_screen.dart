import 'package:flutter/material.dart';
import 'other/my_text.dart';
import 'other/sign_button.dart';
import 'other/my_square.dart';

class LoginScreen extends StatelessWidget {
  const LoginScreen({super.key});

  void signInLogic() {}
  @override
  Widget build(BuildContext context) {
    final emailController = TextEditingController();
    final passwordController = TextEditingController();

    return Scaffold(
      backgroundColor: Colors.grey[400],
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
                
                // Use MyTextFormField for email
                // Obscure allows the password to be hidden (privacy)
                MyTextFormField(
                  controller: emailController,
                  hintText: 'Email',
                  obscureText: false,
                ),
          
                // Use MyTextFormField for password
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
                    // Implement what happens when the button is pressed
                    // For example, validate form and sign in the user
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
                  imagePath: 'lib/images/google.png', // Specify the image path here
                  onPressed: signInLogic,
                ),
              ],
            ),
          ),
        ),
      ),
    );
  }
}