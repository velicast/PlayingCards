#ifndef CLASSIFY_H
#define CLASSIFY_H

#include <bits/stdc++.h>
#include <opencv2/opencv.hpp>
#include "characterize.h"

#define CLASS_LEN 4

using namespace std;
using namespace cv;

int read_data(const char *, vector<vector<double> > &, vector<vector<double> > &);

CvANN_MLP get_mlp(const char *, double, double);

CvKNearest get_knn(const char *);

float toval(vector<double>);

#endif
