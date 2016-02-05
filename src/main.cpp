#include <bits/stdc++.h>
#include <opencv2/opencv.hpp>
#include "process.h"
#include "segment.h"
#include "characterize.h"
#include "classify.h"

using namespace std;
using namespace cv;

string class_name[] = {"A", "10", "2", "3", "4", "5", "6", "7", "8", "J", "Q", "K", "C", "HS", "D"};

int main(int argc, char** argv) {

  VideoCapture cam;
  if (argc <= 1) {
    cam.open(0);
  } else if (argc == 2) {
    string param = argv[1];
    if (param == "-h") {
      cout << "Usage: -option [value]\n";
      cout << "Options:\n";
      cout << "-d Input stream from camera device (device number)\n";
      cout << "-f Input stream from media file (path to file)" << endl;
      return 0;
    } else {
      cout << "please visit -h for help" << endl;
      return -1;
    }
  }
  else if (argc <= 3) {
    string param = argv[1];
    if (param == "-d") {
      int device = atoi(argv[2]);
      cam.open(device);
    }
    else if (param == "-f") {
      string file = argv[2];
      cam.open(file);
    } else {
      cout << "please visit -h for help" << endl;
      return -1;
    }
  } else {
      cout << "please visit -h for help" << endl;
    return -1;
  }
  
  if (!cam.isOpened()) {
    cout << "Error when opening the input stream" << endl;
    return -1;
  }
  cam.set(CV_CAP_PROP_FRAME_WIDTH, 640);
  cam.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
  
  srand((unsigned)time(0));
  namedWindow("Poker Card Classifier", CV_WINDOW_AUTOSIZE);
    
  FILE *f = fopen("./s_trdata", "r");
  if (!f) {
    cout << "Missing s_trdata" << endl;
    return -1;
  }
  fclose(f);  
  f = fopen("./v_trdata", "r");
  if (!f) {
    cout << "Missing v_trdata" << endl;
    return -1;
  }
  fclose(f);
  
  cout << "Training classifier with s_trdata ..." << endl;
  CvANN_MLP s_mlp = get_mlp("./s_trdata", 0.1, 0.1);
  cout << "Training classifier with v_trdata ..." << endl;
  CvANN_MLP v_mlp = get_mlp("./v_trdata", 0.01, 0.1);
  
  cout << "OK! (press q to quit)" << endl;
  
  while (1) {
    Mat src, dst;
    if (!cam.read(src)) {
      return 0;
    }
    dst = process(src);

    int key = waitKey(50);
    if (key == 'q') {
      break;
    }

    imshow("Poker Card Classifier", src);     
    //Mat g_dst = dst.clone();
    vector<card_t> res = segment(dst, src);
    //cvtColor(g_dst, dst, CV_GRAY2RGB);
    
    for (int i = 0; i < res.size(); i++) {
      Mat s = res[i].s;
      Mat v = res[i].v;
      
      double hu[VEC_LEN];
      Mat test(1, VEC_LEN, CV_32FC1);
      Mat pred(1, CLASS_LEN, CV_32FC1);
      vector<double> dec(CLASS_LEN);
      
        // ---------------
        // Clasificar SUIT
      characterize(s, hu);
      for (int hh = 0; hh < VEC_LEN; hh++) {
        test.ptr<float>(0)[hh] = hu[hh];
      }
      s_mlp.predict(test, pred);

      for (int j = 0; j < CLASS_LEN; j++) {
        dec[j] = pred.ptr<float>(0)[j];
      }
      int sp = toval(dec);
        // ----------------
        // Clasificar VALOR
      characterize(v, hu);
      for (int hh = 0; hh < VEC_LEN; hh++) {
        test.ptr<float>(0)[hh] = hu[hh];
      }
      v_mlp.predict(test, pred);
      
      for (int j = 0; j < CLASS_LEN; j++) {
        dec[j] = pred.ptr<float>(0)[j];
      }
      int vp = toval(dec);

      rectangle(src,
                Point(res[i].rv.x, res[i].rv.y),
                Point(res[i].rv.width, res[i].rv.height),
                Scalar(255, 0, 0),
                1);
      rectangle(src,
                Point(res[i].rs.x, res[i].rs.y),
                Point(res[i].rs.width, res[i].rs.height),
                Scalar(255, 0, 0),
                1);
                
      Scalar cc(rand()%128, rand()%128, rand()%128);
                      
      if (vp < 12) {
        string classy = class_name[vp];
        if (vp == 6) {
          classy = "6";
          if (check9(res[i])) {
            classy = "9";
          }
        }
        putText(src,
                classy,
                Point(res[i].rv.width+5, res[i].rv.height),
                FONT_HERSHEY_SIMPLEX,
                1,
                cc,
                3);
      }
      
      if (sp > 11 && sp < 15) {
        string classy = class_name[sp];
        if (sp == 13) {
          classy = "S";
          Scalar col = get_avg_color(res[i], src);
          int r = col[2];
          if (r == max(col[0], max(col[1], col[2])) && r > 50) {
            classy = "H";
          }
        }
        putText(src,
                classy,
                Point(res[i].rs.width+5, res[i].rs.height),
                FONT_HERSHEY_SIMPLEX,
                1,
                cc,
                3);
      }
    }
    imshow("Poker Card Classifier", src);
    waitKey(50);
  }
  return 0;
}
