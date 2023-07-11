#include <string.h>
#include "rt_nonfinite.h"
#include "simulate_abm.h"
#include "fcast.h"
#include "sum.h"

void fcast(double b_y0, double By, const double Bx_data[], const int Bx_size[2],
           const double xdata_data[], const int xdata_size[2], double yhat[13])
{
  int loop_ub;
  int b_size_idx_0;
  int b_size_idx_1;
  int b_loop_ub;
  int i11;
  int it;
  int c_loop_ub;
  int i12;
  double b_data[4];
  double a_data[4];
  double tmp_data[1];
  int b_size[1];
  double b_b_data[12];
  emxArray_real_T c_b_data;
  memset(&yhat[0], 0, 13U * sizeof(double));
  yhat[0] = b_y0;
  loop_ub = xdata_size[1];
  b_size_idx_0 = Bx_size[1];
  b_size_idx_1 = Bx_size[0];
  b_loop_ub = Bx_size[0];
  for (i11 = 0; i11 < b_loop_ub; i11++) {
    c_loop_ub = Bx_size[1];
    for (i12 = 0; i12 < c_loop_ub; i12++) {
      b_data[i12 + b_size_idx_0 * i11] = Bx_data[i11 + Bx_size[0] * i12];
    }
  }

  for (it = 0; it < 12; it++) {
    for (i11 = 0; i11 < loop_ub; i11++) {
      a_data[i11] = xdata_data[(it + 13 * i11) + 1];
    }

    if ((xdata_size[1] == 1) || (b_size_idx_0 == 1)) {
      b_loop_ub = xdata_size[1];
      for (i11 = 0; i11 < b_size_idx_1; i11++) {
        tmp_data[i11] = 0.0;
        for (i12 = 0; i12 < b_loop_ub; i12++) {
          tmp_data[i11] += xdata_data[(it + 13 * i12) + 1] * b_data[i12 +
            b_size_idx_0 * i11];
        }
      }
    } else {
      i11 = xdata_size[1] - 1;
      for (c_loop_ub = 0; c_loop_ub < b_size_idx_1; c_loop_ub++) {
        tmp_data[0] = 0.0;
        for (b_loop_ub = 0; b_loop_ub <= i11; b_loop_ub++) {
          tmp_data[0] += b_data[b_loop_ub] * a_data[b_loop_ub];
        }
      }
    }

    b_size[0] = 1;
    memcpy(&b_b_data[0], &yhat[it], sizeof(double));
    b_loop_ub = b_size[0];
    for (i11 = 0; i11 < b_loop_ub; i11++) {
      b_b_data[i11] *= By;
    }

    c_b_data.data = &b_b_data[0];
    c_b_data.size = &b_size[0];
    c_b_data.allocatedSize = 12;
    c_b_data.numDimensions = 1;
    c_b_data.canFreeData = false;
    yhat[1 + it] = d_sum(&c_b_data) + tmp_data[0];
  }
}
