#include "rt_nonfinite.h"
#include "simulate_abm.h"
#include "xdotc.h"

double xdotc(int n, const double x_data[], int ix0, const double y_data[], int
             iy0)
{
  double d;
  int ix;
  int iy;
  int k;
  d = 0.0;
  if (n >= 1) {
    ix = ix0;
    iy = iy0;
    for (k = 0; k < n; k++) {
      d += x_data[ix - 1] * y_data[iy - 1];
      ix++;
      iy++;
    }
  }

  return d;
}
