#include <string.h>
#include "rt_nonfinite.h"
#include "simulate_abm.h"
#include "svd.h"
#include "svd1.h"
#include "anyNonFinite.h"

void c_svd(const double A_data[], const int A_size[2], double U_data[], int
           U_size[2], double S_data[], int S_size[2], double V_data[], int
           V_size[2])
{
  signed char unnamed_idx_0;
  double s_data[5];
  int s_size[1];
  signed char unnamed_idx_1;
  int tmp_size[2];
  int loop_ub;
  double tmp_data[400];
  double U1_data[400];
  int U1_size[2];
  double V1_data[340];
  int V1_size[2];
  int i9;
  if (!b_anyNonFinite(A_data, A_size)) {
    d_svd(A_data, A_size, U_data, U_size, s_data, s_size, V_data, V_size);
  } else {
    unnamed_idx_0 = (signed char)A_size[0];
    unnamed_idx_1 = (signed char)A_size[1];
    tmp_size[0] = unnamed_idx_0;
    tmp_size[1] = unnamed_idx_1;
    loop_ub = unnamed_idx_0 * unnamed_idx_1;
    if (0 <= loop_ub - 1) {
      memset(&tmp_data[0], 0, (unsigned int)(loop_ub * (int)sizeof(double)));
    }

    d_svd(tmp_data, tmp_size, U1_data, U1_size, s_data, s_size, V1_data, V1_size);
    U_size[0] = U1_size[0];
    U_size[1] = U1_size[1];
    loop_ub = U1_size[0] * U1_size[1];
    for (i9 = 0; i9 < loop_ub; i9++) {
      U_data[i9] = rtNaN;
    }

    loop_ub = s_size[0];
    for (i9 = 0; i9 < loop_ub; i9++) {
      s_data[i9] = rtNaN;
    }

    V_size[0] = V1_size[0];
    V_size[1] = V1_size[1];
    loop_ub = V1_size[0] * V1_size[1];
    for (i9 = 0; i9 < loop_ub; i9++) {
      V_data[i9] = rtNaN;
    }
  }

  S_size[0] = U_size[1];
  S_size[1] = V_size[1];
  loop_ub = U_size[1] * V_size[1];
  if (0 <= loop_ub - 1) {
    memset(&S_data[0], 0, (unsigned int)(loop_ub * (int)sizeof(double)));
  }

  i9 = s_size[0] - 1;
  for (loop_ub = 0; loop_ub <= i9; loop_ub++) {
    S_data[loop_ub + S_size[0] * loop_ub] = s_data[loop_ub];
  }
}

void svd(const double A_data[], const int A_size[2], double U_data[], int
         U_size[2], double S_data[], int S_size[2], double V[4])
{
  signed char unnamed_idx_0;
  double s_data[2];
  int s_size[1];
  int tmp_size[2];
  int loop_ub;
  double tmp_data[160];
  double U1_data[400];
  int U1_size[2];
  double V1[4];
  int i4;
  if (!anyNonFinite(A_data, A_size)) {
    b_svd(A_data, A_size, U_data, U_size, s_data, s_size, V);
  } else {
    unnamed_idx_0 = (signed char)A_size[0];
    tmp_size[0] = unnamed_idx_0;
    tmp_size[1] = 2;
    loop_ub = unnamed_idx_0 << 1;
    if (0 <= loop_ub - 1) {
      memset(&tmp_data[0], 0, (unsigned int)(loop_ub * (int)sizeof(double)));
    }

    b_svd(tmp_data, tmp_size, U1_data, U1_size, s_data, s_size, V1);
    U_size[0] = U1_size[0];
    U_size[1] = U1_size[1];
    loop_ub = U1_size[0] * U1_size[1];
    for (i4 = 0; i4 < loop_ub; i4++) {
      U_data[i4] = rtNaN;
    }

    loop_ub = s_size[0];
    for (i4 = 0; i4 < loop_ub; i4++) {
      s_data[i4] = rtNaN;
    }

    V[0] = rtNaN;
    V[1] = rtNaN;
    V[2] = rtNaN;
    V[3] = rtNaN;
  }

  S_size[0] = U_size[1];
  S_size[1] = 2;
  loop_ub = U_size[1] << 1;
  if (0 <= loop_ub - 1) {
    memset(&S_data[0], 0, (unsigned int)(loop_ub * (int)sizeof(double)));
  }

  i4 = s_size[0] - 1;
  for (loop_ub = 0; loop_ub <= i4; loop_ub++) {
    S_data[loop_ub + S_size[0] * loop_ub] = s_data[loop_ub];
  }
}
