#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <fcntl.h>
#include <time.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;
using namespace cv;

#define MOUSEFILE "/dev/input/mouse0\0"
#define SERVOBLASTERFILE "/dev/servoblaster"

int fd;
int fd_sb;
char str1[256];
char str2[256];


/** Function Headers */

void servoTurnLeft()
{
    //printf("servoTurnLeft() was called\n");
    //system("echo P1-24=+3 > /dev/servoblaster");
    sprintf(str1, "P1-24=+3\n");
    write(fd_sb, str1, strlen(str1));

}

void servoTurnRight()
{
    //printf("servoTurnRight() was called\n");
    //system("echo P1-24=-3 > /dev/servoblaster");
    sprintf(str1, "P1-24=-3\n");
    write(fd_sb, str1, strlen(str1));

}

void servoMiddle()
{
    //printf("servoMiddle() was called\n");
    //system("echo P1-24=180 > /dev/servoblaster");

    sprintf(str1, "P1-24=180\n");
    write(fd_sb, str1, strlen(str1)); 

}

void servoClose()
{
    //printf("servoClose() was called\n");
    //system("echo P1-24=0> /dev/servoblaster");
    sprintf(str1, "P1-24=0\n");
    write(fd_sb, str1, strlen(str1));
    sprintf(str2, "P1-26=0\n");
    write(fd_sb, str2, strlen(str2));
}





void detectAndDisplay( Mat frame );

/** Global variables */


String face_cascade_name = "./haarcascades/haarcascade_frontalface_alt.xml";
String eyes_cascade_name = "./haarcascades/haarcascade_eye_tree_eyeglasses.xml";


CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
String window_name = "Capture - Face detection1";

int gotFace = 0;
Mat croppedImage;

clock_t begin_time;

/** @function main */
int main( void )
{

    servoMiddle();
    VideoCapture stream1(0);
    Mat frame;

    if((fd_sb = open(SERVOBLASTERFILE, O_WRONLY)) == -1)
    {
        printf("Opening %s got ERROR\n", SERVOBLASTERFILE);
        exit(EXIT_FAILURE);

    }

    servoMiddle();


    //-- 1. Load the cascades
    if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading face cascade\n"); return -1; };
    //if( !eyes_cascade.load( eyes_cascade_name ) ){ printf("--(!)Error loading eyes cascade\n"); return -1; };

    //-- 2. Read the video stream
    //stream1.open( 1 );
    if ( ! stream1.isOpened() ) { printf("--(!)Error opening video capture\n"); return -1; }

    begin_time = clock();
    while ( stream1.read(frame) )
    {
        if( frame.empty() )
        {
            printf(" --(!) No captured frame -- Break!");
            break;
        }

        //-- 3. Apply the classifier to the frame
        detectAndDisplay( frame );

        int c = waitKey(10);
        if( (char)c == 27 ) { 
	    break; } // escape

    }

    servoClose();
    close(fd_sb);

    return 0;
}

/** @function detectAndDisplay */
void detectAndDisplay( Mat frame_src )
{
	static int matchCounter = 1;
	static int noMatchCounter = 0;
    std::vector<Rect> faces;
    Mat frame;
    Mat frame_gray;
    Mat result;
    int match_method = CV_TM_SQDIFF_NORMED;
    int match = 0;



    resize(frame_src, frame, Size(80, 60));

    cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
    equalizeHist( frame_gray, frame_gray );

    //-- Detect faces by Cascade Classification

    if (!gotFace) {
		face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CASCADE_SCALE_IMAGE, Size(10, 10), Size(40, 30) );

		for ( size_t i = 0; i < faces.size(); i++ )
		{
			gotFace = 1;
			Point center( faces[i].x + faces[i].width/2, faces[i].y + faces[i].height/2 );


			Mat faceROI = frame_gray( faces[i] );

			frame_gray( faces[i] ).copyTo(croppedImage);
			//imshow( "croppedImage1", croppedImage );

		}
    } else {

    // let's doing template matching
    // 	 frame_gray as original image
    //   croppedImage as patch

    	match = 0;
    	int result_cols = frame_gray.cols - croppedImage.cols + 1;
    	int result_rows = frame_gray.rows - croppedImage.rows + 1;
    	result.create( result_cols, result_rows, CV_32FC1 );


    	/// Do the Matching and Normalize
    	matchTemplate( frame_gray, croppedImage, result, match_method);
    	//normalize( result, result, 0, 1, NORM_MINMAX, -1.0, Mat() );

    	/// Localizing the best match with minMaxLoc
    	double minVal; double maxVal; Point minLoc; Point maxLoc;
    	Point matchLoc;

    	minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );

    	/// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
    	if( match_method  == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED )
    	    {
    			if (minVal <=0.25) {
    				matchLoc = minLoc;
    				match = 1;
    			}
    			//printf("minVal = %f\n", (float) minVal);
    			//printf("maxVal = %f\n", (float) maxVal);
    	    }
    	else
    	    { matchLoc = maxLoc; }

    	/// Show me what you got
    	if (match) {
    		noMatchCounter = 0;
    		rectangle(frame, matchLoc, Point( matchLoc.x + croppedImage.cols , matchLoc.y + croppedImage.rows ), Scalar::all(0), 2, 8, 0 );
                //printf("x=%d, y=%d, w=%d, h=%d\n", matchLoc.x, matchLoc.y, croppedImage.cols, croppedImage.rows);

		if (matchLoc.x+croppedImage.cols/2 < 30) servoTurnLeft();
                if (matchLoc.x+croppedImage.cols/2 > 50) servoTurnRight();
    	} else {
    		noMatchCounter ++;
    		if (noMatchCounter >= 75) gotFace=0;
    	}



    }

    printf("FPS = %f\n", (float) matchCounter / ((clock() - begin_time)/CLOCKS_PER_SEC));
    cout.flush();
    //-- Show what you got
    imshow( window_name, frame );
    matchCounter++;


}
