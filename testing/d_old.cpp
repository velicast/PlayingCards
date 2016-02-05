#include <bits/stdc++.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

#define BKG 1
#define CARD 0
#define VLEN 16

string V[] = {"0", "A", "2", "3", "4", "5", "6", "7", "8", "J", "Q", "K",
              "S", "D", "H", "C"};
              
// -------------------------- //
// PROCESAMIENTO DE LA IMAGEN //
// -------------------------- //

void invert(Mat&);
Mat process(Mat);

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

// ------------------------- //
// SEGMENTACION DE LA IMAGEN //
// ------------------------- //

void dfs(int**, int, int, int, int, int);
void connected(int**, int, int, int, int);
int flood_fill(int**, int, int);
vector<pair<Mat, Rect> > segment(Mat&);

int dx[] = {0,  0, 0, -1, 1, -1,  1, -1, 1};
int dy[] = {0, -1, 1,  0, 0, -1, -1,  1, 1};

  /* Etiqueta toda el area conexa a (r,c) con valor tag */
void dfs(int** data, int tag, int r, int c, int rows, int cols) {

  stack<Point> S;
  S.push(Point(c, r));
  data[r][c] = tag;
  
  while (!S.empty()) {
    Point u = S.top();
    S.pop();
    for (int i = 1; i < 9; i++) {
      Point v(u.x+dx[i], u.y+dy[i]);
      if (v.y <= 0 || v.y >= rows-1 || v.x <= 0 || v.x >= cols-1) {
        continue;
      }
      if (data[v.y][v.x] != INT_MAX || !data[v.y][v.x]) {
        continue;
      }
      data[v.y][v.x] = tag;
      S.push(v);
    }
  }
}

  /* Realiza el etiquetado de toda la imagen */
int flood_fill(int **data, int r, int c) {

  int tag = BKG;
  for (int i = 0; i < r; i++) {
    data[i][0] = data[i][c-1] = BKG;
  }
  for (int i = 0; i < c; i++) {
    data[0][i] = data[r-1][i] = BKG;
  }
  tag++;
  for (int i = 1; i < r-1; i++) {
    for (int j = 1; j < c-1; j++) {
      if (data[i][j] == INT_MAX) {
        dfs(data, tag++, i, j, r, c);
      }
    }
  }
  return tag-1;
}

char vis[1024][1024];

void connected(int **data, int r, int c, int rows, int cols,
               vector<vector<int> > &graph) {
               
  set<int> component;
  stack<Point> S;
  S.push(Point(c, r));
  vis[r][c] = 1;
  
  while (!S.empty()) {
    Point u = S.top();
    S.pop();
    for (int i = 1; i < 9; i++) {
      Point v(u.x+dx[i], u.y+dy[i]);
      if (v.y <= 0 || v.y >= rows-1 || v.x <= 0 || v.x >= cols-1 ||
          vis[v.y][v.x]) {
        continue;
      }
      int t = data[v.y][v.x];
      vis[v.y][v.x] = 1;
      if (t && t-BKG) {
        component.insert(t);
      } else {
        S.push(v);
      }
    }
  }
  vector<int> regs;
  for (__typeof(component.begin()) it = component.begin();
       it != component.end(); it++) {
    regs.push_back(*it);
  }
  int sz = regs.size();
  for (int i = 0; i < sz; i++) {
    int u = regs[i];
    for (int j = i+1; j < sz; j++) {
      int v = regs[j];
      graph[u].push_back(v);
      graph[v].push_back(u);
    }
  }
}

  /* Retorna todos las regiones conexas. */
vector<pair<Mat,Rect> > segment(Mat& src, vector<vector<int> > &graph) {
    // ---------------------
    // Crear matrix temporal
    // ---------------------
  int cols = src.cols;
  int rows = src.rows;
  int **data = new int*[rows];
  
  for (int i = 0; i < rows; i++) {
    data[i] = new int[cols];
    uchar *p = src.ptr(i);
    for (int j = 0; j < cols; j++) {
      data[i][j] = p[j] ? INT_MAX : 0;
    }
  }
    // ------------------------------
    // Etiquetar elementos en la foto
    // ------------------------------
  int tags = flood_fill(data, rows, cols);
  if (!tags) {
    tags++;
  }
    // -------------------------------
    // bounding box de cada etiqueta
    // -------------------------------
  vector<Rect> seg_bb(tags+1);
  for (int i = 1; i <= tags; i++) {
    Rect &r = seg_bb[i];
    r.x = INT_MAX;
    r.y = INT_MAX;
  }
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      int v = data[i][j];
      Rect &r = seg_bb[v];
      r.y = min(r.y, i);
      r.height = max(r.height, i);
      r.x = min(r.x, j);
      r.width = max(r.width, j);
    }
  }
    // --------------------------------------
    // Obtener todas las imagenes segmentadas
    // --------------------------------------
  vector<pair<Mat, Rect> > segs(tags+1);
  
  for (int i = 1; i <= tags; i++) {
    Rect &r = seg_bb[i];
    Mat s = Mat::zeros(r.height-r.y+3, r.width-r.x+3, CV_8UC1);
    int s_cols = s.cols, s_rows = s.rows;
    for (int j = 0; j < s_rows-2; j++) {
      uchar *ps = s.ptr(j+1);
      for (int k = 0; k < s_cols-2; k++) {
        if (data[j+r.y][k+r.x] == i) {
          ps[k+1] = 255;
        }
      }
    }
    segs[i] = make_pair(s, r);
  }
    // --------------------------------
    // Calcular las componentes conexas
    // --------------------------------
  graph.resize(tags+1, vector<int>());
  memset(vis, 0, sizeof(vis));
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      if (!data[i][j]) {
        connected(data, i, j, rows, cols, graph);
      }
    }
  }
    // ------------------------
    // Liberar memoria dinamica
    // ------------------------
  for (int i = 0; i < rows; i++) {
    free(data[i]);
  }
  free(data);
  
  return segs;
}

  /* Calcula los momentos de Hu para la imagen binaria de entrada */
void characterize(Mat &src, double vec[7]) {
  
  Moments mom = moments(src, true);
  HuMoments(mom, vec);
}

int main() {
  for (int vv = 0; vv < VLEN; vv++) {
    string root = "./data/poker/"+V[vv]+"/";
    
    freopen((root+"in").c_str(), "r", stdin);
    freopen((root+"out").c_str(), "w", stdout);
    
    int n;
    cin >> n;
    cout << n << endl;
    
    for (int i = 1; i <= n; i++) {
      stringstream ss;
      ss << i;
      string num = ss.str();
      
      Mat src = imread(root+num+".jpg");
      Mat dst = process(src);
        
      vector<vector<int> > graph;
      vector<pair<Mat, Rect> > s = segment(dst, graph);
      
      for (int j = 2; j < s.size(); j++) {
        stringstream sss;
        sss << j;
        string nseg = sss.str();
        
        Mat m = s[j].first;
        if (m.rows < 20 || m.cols < 20) {
          continue;
        }
        string tt = num+"-"+nseg;
        
        //namedWindow(tt.c_str(), CV_WINDOW_AUTOSIZE);
        //imshow(tt.c_str(), m);
        
        //int key = waitKey(0);
        //if (key == 'x') {
        //  continue;
        // }
        //cout << key << " ";
        double hu[7];
        characterize(m, hu);
        cout << hu[0];
        for (int i = 1; i < 6; i++) {
          cout << " " << hu[i];
        }
        cout << endl;
      }
    }
  }
  return 0;
}
