#include <bits/stdc++.h>
#include <opencv2/opencv.hpp>

#define N_IMG 3
#define CHANNEL 0

using namespace std;
using namespace cv;

Mat indoor[N_IMG], outdoor[N_IMG], outdoor_s[N_IMG];
MatND h_indoor[N_IMG], h_outdoor[N_IMG], h_outdoor_s[N_IMG];

void read_images() {

  string dir = "resources/taller1/";
    
  for (int i = 0; i < N_IMG; i++) {
    string d = dir+"indoor";
    d += char(i+1+'0');
    d += ".jpg";
    indoor[i] = imread(d.c_str(), CHANNEL);
    
    d = dir+"outdoor";
    d += char(i+1+'0');
    d += ".jpg";
    outdoor[i] = imread(d.c_str(), CHANNEL); 
    
    d = dir+"outdoors";
    d += char(i+1+'0');
    d += ".jpg";
    outdoor_s[i] = imread(d.c_str(), CHANNEL);
  }
}

void get_histograms(int bins) {

  int channels[] = {CHANNEL};
  int histSize[] = {bins};
  float range[] = {0, 256};
  const float *ranges[] = {range};
  
  for (int i = 0; i < N_IMG; i++) {
    calcHist(&indoor[i], 1, channels, Mat(), h_indoor[i], 1,
             histSize, ranges);
    calcHist(&outdoor[i], 1, channels, Mat(), h_outdoor[i], 1,
             histSize, ranges);
    calcHist(&outdoor_s[i], 1, channels, Mat(), h_outdoor_s[i], 1, 
             histSize, ranges);
  }
}

void show_match(MatND &src, MatND &dst) {
  
  cout << "Correlation: " << compareHist(src, dst, CV_COMP_CORREL) << endl;
  cout << "Chi-Square:  " << compareHist(src, dst, CV_COMP_CHISQR) << endl;
  cout << "Intersect:   " << compareHist(src, dst, CV_COMP_INTERSECT)
       << endl;
  cout << "Hellinger:   " << compareHist(src, dst, CV_COMP_BHATTACHARYYA)
       << endl;
}

int main() {
  read_images();
  
  int test_bins[] = {2, 16, 256};
  for (int i = 0; i < 3; i++) {
    get_histograms(test_bins[i]);
    
    cout << "Bins: " << test_bins[i] << "\n";
    cout << " - Train indoor1.jpg | Test indoor2.jpg\n";
    show_match(h_indoor[0], h_indoor[1]);
    cout << endl;
    cout << " - Train indoor1.jpg | Test indoor3.jpg\n";
    show_match(h_indoor[0], h_indoor[2]);
    cout << endl;
  }
	return 0;
}
