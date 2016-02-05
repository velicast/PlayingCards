#include <bits/stdc++.h>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

  // maximo numero de clusters
#define MAXZ 32

  // Retorna la distancia euclideana entre
  // los vectores n-dimensionales a y b
double euclidean(double *a, double *b, int n) {

  double d = 0;
  for (int i = 0; i < n; i++) {
    d += (a[i]-b[i])*(a[i]-b[i]);
  }
  return d;
}
  // descarta ith-elemento
void shift(int *s, int &sz, int i) {
  
  for (int j = i+1; j < sz; j++) {
    s[j-1] = s[j];
  }
  sz--;
}
  // Algoritmo de clustering Max-Min
  // v matriz de vectores.
  // n numeros de vectores en v (un vector por fila)
  // m dimension de cada vector
  // f factor de distancia promedio
  // c vector de salida donde se asigna el cluster a cada vector
  //   de entrada (se asigna el cluster como un valor numerico)
  // Retorna el numero de clusters creados
int max_min(double **v, int n, int m, double f, int *c) {
    // numero de clusters creados
  int k = 0;
      // distancia del i-th centroide al u'ltimo agregado
  double zd[MAXZ] = {};
    // centroide del cluster
  int z[MAXZ];
    // conjunto de puntos no asignados
  int sz = n;
  int *s = new int[n];
  for (int i = 0; i < n; i++) {
    s[i] = i;
  }
  
    // variable auxiliares
  double maxi = 0, mini;
  int p, x;
  
    // primer cluster
  int x0 = rand()%n;
  shift(s, sz, x0);
  c[x0] = k;
  z[k++] = x0;
  
    // segundo cluster
  for (int i = 0; i < sz; i++) {
    double d = euclidean(v[x0], v[s[i]], m);
    if (maxi < d) {
      maxi = d;
      p = i;
    }
  }
  int x1 = s[p];
  shift(s, sz, p);
  c[x1] = k;
  z[k++] = x1;
  
  zd[0] = maxi;
  
  double zsum = 0;
  do {
      // distancia promedio entre clusters
    for (int i = 0; i < k-1; i++) {
      zsum += zd[i];
    }
    double avg = zsum/(k*(k-1)/2.);
    
    maxi = 0;
    for (int i = 0; i < sz; i++) {
      mini = 1e14;
      for (int j = 0; j < k; j++) {
        mini = min(euclidean(v[s[i]], v[z[j]], m), mini);
      }
      if (mini > maxi) {
        maxi = mini;
        p = i;
      }
    }
    
    if (maxi > f*avg) {
      x = s[p];
      shift(s, sz, p);
      c[x] = k;
      z[k] = x;
        // agregar distancias al nuevo cluster
      for (int i = 0; i < k; i++) {
        zd[i] = euclidean(v[z[i]], v[x], m);
      }
      k++;
    } else {
      break;
    }
  } while (1);
  
  for (int i = 0; i < sz; i++) {
    mini = 1e14;
    x = s[i];
    for (int j = 0; j < k; j++) {
      double d = euclidean(v[x], v[z[j]], m);
      if (mini > d) {
        mini = d;
        c[x] = j;
      }
    }
  }
  return k;
}

int main() {
  srand(unsigned(time(0)));
  
  int n = 9;
  
  double **v = new double*[n];
  for (int i = 0; i < n; i++) {
    v[i] = new double[2];
  }
    // cada vector representa un punto (x, y) -> (v[i][0], v[i][1])
  v[0][0] = 0;
  v[0][1] = 0;
  
  v[1][0] = 20;
  v[1][1] = 0;
  
  v[2][0] = 20;
  v[2][1] = 20;
  
  v[3][0] = 0;
  v[3][1] = 20;

  v[4][0] = 1;
  v[4][1] = 1;
  
  v[5][0] = 10;
  v[5][1] = 10;
  
  v[6][0] = 3;
  v[6][1] = 17;
  
  v[7][0] = 15;
  v[7][1] = 14;
  
  v[8][0] = 18;
  v[8][1] = 2;  
  
  int *c = new int[n];
  
  cout << "Clusters: " << max_min(v, n, 2, 0.4, c) << endl;
  
  for (int i = 0; i < n; i++) {
    cout << v[i][0] << "," << v[i][1] << " -> " << c[i] << endl;
  }
  return 0;
}
