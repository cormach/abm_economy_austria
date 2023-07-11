#include "rt_nonfinite.h"
#include "simulate_abm.h"
#include "xaxpy.h"

void b_xaxpy(int n, double a, const double x_data[], int ix0, double y_data[],
             int iy0)
{
  int ix;
  int iy;
  int i29;
  int k;
  if ((n < 1) || (a == 0.0)) {
  } else {
    ix = ix0 - 1;
    iy = iy0 - 1;
    i29 = n - 1;
    for (k = 0; k <= i29; k++) {
      y_data[iy] += a * x_data[ix];
      ix++;
      iy++;
    }
  }
}

void xaxpy(int n, double a, int ix0, double y_data[], int iy0)
{
  int ix;
  int iy;
  int i27;
  int k;
  if ((n < 1) || (a == 0.0)) {
  } else {
    ix = ix0 - 1;
    iy = iy0 - 1;
    i27 = n - 1;
    for (k = 0; k <= i27; k++) {
      y_data[iy] += a * y_data[ix];
      ix++;
      iy++;
    }
  }
}
