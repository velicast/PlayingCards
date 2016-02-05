#include "segment.h"

#define MAXR 256
#define MAXD 1024
#define BKG 1
#define CARD 0

int dx[] = {0,  0, 0, -1, 1, -1,  1, -1, 1};
int dy[] = {0, -1, 1,  0, 0, -1, -1,  1, 1};

int data[MAXD][MAXD];
int tmp_data[MAXD][MAXD];
vector<vector<int> > graph;

/* Etiqueta toda el area conexa a (r,c) con valor tag */
void dfs(int tag, int r, int c, int rows, int cols) {

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
int flood_fill(int r, int c) {

  int tag = BKG;
  for (int i = 0; i < r; i++) {
    data[i][0] = data[i][c-1] = BKG;
  }
  for (int i = 0; i < c; i++) {
    data[0][i] = data[r-1][i] = BKG;
  }
  for (int i = 1; i < r-1; i++) {
    for (int j = 1; j < c-1; j++) {
      if (data[i][j] == INT_MAX) {
        dfs(tag++, i, j, r, c);
      }
    }
  }
  return tag-1;
}

char vis[MAXD][MAXD];

void connected(int r, int c, int rows, int cols,
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
      if (t == BKG) {
        continue;
      }
      vis[v.y][v.x] = 1;
      if (!t) {
        S.push(v);
      } else {
        component.insert(t);
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
    vector<int> &gu = graph[u];
    for (int j = i+1; j < sz; j++) {
      int v = regs[j];
      gu.push_back(v);
      graph[v].push_back(u);
    }
  }
}

int get_d(int cols, int i, int &j, int &v) {
  
  v = -1;
  int d = 0;
  while (j < cols && !data[i][j]) {
    ++d;
    ++j;
  }
  if (j < cols) {
    v = data[i][j];
  }
  while (j < cols && data[i][j]-BKG) {
    ++j;
  }
  return d;
}

void border_dist(int rows, int cols, int dist[MAXR]) {
  
  for (int i = 0; i < MAXR; i++) {
    dist[i] = INT_MAX;
  }
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      if (!data[i][j]) {
        int v;
        int d = get_d(cols, i, j, v);
        if (v != -1) {
          dist[v] = min(dist[v], d);
        }
      }
    }
  }
}

pair<Rect, Rect> get_sv(vector<int> &comp,
                        int dist[MAXR],
                        vector<Rect>& rects,
                        pair<int, int>& ids) {
                        
  vector<pair<int, int> > p;
  int sz = comp.size();
  for (int i = 0; i < sz; i++) {
    p.push_back(make_pair(dist[comp[i]], comp[i]));
  }
  sort(p.begin(), p.end());
  
  int p0s = p[0].second;
  int p1s = p[1].second;
  pair<Rect, Rect> res = make_pair(rects[p0s], rects[p1s]);
  ids.first  = p0s;
  ids.second = p1s;
  
  if (res.first.y == res.second.y) {
    if (res.second.x < res.first.x) {
      swap(res.first, res.second);
      swap(ids.first, ids.second);
    }
  }
  else if (res.second.y < res.first.y) {
    swap(res.first, res.second);
    swap(ids.first, ids.second);
  }
  return res;
}

Mat get_im(Rect &r, int i) {

  Mat s = Mat::zeros(r.height-r.y+3, r.width-r.x+3, CV_8UC1);
  int s_cols = s.cols-2, s_rows = s.rows-2;
  for (int j = 0; j < s_rows; j++) {
    uchar *ps = s.ptr(j+1);
    for (int k = 0; k < s_cols; k++) {
      if (data[j+r.y][k+r.x] == i) {
        ps[k+1] = 255;
      }
    }
  }
  return s;
}

int vis_comp[MAXR];

void dfs_comp(int u, vector<int> &c) {

  c.push_back(u);
  vis_comp[u] = 1;
  int sz = graph[u].size();
  vector<int> &gu = graph[u];
  for (int i = 0; i < sz; i++) {
    int v = gu[i];
    if (!vis_comp[v]) {
      dfs_comp(v, c);
    }
  }
}

  /* Retorna todos las cartas. */
