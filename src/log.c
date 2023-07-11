#include <math.h>
#include "rt_nonfinite.h"
#include "simulate_abm.h"
#include "log.h"

void b_log(double x_data[], int x_size[1])
{
  int nx;
  int k;
  nx = x_size[0];
  for (k = 0; k < nx; k++) {
    x_data[k] = log(x_data[k]);
  }
}
