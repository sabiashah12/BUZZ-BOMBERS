# BUZZ-BOMBERS

**BUZZ-BOMBERS** is a game built using C++ and SFML (Simple and Fast Multimedia Library). This project allows you to compile and run the game on a Linux system, provided the necessary dependencies are installed.

## Requirements

Before compiling and running the game, you need to install the required dependencies. These steps should only be run once.

### 1. Install the GNU G++ Compiler

To compile the C++ code, you will need the **GNU G++ Compiler**. You can install it with the following command:

```bash
sudo apt-get install g++
2. Install SFML (Simple and Fast Multimedia Library)
SFML is a multimedia library used for graphics, audio, and window management. Install it by running:

bash
Copy code
sudo apt-get install libsfml-dev
Compilation Instructions
To compile the game, follow these steps:

Compile the source file (buzz.cpp):

bash
Copy code
g++ -c buzz.cpp
Link the object file and create the executable (sfml-app):

bash
Copy code
g++ buzz.o -o sfml-app -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system
Running The Game
Once the game has been compiled, you can run it with the following command:

bash
Copy code
./sfml-app
Enjoy playing BUZZ-BOMBERS!
