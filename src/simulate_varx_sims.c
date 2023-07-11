#include "rt_nonfinite.h"
#include "simulate_abm.h"
#include "simulate_varx_sims.h"
#include "sims_simulate.h"
#include "sqrt.h"
#include "cov.h"
#include "rfvar3.h"

double simulate_varx_sims(const double rate0_data[], const int rate0_size[1],
  const double exo_data[], const int exo_size[2])
{
  int loop_ub;
  int i21;
  cell_wrap_1 reshapes[2];
  int i22;
  double b_exo_data[243];
  int reshapes_size[2];
  double reshapes_data[324];
  double var_By;
  double var_Bx_data[4];
  int var_Bx_size[2];
  double var_u_data[80];
  int var_u_size[1];
  static double expl_temp_data[4624];
  int expl_temp_size[2];
  double expl_temp;
  signed char input_sizes_idx_1;
  int exo_size_idx_0_tmp;
  int tmp_size[2];
  int tmp_data_tmp;
  double tmp_data[8];
  double b_Y[2];
  loop_ub = rate0_size[0];
  for (i21 = 0; i21 < loop_ub; i21++) {
    reshapes[0].f1.data[i21] = 1.0;
  }

  loop_ub = rate0_size[0];
  for (i21 = 0; i21 < 3; i21++) {
    for (i22 = 0; i22 < loop_ub; i22++) {
      b_exo_data[i22 + loop_ub * i21] = exo_data[i22 + exo_size[0] * i21];
    }
  }

  reshapes[1].f1.size[0] = rate0_size[0];
  loop_ub = rate0_size[0] * 3;
  for (i21 = 0; i21 < loop_ub; i21++) {
    reshapes[1].f1.data[i21] = b_exo_data[i21];
  }

  reshapes_size[0] = rate0_size[0];
  reshapes_size[1] = 4;
  for (i21 = 0; i21 < 1; i21++) {
    loop_ub = rate0_size[0];
    for (i22 = 0; i22 < loop_ub; i22++) {
      reshapes_data[i22] = reshapes[0].f1.data[i22];
    }
  }

  for (i21 = 0; i21 < 3; i21++) {
    loop_ub = reshapes[1].f1.size[0];
    for (i22 = 0; i22 < loop_ub; i22++) {
      reshapes_data[i22 + reshapes_size[0] * (i21 + 1)] = reshapes[1]
        .f1.data[i22 + reshapes[1].f1.size[0] * i21];
    }
  }

  b_rfvar3(rate0_data, rate0_size, reshapes_data, reshapes_size, &var_By,
           var_Bx_data, var_Bx_size, var_u_data, var_u_size, expl_temp_data,
           expl_temp_size, &expl_temp);
  if (rate0_size[0] > rate0_size[0] + 1) {
    i21 = -1;
    i22 = -1;
  } else {
    i21 = rate0_size[0] - 2;
    i22 = rate0_size[0];
  }

  if (i22 - i21 != 0) {
    input_sizes_idx_1 = 3;
  } else {
    input_sizes_idx_1 = 0;
  }

  expl_temp = cov(var_u_data, var_u_size);
  b_sqrt(&expl_temp);
  exo_size_idx_0_tmp = i22 - i21;
  for (i22 = 0; i22 < 3; i22++) {
    for (tmp_data_tmp = 0; tmp_data_tmp < exo_size_idx_0_tmp; tmp_data_tmp++) {
      b_exo_data[tmp_data_tmp + exo_size_idx_0_tmp * i22] = exo_data[((i21 +
        tmp_data_tmp) + exo_size[0] * i22) + 1];
    }
  }

  tmp_size[0] = 2;
  tmp_size[1] = 1 + input_sizes_idx_1;
  tmp_data[0] = 1.0;
  tmp_data[1] = 1.0;
  loop_ub = input_sizes_idx_1;
  for (i21 = 0; i21 < loop_ub; i21++) {
    exo_size_idx_0_tmp = i21 << 1;
    tmp_data_tmp = (i21 + 1) << 1;
    tmp_data[tmp_data_tmp] = b_exo_data[exo_size_idx_0_tmp];
    tmp_data[1 + tmp_data_tmp] = b_exo_data[1 + exo_size_idx_0_tmp];
  }

  sims_simulate(rate0_data[rate0_size[0] - 1], var_By, var_Bx_data, var_Bx_size,
                tmp_data, tmp_size, expl_temp, b_Y);
  return b_Y[1];
}
