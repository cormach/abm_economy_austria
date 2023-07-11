#include <string.h>
#include "rt_nonfinite.h"
#include "simulate_abm.h"
#include "forecast_var_sims.h"
#include "sum.h"
#include "rfvar3.h"

void forecast_var_sims(const double rate0_data[], const int rate0_size[1],
  double Y[12])
{
  int tmp_size[1];
  int loop_ub;
  int i3;
  double tmp_data[69];
  double var_By;
  double var_Bx_data[1];
  int var_Bx_size[2];
  double expl_temp_data[80];
  int expl_temp_size[1];
  double expl_temp[4];
  double b_expl_temp;
  double b_Y[13];
  int it;
  int b_size[1];
  double b_data[12];
  emxArray_real_T b_b_data;
  tmp_size[0] = rate0_size[0];
  loop_ub = rate0_size[0];
  for (i3 = 0; i3 < loop_ub; i3++) {
    tmp_data[i3] = 1.0;
  }

  rfvar3(rate0_data, rate0_size, tmp_data, tmp_size, &var_By, var_Bx_data,
         var_Bx_size, expl_temp_data, expl_temp_size, expl_temp, &b_expl_temp);
  memset(&b_Y[0], 0, 13U * sizeof(double));
  b_Y[0] = rate0_data[rate0_size[0] - 1];
  for (it = 0; it < 12; it++) {
    b_size[0] = 1;
    memcpy(&b_data[0], &b_Y[it], sizeof(double));
    loop_ub = b_size[0];
    for (i3 = 0; i3 < loop_ub; i3++) {
      b_data[i3] *= var_By;
    }

    b_b_data.data = &b_data[0];
    b_b_data.size = &b_size[0];
    b_b_data.allocatedSize = 12;
    b_b_data.numDimensions = 1;
    b_b_data.canFreeData = false;
    b_Y[1 + it] = d_sum(&b_b_data) + var_Bx_data[0];
  }

  memcpy(&Y[0], &b_Y[1], 12U * sizeof(double));
}
