

#include "opencv2/core/core.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <fstream>
    using std::ofstream;
#include <iostream>
    using std::cout;
    using std::endl;


using namespace cv;


int main(int argc, const char *argv[]) {
  if( argc != 3)
  {
   cout <<" Usage: convert inputfile outputfile" << endl;
   return -1;
  }
  ofstream stream;
  Mat image;
  Mat org_image;
  org_image = imread(argv[1], CV_LOAD_IMAGE_COLOR);   // Read the file

  cvtColor(org_image,image, CV_BGR2GRAY);

  if(! image.data )                              // Check for invalid input
  {
      cout <<  "Could not open or find the image" << std::endl ;
      return -1;
  }
  else{


    stream.open(argv[2]);
    if( !stream )
        cout << "Opening file failed" << endl;

    stream.write((char *)image.data,image.rows*image.cols);
    // test if write was succesful - not *really* necessary
    if( !stream )
        cout << "Write failed" << endl;
  }

  return 0;
}
