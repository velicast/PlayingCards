#include "characterize.h"

  /* Calcula los momentos de Hu para la imagen binaria de entrada */
void characterize(Mat &src, double vec[VEC_LEN]) {
  
  Moments mom = moments(src, true);
  HuMoments(mom, vec);
}