vector<card_t> segment(Mat& src, Mat &rgb) {

  graph.clear();
  memset(data, 0, sizeof(data));
  memset(vis, 0, sizeof(vis));
  memset(vis_comp, 0, sizeof(vis_comp));

    // ---------------------
    // Crear matrix temporal
    // ---------------------    
  int cols = src.cols;
  int rows = src.rows;
  for (int i = 0; i < rows; i++) {
    uchar *p = src.ptr(i);
    for (int j = 0; j < cols; j++) {
      data[i][j] = p[j] ? INT_MAX : 0;
    }
  }
    // ------------------------------
    // Etiquetar elementos en la foto
    // ------------------------------
  int tags = flood_fill(rows, cols);
  
  if (tags >= MAXR || !tags) {
    return vector<card_t>();
  }
    // -----------------------------
    // bounding box de cada etiqueta
    // -----------------------------
  vector<Rect> rects(tags+1);
  for (int i = 1; i <= tags; i++) {
    Rect &r = rects[i];
    r.x = INT_MAX;
    r.y = INT_MAX;
  }
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      int v = data[i][j];
      Rect &r = rects[v];
      r.y = min(r.y, i);
      r.height = max(r.height, i);
      r.x = min(r.x, j);
      r.width = max(r.width, j);
    }
  }
    // --------------------------------
    // Calcular las componentes conexas
    // --------------------------------
  graph.resize(tags+1, vector<int>());
  memset(vis, 0, sizeof(vis));
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      if (!data[i][j] && !vis[i][j]) {
        connected(i, j, rows, cols, graph);
      }
    }
  }
  
  int dist[MAXR];
  border_dist(rows, cols, dist);
  
    // -------------------------------------------
    // Obtener valores representativos de la carta
    // -------------------------------------------    
  vector<card_t> res;
  int sz = rects.size();
  for (int i = 2; i < sz; i++) {
    if (vis_comp[i]) {
      continue;
    }
    vector<int> c;
    dfs_comp(i, c);
    if (c.size() <= 1) {
      continue;
    }
    pair<int, int> ids;
    pair<Rect, Rect> p = get_sv(c, dist, rects, ids);
    Rect r0 = p.first;
    Rect r1 = p.second;
    Mat m0 = get_im(r0, ids.first);
    Mat m1 = get_im(r1, ids.second);
    
    card_t card(r0, r1, m0, m1, ids.first, ids.second);
    res.push_back(card);
  }
  return res;
}

Scalar get_avg_color(card_t& card, Mat &rgb) {

  Rect &rect = card.rs;
  int mask = card.ms;
  
  int r = 0, g = 0, b = 0, a = 0;
  int rh = rect.height, rw = rect.width;
  
  for (int i = rect.y; i < rh; i++) {
    for (int j = rect.x; j < rw; j++) {
      if (data[i][j] == mask) {
        a++;
        Vec3b pixel = rgb.at<Vec3b>(i, j);
        r += pixel[2];
        g += pixel[1];
        b += pixel[0];
      }
    }
  }
  if (!a) {
    return Scalar(0, 0, 0);
  }
  return Scalar(b/a, g/a, r/a);
}

void check9_floodfill(int r, int c, Rect &rect, int mask) {
  
  int left   = rect.x;
  int top    = rect.y;
  int right  = rect.width;
  int bottom = rect.height;
  
  stack<Point> S;
  S.push(Point(c, r));
  tmp_data[r][c] = BKG;
  
  while (!S.empty()) {
    Point u = S.top();
    S.pop();
    for (int i = 1; i < 9; i++) {
      Point v(u.x+dx[i], u.y+dy[i]);
      if (v.y < top || v.y > bottom || v.x < left || v.x > right) {
        continue;
      }
      if (tmp_data[v.y][v.x] == mask || tmp_data[v.y][v.x] == BKG) {
        continue;
      }
      tmp_data[v.y][v.x] = BKG;
      S.push(v);
    }
  }
}

int check9(card_t& card) {
  
  Rect &rect = card.rv;
  int mask = card.mv;
    
  int left   = rect.x;
  int top    = rect.y;
  int right  = rect.width;
  int bottom = rect.height;
  
  for (int i = top; i <= bottom; i++) {
    for (int j = left; j <= right; j++) {
      tmp_data[i][j] = data[i][j];
    }
  }
  for (int i = top; i <= bottom; i++) {
    if (tmp_data[i][left]-mask && tmp_data[i][left]-BKG) {
      check9_floodfill(i, left, rect, mask);
    }
    if (tmp_data[i][right]-mask && tmp_data[i][right]-BKG) {
      check9_floodfill(i, right, rect, mask);
    }
  }
  for (int i = left; i <= right; i++) {
    if (tmp_data[top][i]-mask && tmp_data[top][i]-BKG) {
      check9_floodfill(top, i, rect, mask);
    }
    if (tmp_data[bottom][i]-mask && tmp_data[bottom][i]-BKG) {
      check9_floodfill(bottom, i, rect, mask);
    }
  }
  for (int i = top; i <= bottom; i++) {
    for (int j = left; j <= right; j++) {
      if (!tmp_data[i][j]) {
        return i-top+1 <= 0.4*(bottom-top+1);
      }
    }
  }
  return 0;
}

