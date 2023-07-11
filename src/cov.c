#include <string.h>
#include "rt_nonfinite.h"
#include "simulate_abm.h"
#include "cov.h"

double cov(const double x_data[], const int x_size[1])
{
  double xy;
  double b_x_data[80];
  int m;
  double muj;
  int i;
  double temp;
  double d2;
  if (0 <= x_size[0] - 1) {
    memcpy(&b_x_data[0], &x_data[0], (unsigned int)(x_size[0] * (int)sizeof
            (double)));
  }

  m = x_size[0] - 1;
  xy = 0.0;
  if (x_size[0] == 0) {
    xy = rtNaN;
  } else {
    if (x_size[0] >= 2) {
      muj = 0.0;
      for (i = 0; i <= m; i++) {
        muj += x_data[i];
      }

      muj /= (double)x_size[0];
      temp = 0.0;
      for (i = 0; i <= m; i++) {
        d2 = b_x_data[i] - muj;
        b_x_data[i] = d2;
        temp += d2 * d2;
      }

      xy = 1.0 / ((double)x_size[0] - 1.0) * temp;
    }
  }

  return xy;
}
