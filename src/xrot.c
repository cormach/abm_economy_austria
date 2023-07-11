#include "rt_nonfinite.h"
#include "simulate_abm.h"
#include "xrot.h"

void b_xrot(int n, double x_data[], int ix0, int iy0, double c, double s)
{
  int ix;
  int iy;
  int k;
  double temp;
  if (n >= 1) {
    ix = ix0 - 1;
    iy = iy0 - 1;
    for (k = 0; k < n; k++) {
      temp = c * x_data[ix] + s * x_data[iy];
      x_data[iy] = c * x_data[iy] - s * x_data[ix];
      x_data[ix] = temp;
      iy++;
      ix++;
    }
  }
}

void xrot(double x[4], int iy0, double c, double s)
{
  int iy;
  double temp;
  iy = iy0 - 1;
  temp = c * x[0] + s * x[iy];
  x[iy] = c * x[iy] - s * x[0];
  x[0] = temp;
  iy++;
  temp = c * x[1] + s * x[iy];
  x[iy] = c * x[iy] - s * x[1];
  x[1] = temp;
}
