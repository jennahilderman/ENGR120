First-Year Robot Competition ECE120

Project Overview

This project was part of our first-year robot competition, where the goal was to connect a string to a target using a robot. 
The robot used infrared beacon detection to find its way to the target and drop a hook once it was close enough. 
Additionally, the robot had sensors on the front corners to detect walls and avoid obstacles.

Features

Infrared Beacon Detection: The robot uses an IR sensor to locate and track the beacon.
Wall Detection: Sensors located on the front corners help the robot detect walls and avoid collisions while navigating.
Signal Loss Adjustment: The robot adjusts its path left and right when it loses the beacon signal.
Hook Deployment: Once close to the beacon, the robot drops the hook to complete the task.
LED Feedback: LEDs on the robot provide visual feedback about its proximity to the target.
Code Explanation

The robot was programmed using ROBOTC, and the following features were implemented:

IR Sensor Handling: The robot reads from three IR sensors (front, left, and right) to track the beacon and adjust its path.
Wall Detection: The corner sensors detect walls and trigger movement adjustments to avoid obstacles.
Position Tracking: The robot tracks its state (straight, found, lost, right, left, stopped, started) to handle different movements.
Hook Deployment: The robot uses a claw mechanism to drop the hook once it reaches the beacon.
LED Feedback: LEDs provide visual feedback about the robot's status, indicating whether it's too close, just right, or too far from the target.

How It Works

Movement and Signal Adjustment: The robot continuously monitors IR signals. If it loses the beacon's signal, it makes small left and right adjustments until it re-acquires the signal.
Wall Detection and Avoidance: When the corner sensors detect a wall, the robot backs up and turns to avoid it.
Distance and Hook Deployment: When the robot is close enough to the beacon (measured by an ultrasonic sensor), it automatically drops the hook.
