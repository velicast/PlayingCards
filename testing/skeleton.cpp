#include <opencv2/opencv.hpp>
using namespace cv;

  // Calcula el esqueleto de la ima'gen A con elemento estructurante B
Mat skeleton(Mat A, Mat &B) {
  Mat E, O, S = Mat::zeros(A.rows, A.cols, CV_8UC1);
  do {
    erode(A, E, B);
    A = E.clone();
    erode(A, E, B);
    dilate(E, O, B);
    S |= A-O;
  } while (countNonZero(E));
  return S;
}

int main() {
  Mat src = imread("img.jpg", 0);
  Mat ele = getStructuringElement(MORPH_RECT, Size(3, 3), Point(1, 1));
  Mat ske = skeleton(src, ele);
  
  namedWindow("src", CV_WINDOW_AUTOSIZE);
  imshow("src", src);
  namedWindow("ske", CV_WINDOW_AUTOSIZE);
  imshow("ske", ske);
  
  waitKey(0);
	return 0;
}

