
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "detector.h"
#include "object.h"

#include "opencv2/core/core.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

#define MAX_FACES 10
#define DETECT_SKIP 10

using namespace cv;
using namespace std;

int main(int argc, const char *argv[]) {
    int deviceId = 0;
    // Get a handle to the Video device:
    VideoCapture cap(deviceId);
    // Check if we can use this device at all:
    if(!cap.isOpened()) {
        cerr << "Capture Device ID " << deviceId << "cannot be opened." << endl;
        return -1;
    }
    int width = 640;
    int height = 480;
    cap.set(CV_CAP_PROP_FRAME_WIDTH, width);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, height);

    Detector *det = new Detector(width, height, height/10);
    int frames = 0;

    Object *fa=new Object[MAX_FACES];
    static int detected_faces = 0;


    Mat frame;
    for(;;) {
        cap >> frame;
        // Clone the current frame:
        Mat original = frame.clone();
        // Convert the current frame to grayscale:
        Mat gray;
        cvtColor(original, gray, CV_BGR2GRAY);

        vector< Rect_<int> > faces;

        int numfaces = MAX_FACES;

            det->detect( gray.data, fa, &numfaces);
            cout << "Run detect" << endl;

        detected_faces = numfaces;
        frames++;
        for(int j=0;j<numfaces;j++) {
            faces.push_back(Rect(fa[j].x, fa[j].y, fa[j].width, fa[j].height));
        }


        for(int i = 0; i < faces.size(); i++) {

            Rect face_i = faces[i];

            Mat face = gray(face_i);

            rectangle(original, face_i, CV_RGB(0, 255,0), 1);
        }
        // Show the result:
        imshow("face_recognizer", original);
        // And display it:
        char key = (char) waitKey(20);
        // Exit this loop on escape:
        if(key == 27)
            break;
    }
    return 0;
}
