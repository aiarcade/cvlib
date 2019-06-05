

#ifndef _LBPCOMMON_H
#define _LBPCOMMON_H

#include <stdint.h>
#include <iostream>
using namespace std;

class LbpTask {
  public:
    int x;
    int y;
    float scale;

};

class LbpRect{
  public:
    int x;
    int y;
    int w;
    int h;
};

class WeakClassifier{
  public:
    int rect_idx;
    int lbpmap[8];
    float pos;
    float neg;
};

class Stage{
  public:
    float stage_threshold;
    int num_weak_classifiers;
    WeakClassifier * classifiers;

};

class LbpData{
  public:
    int feature_width;
    int feature_height;
    int num_stages;
    Stage *s;
    int num_rects;
    LbpRect *r;

};

class LbpPara{
  public:
    float scaling_factor;
    int step_scale_x;
    int step_scale_y;
    float tracking_scale_down;
    float tracking_scale_up;
    float tracking_offset; // width percentage
    float eps;
    int group_threshold;
    int min_face_width;

};

//typedef float (*lbp_classify_t) (struct lbp_rect *r, struct weak_classifier *c, unsigned int *img, int x, int y, int width, int height, float scale);
//extern lbp_classify_t pf_lbp_classify;

#endif
