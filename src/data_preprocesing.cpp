#include <bits/stdc++.h>
#include <opencv2/opencv.hpp>
#include "characterize.h"
#include "classify.h"

using namespace std;
using namespace cv;

int vec_class_value[256][CLASS_LEN];
string v_class[] = {"A", "1", "2", "3", "4", "5", "6", "7", "8", "J", "Q", "K"};
string s_class[] = {"C", "H", "D", "S"};

void init_class_values() {
#define c vec_class_value
  c['A'][0] = -1; c['A'][1] = -1; c['A'][2] = -1; c['A'][3] = -1;
  c['1'][0] =  1; c['1'][1] = -1; c['1'][2] = -1; c['1'][3] = -1;
  c['2'][0] = -1; c['2'][1] =  1; c['2'][2] = -1; c['2'][3] = -1;
  c['3'][0] = 1; c['3'][1] = 1; c['3'][2] = -1; c['3'][3] = -1;
  c['4'][0] = -1; c['4'][1] = -1; c['4'][2] = 1; c['4'][3] = -1;
  c['5'][0] = 1; c['5'][1] = -1; c['5'][2] = 1; c['5'][3] = -1;
  c['6'][0] = -1; c['6'][1] = 1; c['6'][2] = 1; c['6'][3] = -1;
  c['7'][0] = 1; c['7'][1] = 1; c['7'][2] = 1; c['7'][3] = -1;
  c['8'][0] = -1; c['8'][1] = -1; c['8'][2] = -1; c['8'][3] = 1;
  c['J'][0] = 1; c['J'][1] = -1; c['J'][2] = -1; c['J'][3] = 1;
  c['Q'][0] = -1; c['Q'][1] = 1; c['Q'][2] = -1; c['Q'][3] = 1;
  c['K'][0] = 1; c['K'][1] = 1; c['K'][2] = -1; c['K'][3] = 1;
    // suits
  c['C'][0] = -1; c['C'][1] = -1; c['C'][2] = 1; c['C'][3] = 1;
  c['S'][0] = 1; c['S'][1] = -1; c['S'][2] = 1; c['S'][3] = 1;
  c['D'][0] = -1; c['D'][1] = 1; c['D'][2] = 1; c['D'][3] = 1;
  c['H'][0] = 1; c['H'][1] = -1; c['H'][2] = 1; c['H'][3] = 1;
#undef c
}

void load(const char *path, char v, vector<vector<double> > &tr) {

  ifstream in(path);
  int n;
  
  in >> n;
  for (int i = 0; i < n; i++) {
    vector<double> a(VEC_LEN);
    for (int j = 0; j < VEC_LEN; j++) {
      in >> a[j];
    }
    for (int j = 0; j < CLASS_LEN; j++) {
      a.push_back(vec_class_value[v][j]);
    }
    tr.push_back(a);
  }
}

void save(vector<vector<double> > &tr) {

  int n = tr.size();
  
  cout << n << endl;
  for (int i = 0; i < n; i++) {
    vector<double> a = tr[i];
    int sz = a.size();
    cout << a[0];
    for (int j = 1; j < sz; j++) {
      cout << " " << a[j];
    }
    cout << endl;
  }
}

int main() {
  init_class_values();
  
  vector<vector<double> > s_trdata;
  vector<vector<double> > v_trdata;
  
  string root = "./data/poker/";
  
    // --------------------------
    // Crer Dataset de las pintas
    // --------------------------
  freopen("./data/poker/s_trdata", "w", stdout);
  for (int i = 0; i < 4; i++) {
    load((root+s_class[i]).c_str(), s_class[i][0], s_trdata);
  }
  save(s_trdata);
  
    // ---------------------------
    // Crer Dataset de los valores
    // ---------------------------
  freopen("./data/poker/v_trdata", "w", stdout);
  for (int i = 0; i < 12; i++) {
    load((root+v_class[i]).c_str(), v_class[i][0], v_trdata);
  }
  save(v_trdata);
  return 0;
}
