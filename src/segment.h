#ifndef SEGMENT_H
#define SEGMENT_H

#include <bits/stdc++.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

struct card_t {
  Rect rv, rs;
  Mat v, s;
  int mv, ms;
  
  card_t(Rect& rrv, Rect& rrs, Mat& vv, Mat& vs, int mmv, int mms) {
    rs = rrs;
    rv = rrv;
    s = vs;
    v = vv;
    mv = mmv;
    ms = mms;
  }
};

vector<card_t> segment(Mat&, Mat&);

Scalar get_avg_color(card_t&, Mat&);

int check9(card_t&);

#endif
