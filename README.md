# IMU ICM-20948 with Teensy 4.1
This repository contains code for interfacing the ICM-20948 Inertial Measurement Unit (IMU) with a Teensy 4.1 microcontroller. The code is written in C++ and is designed to read data from the IMU, including acceleration, gyroscope, and magnetometer readings.

Platformio is used instead of the Arduino IDE.

This project follows Paul McWhorter's tutorial on [9-Axis IMU](https://www.youtube.com/playlist?list=PLGs0VKk2DiYwEo-k0mjIkWXlkrJWAU4L9)

The goal is to correct the readings from the IMU to account for any biases or errors, and to provide accurate data for applications such as motion tracking, navigation, or robotics.

There will be a simulation of the IMU data to show the IMU readings in a visualization tool, such as a 3D model, using python.

## Files
- `IMU.code-workspace`: Visual Studio Code workspace file that organizes the project files.
- `ICM20948/`: Directory containing the code for interfacing with the ICM-20948 IMU, including libraries and example code.
- `Setup/`: Fritzing diagram of the breadboard setup for connecting the ICM-20948 to the Teensy 4.1. The parts for the Teensy 4.1 is taken from a discussion on the [Fritzing forum](https://forum.fritzing.org/t/teensy-4-1-part/11035) and the ICM-20948 is taken from the [Sparkfun repository](https://github.com/sparkfun/Fritzing_Parts/blob/main/products/15335_sfe_IMU_Breakout_ICM-20948.fzpz)
- `python/`: Directory containing Python code for simulating the IMU data and visualizing it using a 3D model.
- `README.md`: This file, providing an overview of the project and its contents.

## Components
- Teensy 4.1 microcontroller
- ICM-20948 Inertial Measurement Unit (IMU)
- Breadboard and jumper wires for connecting the components