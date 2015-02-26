# Imports
import numpy as np
import cv2
import webiopi
import time

SERVO_LEFTMOST = 0.125
SERVO_RIGHTMOST = 0.065
SERVO_MIDDLE = 0.085
SERVO_ANGLE = SERVO_MIDDLE
SERVO_DELTA = 0.001



GPIO = webiopi.GPIO

def servoTurnLeft():
    global SERVO_ANGLE
    global SERVO_DELTA

    print "servoTurnLeft() was called"
    SERVO_ANGLE = SERVO_ANGLE + SERVO_DELTA
    if SERVO_ANGLE >= 0.125:
        SERVO_ANGLE = 0.125

    #GPIO.setFunction(14, GPIO.PWM)
    GPIO.pulseRatio(14, SERVO_ANGLE)
    #time.sleep(1)
    #GPIO.setFunction(14, GPIO.IN)

def servoTurnRight():
    global SERVO_ANGLE
    global SERVO_DELTA

    print "servoTurnRight() was called"
    SERVO_ANGLE = SERVO_ANGLE - SERVO_DELTA
    if SERVO_ANGLE <= 0.065:
        SERVO_ANGLE = 0.065

    #GPIO.setFunction(14, GPIO.PWM)
    GPIO.pulseRatio(14, SERVO_ANGLE)
    #time.sleep(1)
    #GPIO.setFunction(14, GPIO.IN)

def detectAndDisplay(frame1):
    frame2 = cv2.resize(frame1,(80, 60))
    #cv2.imshow('opencv_python', frame2)
    # Our operations on the frame come here
    gray = cv2.cvtColor(frame2, cv2.COLOR_BGR2GRAY)
    faces = face_cascade.detectMultiScale(gray, 1.1, 2)
    for (x,y,w,h) in faces:
        cv2.rectangle(frame2,(x,y),(x+w,y+h),(255,0,0),2)
        print x+w/2, y+h/2
        if x+w/2 < 30:
            servoTurnLeft()
        if x+w/2 > 50:
            servoTurnRight() 

    cv2.imshow('opencv_python', frame2)
	

GPIO.setFunction(14, GPIO.PWM)
GPIO.pulseRatio(14, SERVO_MIDDLE)

SERVO_ANGLE = SERVO_MIDDLE


cap = cv2.VideoCapture(0)
face_cascade = cv2.CascadeClassifier('./haarcascades/haarcascade_frontalface_alt.xml')

while(True):
    # Capture frame-by-frame
    ret, frame = cap.read()

    # Our operations on the frame come here
    #gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    # Display the resulting frame
    #cv2.imshow('frame',gray)
    
    detectAndDisplay(frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# When everything done, release the capture
GPIO.setFunction(14, GPIO.IN)
cap.release()
cv2.destroyAllWindows()

