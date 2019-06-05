#ifndef _LBP_H
#define _LBP_H

#include <stdint.h>
#include <iostream>
#include <vector>
#include <iostream>
#include <fstream>
#include <errno.h>
#include <math.h>
#include <stdlib.h>

#include "lbpcommon.h"


using namespace std;



class Lbp{



public:

  LbpData data;
  LbpPara para;
  int width;
  int height;
  std::vector<LbpRect> detected_r; /* must use new/delete instead of malloc/free because of this */
  std::vector<LbpTask> tasks;

  unsigned int getValueBilinear(unsigned int *img, float x, float y, int width);
  void getInterpolatedIntegralValue(LbpRect *r, unsigned int *img, int x, int y, int width, float scale, unsigned int *p);
  float lbpClassify(LbpRect *r, WeakClassifier *c, unsigned int *img, int x, int y, int width, int height, float scale);




};
#endif
