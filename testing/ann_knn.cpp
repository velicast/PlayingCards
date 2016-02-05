void get_trdata(vector<vector<double> > &v_trdata,
                vector<vector<double> > &v_labels) {
  ifstream input("./data/poker/trdata");  
    
  int n, m;
  
  input >> n >> m;
  
  v_trdata.resize(n, vector<double>(XLEN));
  v_labels.resize(n, vector<double>(YLEN));
  
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < XLEN; j++) {
      input >> v_trdata[i][j];
    }
    for (int j = 0; j < YLEN; j++) {
      input >> v_labels[i][j];
      /*if (v_labels[i][j] < 0) {
        v_labels[i][j] = 0;
      }*/
    }
  }
}

/* Inicializa la red neuronal que clasifica los elementos de una carta */
CvANN_MLP create_ann() {

  CvANN_MLP mlp;
  CvANN_MLP_TrainParams params;
  CvTermCriteria criteria;

  criteria.max_iter = 2000;
  criteria.epsilon  = 0.00001f;
  criteria.type     = CV_TERMCRIT_ITER | CV_TERMCRIT_EPS;

  params.train_method    = CvANN_MLP_TrainParams::BACKPROP;
  params.bp_dw_scale     = 0.01f;
  params.bp_moment_scale = 0.005f;
  params.term_crit       = criteria;

  Mat layers = Mat(4, 1, CV_32SC1);

  layers.row(0) = Scalar(XLEN);
  layers.row(1) = Scalar(70);
  layers.row(2) = Scalar(70);
  layers.row(3) = Scalar(YLEN);

  mlp.create(layers, CvANN_MLP::SIGMOID_SYM, 1, 1);
  
  vector<vector<double> > v_trdata;
  vector<vector<double> > v_labels;
  
  get_trdata(v_trdata, v_labels);
  
  int n = v_trdata.size();
  
  Mat labels(n, YLEN, CV_32FC1);
  Mat trdata(n, XLEN, CV_32FC1);
  
  for (int i = 0; i < n; i++) {
    float* tr = trdata.ptr<float>(i);
    for (int j = 0; j < XLEN; j++) {
      tr[j] = v_trdata[i][j];
      //cout << tr[j] << " ";
    }
    //cout << endl;
  }
  for (int i = 0; i < n; i++) {
    float* lab = labels.ptr<float>(i);
    for (int j = 0; j < YLEN; j++) {
      lab[j] = v_labels[i][j];
      //cout << lab[j] << " ";
    }
    //cout << endl;
  }
  
  mlp.train(trdata, labels, Mat(), Mat(), params);
  
  for (int i = 0; i < trdata.rows; i++) {
    Mat sample = trdata.row(i);
    Mat pred(1, YLEN, CV_32FC1);
    mlp.predict(sample, pred);
    for (int j = 0; j < YLEN; j++) {
      //cout << pred.ptr<float>(0)[j] << " ";
      if (pred.ptr<float>(0)[j] < 0) {
        cout << 0;
      } else {
        cout << 1;
      }
    }
    cout << endl;
    //cout << pred << endl;
  }
  Mat pred(1, YLEN, CV_32FC1);
  
  Mat samp(1, XLEN, CV_32FC1);  
  samp.ptr<float>(0)[0] = 25;
  samp.ptr<float>(0)[1] = 0;
  samp.ptr<float>(0)[2] = 0;
  samp.ptr<float>(0)[3] = 0;
  samp.ptr<float>(0)[4] = 0;
  samp.ptr<float>(0)[5] = 0;
  samp.ptr<float>(0)[6] = 0;  
            
  mlp.predict(samp, pred);
  cout << endl;  
  for (int j = 0; j < YLEN; j++) {
    cout << pred.ptr<float>(0)[j] << " ";
    /*if (pred.ptr<float>(0)[j] < 0) {
      cout << 0;
    } else {
      cout << 1;
    }*/
  }  
  cout << endl;
  return mlp;
}

float toval(vector<double> a) {

  int v = 0, e = 1;
  for (int i = 0; i < a.size(); i++) {
    v += (a[i] == 1)*e;
    e <<= 1;
  }
  return v;
}
  
CvKNearest create_knn() {
  
  vector<vector<double> > v_trdata;
  vector<vector<double> > v_labels;
  
  get_trdata(v_trdata, v_labels);
  
  int n = v_trdata.size();
  
  Mat labels(n, 1, CV_32FC1);
  Mat trdata(n, XLEN, CV_32FC1);
  
  for (int i = 0; i < n; i++) {
    float* tr = trdata.ptr<float>(i);
    for (int j = 0; j < XLEN; j++) {
      tr[j] = v_trdata[i][j];
    }
  }
  for (int i = 0; i < n; i++) {
    float* lab = labels.ptr<float>(i);
    for (int j = 0; j < 1; j++) {
      lab[j] = toval(v_labels[i]);
      //cout << lab[j] << " ";
    }
    //cout << endl;
  }
  
  CvKNearest knn(trdata, labels);
  Mat m;
  for (int i = 0; i < n; i++) {
    labels.ptr<float>(i)[0] = -1;
  }
  knn.find_nearest(trdata, 2, labels, m, m);
  for (int j = 0; j < n; j++) {
    cout << labels.ptr<float>(j)[0] << endl;
  }
  return knn;
}
