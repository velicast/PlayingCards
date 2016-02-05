#include "process.h"

void invert(Mat&);

  /* Inverte la imagen binaria */
void invert(Mat& img) {

  int cols = img.cols;
  int rows = img.rows;
  for (int i = 0; i < rows; i++) {
    uchar *r = img.ptr(i);
    for (int j = 0; j < cols; j++) {
      r[j] = 255-r[j];
    }
  }
}

  /* Binarizacion y eliminacion de ruido */
Mat process(Mat src) {

  Mat dst;
  cvtColor(src, dst, CV_BGR2GRAY);
  src = dst.clone();
  threshold(src, dst, 0, 255, CV_THRESH_BINARY|CV_THRESH_OTSU);
  invert(dst);
  
  int sz = 1;
  Mat elem = getStructuringElement(MORPH_ELLIPSE, Size(2*sz+1, 2*sz+1),
                                   Point(sz, sz));
  src = dst.clone();
  dilate(src, dst, elem);
  src = dst.clone();
  erode(src, dst, elem);
  
  return dst;
}

