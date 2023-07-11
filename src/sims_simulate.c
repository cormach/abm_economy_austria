#include "rt_nonfinite.h"
#include "simulate_abm.h"
#include "sims_simulate.h"
#include "randn.h"

void sims_simulate(double b_y0, double By, const double Bx_data[], const int
                   Bx_size[2], const double xdata_data[], const int xdata_size[2],
                   double sigma, double yhat[2])
{
  int n;
  int loop_ub;
  int i23;
  int b_loop_ub;
  int i24;
  double b_data[4];
  double tmp_data[1];
  double r;
  yhat[0] = b_y0;
  n = Bx_size[1];
  loop_ub = Bx_size[0];
  for (i23 = 0; i23 < loop_ub; i23++) {
    b_loop_ub = Bx_size[1];
    for (i24 = 0; i24 < b_loop_ub; i24++) {
      b_data[i24 + n * i23] = Bx_data[i23 + Bx_size[0] * i24];
    }
  }

  if ((xdata_size[1] == 1) || (Bx_size[1] == 1)) {
    loop_ub = xdata_size[1];
    b_loop_ub = Bx_size[0];
    for (i23 = 0; i23 < b_loop_ub; i23++) {
      tmp_data[i23] = 0.0;
      for (i24 = 0; i24 < loop_ub; i24++) {
        tmp_data[i23] += xdata_data[1 + (i24 << 1)] * b_data[i24 + n * i23];
      }
    }
  } else {
    i23 = xdata_size[1];
    n = Bx_size[0];
    for (loop_ub = 0; loop_ub < n; loop_ub++) {
      tmp_data[0] = 0.0;
      for (b_loop_ub = 0; b_loop_ub < i23; b_loop_ub++) {
        tmp_data[0] += b_data[b_loop_ub] * xdata_data[1 + (b_loop_ub << 1)];
      }
    }
  }

  r = b_randn();
  r *= sigma;
  if (sigma < 0.0) {
    r = rtNaN;
  }

  yhat[1] = (By * b_y0 + tmp_data[0]) + r;
}
