#include <bits/stdc++.h>
#include <opencv2/opencv.hpp>
#include "process.h"
#include "segment.h"
#include "characterize.h"

#include <dirent.h>

#define VLEN 16
string V[] = {"0", "A", "1", "2", "3", "4", "5", "6", "7", "8",
              "J", "Q", "K", "C", "H", "D", "S"};
              
using namespace std;
using namespace cv;

int main() {

  string root = "./data/poker/dataset_v2/";
  vector<string> file;
  
  DIR *dr;
  dirent *drnt;
  dr = opendir(root.c_str());
  drnt = readdir(dr);
  
  while (drnt) {
    string name = drnt->d_name;
    if (name == "." || name == "..") {
      drnt = readdir(dr);
      continue;
    }
    file.push_back(root+name);
    drnt = readdir(dr);
  }
  closedir(dr);
  
  string rootout = "./data/poker/";
  ofstream outf[256];
  for (int i = 0; i < 17; i++) {
    outf[V[i][0]].open((rootout+V[i]).c_str(), fstream::out|fstream::app);
  }
  
  for (int fi = 0; fi < file.size(); fi++) {
    Mat src = imread(file[fi].c_str(), 1);
    Mat dst = process(src);
    vector<card_t> res = segment(dst, src);
      
    for (int i = 0; i < res.size(); i++) {
      Mat s = res[i].s;
      Mat v = res[i].v;

      double hu[VEC_LEN];
      
      namedWindow("s", CV_WINDOW_AUTOSIZE);
      imshow("s", s);
      int ks = waitKey(0);
      destroyWindow("s");
      
      if (ks != 'x') {
        characterize(s, hu);
        ks = toupper(ks);
        for (int j = 0; j < VEC_LEN; j++) {
          outf[ks] << hu[j] << " ";
        }
        outf[ks] << endl;
      }
      
      namedWindow("v", CV_WINDOW_AUTOSIZE);
      imshow("v", v);
      int kv = waitKey(0);
      destroyWindow("v");
      
      if (kv != 'x') {
        characterize(v, hu);
        kv = toupper(kv);
        for (int j = 0; j < VEC_LEN; j++) {
          outf[kv] << hu[j] << " ";
        }
        outf[kv] << endl;
      }
    }
  }
  destroyAllWindows();
  
  for (int i = 0; i < 17; i++) {
    outf[i].close();
  }
  return 0;
}
