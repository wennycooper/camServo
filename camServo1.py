# Imports
import numpy as np
import cv2
import os

def servoTurnLeft():
    print "servoTurnLeft() was called"
    os.system("echo P1-8=+2 > /dev/servoblaster");

def servoTurnRight():
    print "servoTurnRight() was called"
    os.system("echo P1-8=-2 > /dev/servoblaster");

def servoMiddle():
    os.system("echo P1-8=180 > /dev/servoblaster");

def servoClose():
    os.system("echo P1-8=0 > /dev/servoblaster");

def detectAndDisplay():
    frame2 = cv2.resize(frame,(80, 60))
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
	

servoMiddle()

cap = cv2.VideoCapture(0)
face_cascade = cv2.CascadeClassifier('./haarcascades/haarcascade_frontalface_alt.xml')

while(True):
    # Capture frame-by-frame
    ret, frame = cap.read()

    # Our operations on the frame come here
    #gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    # Display the resulting frame
    #cv2.imshow('frame',gray)
    
    detectAndDisplay()
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# When everything done, release the capture

servoClose()
cap.release()
cv2.destroyAllWindows()

