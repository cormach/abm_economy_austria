#include <math.h>
#include "rt_nonfinite.h"
#include "simulate_abm.h"
#include "simulate_var_sims.h"
#include "randn.h"
#include "cov.h"
#include "rfvar3.h"

double simulate_var_sims(const double rate0_data[], const int rate0_size[1])
{
  int tmp_size[1];
  int loop_ub;
  int i25;
  double tmp_data[81];
  double var_By;
  double var_Bx_data[1];
  int var_Bx_size[2];
  double var_u_data[80];
  int var_u_size[1];
  double expl_temp[4];
  double b_expl_temp;
  double r;
  tmp_size[0] = rate0_size[0];
  loop_ub = rate0_size[0];
  for (i25 = 0; i25 < loop_ub; i25++) {
    tmp_data[i25] = 1.0;
  }

  rfvar3(rate0_data, rate0_size, tmp_data, tmp_size, &var_By, var_Bx_data,
         var_Bx_size, var_u_data, var_u_size, expl_temp, &b_expl_temp);
  b_expl_temp = sqrt(cov(var_u_data, var_u_size));
  r = b_randn();
  r *= b_expl_temp;
  if (b_expl_temp < 0.0) {
    r = rtNaN;
  }

  return (var_By * rate0_data[rate0_size[0] - 1] + var_Bx_data[0]) + r;
}
