#include <string.h>
#include "rt_nonfinite.h"
#include "simulate_abm.h"
#include "forecast_varx_sims.h"
#include "fcast.h"
#include "rfvar3.h"

void forecast_varx_sims(const double rate0_data[], const int rate0_size[1],
  const double exo_data[], const int exo_size[2], double Y[12])
{
  int loop_ub;
  int i5;
  cell_wrap_1 reshapes[2];
  double b_exo_data[138];
  int reshapes_size[2];
  int i6;
  double reshapes_data[324];
  double var_By;
  double var_Bx_data[4];
  int var_Bx_size[2];
  double expl_temp_data[80];
  int expl_temp_size[1];
  static double b_expl_temp_data[4624];
  int b_expl_temp_size[2];
  double expl_temp;
  int rate0_idx_1;
  double c_exo_data[162];
  int i7;
  double reshapes_f2_data[26];
  int tmp_size[2];
  double tmp_data[39];
  double dv516[13];
  loop_ub = rate0_size[0];
  for (i5 = 0; i5 < loop_ub; i5++) {
    reshapes[0].f1.data[i5] = 1.0;
  }

  loop_ub = rate0_size[0];
  for (i5 = 0; i5 < loop_ub; i5++) {
    b_exo_data[i5] = exo_data[i5];
  }

  for (i5 = 0; i5 < loop_ub; i5++) {
    b_exo_data[i5 + loop_ub] = exo_data[i5 + exo_size[0]];
  }

  reshapes[1].f1.size[0] = rate0_size[0];
  loop_ub = rate0_size[0] << 1;
  for (i5 = 0; i5 < loop_ub; i5++) {
    reshapes[1].f1.data[i5] = b_exo_data[i5];
  }

  reshapes_size[0] = rate0_size[0];
  reshapes_size[1] = 3;
  for (i5 = 0; i5 < 1; i5++) {
    loop_ub = rate0_size[0];
    for (i6 = 0; i6 < loop_ub; i6++) {
      reshapes_data[i6] = reshapes[0].f1.data[i6];
    }
  }

  for (i5 = 0; i5 < 2; i5++) {
    loop_ub = reshapes[1].f1.size[0];
    for (i6 = 0; i6 < loop_ub; i6++) {
      reshapes_data[i6 + reshapes_size[0] * (i5 + 1)] = reshapes[1].f1.data[i6 +
        reshapes[1].f1.size[0] * i5];
    }
  }

  b_rfvar3(rate0_data, rate0_size, reshapes_data, reshapes_size, &var_By,
           var_Bx_data, var_Bx_size, expl_temp_data, expl_temp_size,
           b_expl_temp_data, b_expl_temp_size, &expl_temp);
  if (rate0_size[0] > rate0_size[0] + 12) {
    i5 = -1;
    i6 = -1;
  } else {
    i5 = rate0_size[0] - 2;
    i6 = rate0_size[0] + 11;
  }

  loop_ub = i6 - i5;
  for (rate0_idx_1 = 0; rate0_idx_1 < loop_ub; rate0_idx_1++) {
    c_exo_data[rate0_idx_1] = exo_data[(i5 + rate0_idx_1) + 1];
  }

  for (rate0_idx_1 = 0; rate0_idx_1 < loop_ub; rate0_idx_1++) {
    c_exo_data[rate0_idx_1 + loop_ub] = exo_data[((i5 + rate0_idx_1) + exo_size
      [0]) + 1];
  }

  if (i6 - i5 != 0) {
    rate0_idx_1 = 2;
    i7 = 2;
  } else {
    rate0_idx_1 = 0;
    i7 = 0;
  }

  loop_ub = 13 * i7;
  if (0 <= loop_ub - 1) {
    memcpy(&reshapes_f2_data[0], &c_exo_data[0], (unsigned int)(loop_ub * (int)
            sizeof(double)));
  }

  tmp_size[0] = 13;
  tmp_size[1] = 1 + rate0_idx_1;
  for (i5 = 0; i5 < 13; i5++) {
    tmp_data[i5] = 1.0;
  }

  for (i5 = 0; i5 < rate0_idx_1; i5++) {
    memcpy(&tmp_data[i5 * 13 + 13], &reshapes_f2_data[i5 * 13], 13U * sizeof
           (double));
  }

  fcast(rate0_data[rate0_size[0] - 1], var_By, var_Bx_data, var_Bx_size,
        tmp_data, tmp_size, dv516);
  memcpy(&Y[0], &dv516[1], 12U * sizeof(double));
}
