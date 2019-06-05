#ifndef _DETECTOR_H
#define _DETECTOR_H




#include "lbp.h"
#include "object.h"


class Detector{



private:
  Lbp *l;
  int width;
  int height;
  unsigned int *integral_img;

public :

Detector(int width, int height, int minimum_face_width)
  {
      l = ObjectDetectorLbpCreate(width, height, minimum_face_width);
      this->width = width;
      this->height = height;
      this->integral_img = (unsigned int *)malloc(width * height * sizeof(unsigned int));

  }

  int lbpDetect(Lbp *l, unsigned int *img, int x, int y, int width, int height, float scale);
  int objectDetectorLbpDetect(Lbp *l, unsigned int *img, Object *fa, int *maxfaces);
  void addLbpObject(Lbp *l, int x, int y, float scale);
  Lbp * ObjectDetectorLbpCreate(int width, int height, int minimum_face_width);
  int loadLbpData(Lbp *l);
  void initTask(Lbp *l);
  void ObjectDetectorGroupRectangle(std::vector<LbpRect>& rect_list, int group_threshold, float eps);
  int partition(std::vector<LbpRect>& _vec, std::vector<int>& labels, float eps);
  int predicate(float eps, LbpRect& r1, LbpRect& r2);
  void detectorGenIntegralImage(unsigned int *i_data, unsigned char *data, int width, int height);
  int detect( unsigned char *y,Object *fa, int *maxfaces);

  int max(int a, int b);
  int min(int a, int b);
  int round(float value);
  int abs(int n);




};



#endif
