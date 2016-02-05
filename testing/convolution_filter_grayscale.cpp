#include <cmath>
#include <opencv2/opencv.hpp>

using namespace cv;

  // Convolucio'n del pixel (r,c) en SRC con las mascara MASK con centro en (cr, cc)
uchar convolution(int r, int c, Mat &src, Mat &mask, int cr, int cc) {

  int sr = src.rows,  sc = src.cols;
  int mr = mask.rows, mc = mask.cols;
  int dr = r-cr+sr, dc = c-cc+sc;
  uchar v = 0;
  
  for (int i = 0; i < mr; i++) {
    uchar *s = src.ptr((dr+i)%sr);
    double *m = mask.ptr<double>(i);
    for (int j = 0; j < mc; j++) {
      v += s[(dc+j)%sc]*m[j];
    }
  }
  return v;
}

  // Aplica un filtro de convolucio'n a SRC con la mascara MASK centrada en MASK_CENTER 
  // y guarda el resultado en DST
  // Solo para ima'genes en escala de grises
void filter(Mat &src, Mat &dst, Mat &mask, Point &mask_center) {

  int sr = src.rows,  sc = src.cols;
  int mr = mask.rows, mc = mask.cols;
  int cr = mask_center.y, cc = mask_center.x;
  
  for (int i = 0; i < sr; i++) {
    uchar *d = dst.ptr(i);
    for (int j = 0; j < sc; j++) {
      d[j] = convolution(i, j, src, mask, cr, cc);
    }
  }
}

int main() {
  Mat src = imread("img.jpg", 0);
  Mat dst3x3 = src.clone();
  Mat dst15x15 = src.clone();
  Mat diff = src.clone();
    
    // Mascara 3x3
  int s = 3;
  Mat mask = Mat_<double>(s, s);
  for (int i = 0; i < s; i++) {
    double *m = mask.ptr<double>(i);
    for (int j = 0; j < s; j++) {
      m[j] = 1./(s*s);
    }
  }
  Point p(s/2, s/2);
  
    // Mascara 3x3 aplicada 7 veces
  for (int i = 0; i < 7; i++) {
    Mat a = dst3x3.clone();
    filter(a, dst3x3, mask, p);
  }
  
    // Mascara 15x15
  s = 15;
  mask = Mat_<double>(s, s);
  for (int i = 0; i < s; i++) {
    double *m = mask.ptr<double>(i);
    for (int j = 0; j < s; j++) {
      m[j] = 1./(s*s);
    }
  }
  p = Point(s/2, s/2);
  
    // Mascara 15x15 aplicada 1 vez
  filter(src, dst15x15, mask, p);
  
    // Imagen diferencia
  for (int i = 0; i < src.rows; i++) {
    for (int j = 0; j < src.cols; j++) {
      diff.ptr(i)[j] = abs(dst15x15.ptr(i)[j]-dst3x3.ptr(i)[j]);
    }
  }
  
  namedWindow("Src", CV_WINDOW_AUTOSIZE);
  imshow("Src", src);
  
  namedWindow("Dst 3x3", CV_WINDOW_AUTOSIZE);
  imshow("Dst 3x3", dst3x3);
  
  namedWindow("Dst 15x15", CV_WINDOW_AUTOSIZE);
  imshow("Dst 15x15", dst15x15);
 
  namedWindow("Diff", CV_WINDOW_AUTOSIZE);
  imshow("Diff", diff);
  
  waitKey(0);
  return 0;
}
