#include "classify.h"

int read_data(const char *tr_path,
                vector<vector<double> > &v_trdata,
                vector<vector<double> > &v_labels) {

  ifstream input(tr_path);
  int n;
  input >> n;
  
  v_trdata.resize(n, vector<double>(VEC_LEN));
  v_labels.resize(n, vector<double>(CLASS_LEN));
  
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < VEC_LEN; j++) {
      input >> v_trdata[i][j];
    }
    for (int j = 0; j < CLASS_LEN; j++) {
      input >> v_labels[i][j];
    }
  }
  return n;
}

CvANN_MLP get_mlp(const char *tr_path, double p0, double p1) {

  CvANN_MLP mlp;
  CvANN_MLP_TrainParams params;
  CvTermCriteria criteria;

  criteria.max_iter = 1000;
  criteria.epsilon  = 0.00001f;
  criteria.type     = CV_TERMCRIT_ITER | CV_TERMCRIT_EPS;
  
  params.train_method    = CvANN_MLP_TrainParams::BACKPROP;
  params.bp_dw_scale     = p0;
  params.bp_moment_scale = p1;
  params.term_crit       = criteria;
  
  Mat layers = Mat(4, 1, CV_32SC1);

  layers.row(0) = Scalar(VEC_LEN);
  layers.row(1) = Scalar(70);
  layers.row(2) = Scalar(70);
  layers.row(3) = Scalar(CLASS_LEN);
  
  mlp.create(layers, CvANN_MLP::SIGMOID_SYM, 1, 1);
  
  vector<vector<double> > v_trdata;
  vector<vector<double> > v_labels;
  
  int n = read_data(tr_path, v_trdata, v_labels);
  Mat trdata(n, VEC_LEN, CV_32FC1);
  Mat labels(n, CLASS_LEN, CV_32FC1);
  
  for (int i = 0; i < n; i++) {
    float* tr = trdata.ptr<float>(i);
    for (int j = 0; j < VEC_LEN; j++) {
      tr[j] = v_trdata[i][j];
    }
  }
  for (int i = 0; i < n; i++) {
    float* lab = labels.ptr<float>(i);
    for (int j = 0; j < CLASS_LEN; j++) {
      lab[j] = v_labels[i][j];
    }
  }
  
  mlp.train(trdata, labels, Mat(), Mat(), params);
  return mlp;
}

float toval(vector<double> a) {

  int v = 0, e = 1;
  for (int i = 0; i < a.size(); i++) {
    v += (a[i] >= 0)*e;
    e <<= 1;
  }
  return v;
}

CvKNearest get_knn(const char *tr_path) {
  
  vector<vector<double> > v_trdata;
  vector<vector<double> > v_labels;
  
  int n = read_data(tr_path, v_trdata, v_labels);
  Mat labels(n, 1, CV_32FC1);
  Mat trdata(n, VEC_LEN, CV_32FC1);
  
  for (int i = 0; i < n; i++) {
    float* tr = trdata.ptr<float>(i);
    for (int j = 0; j < VEC_LEN; j++) {
      tr[j] = v_trdata[i][j];
    }
  }
  for (int i = 0; i < n; i++) {
    float* lab = labels.ptr<float>(i);
    for (int j = 0; j < 1; j++) {
      lab[j] = toval(v_labels[i]);
    }
  }
  CvKNearest knn(trdata, labels);
  return knn;
}
