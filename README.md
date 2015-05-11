# camServo
RPi + usbCam + opencv face detection + servoBlaster 

# Demo
https://www.youtube.com/watch?v=7V13thR7rHw

## Requirements
1. opencv 2.4.x
2. servoblaster  (https://github.com/richardghirst/PiBits/tree/master/ServoBlaster)
3. two servos
4. UVC compatible usb camera

## Build Instruction:
g++ -o ./camServo1 ./camServo1.cpp `pkg-config --cflags --libs opencv`



