#include <string.h>
#include "rt_nonfinite.h"
#include "simulate_abm.h"
#include "diag.h"

void b_diag(const double v_data[], const int v_size[1], double d_data[], int
            d_size[2])
{
  int nv;
  int loop_ub;
  nv = v_size[0];
  d_size[0] = (signed char)v_size[0];
  d_size[1] = (signed char)v_size[0];
  loop_ub = (signed char)v_size[0] * (signed char)v_size[0];
  if (0 <= loop_ub - 1) {
    memset(&d_data[0], 0, (unsigned int)(loop_ub * (int)sizeof(double)));
  }

  for (loop_ub = 0; loop_ub < nv; loop_ub++) {
    d_data[loop_ub + d_size[0] * loop_ub] = v_data[loop_ub];
  }
}

void c_diag(const double v_data[], const int v_size[2], double d_data[], int
            d_size[1])
{
  int u0;
  int u1;
  if ((v_size[0] == 1) && (v_size[1] == 1)) {
    d_size[0] = 1;
    d_data[0] = v_data[0];
  } else {
    u0 = v_size[0];
    u1 = v_size[1];
    if (u0 < u1) {
      u1 = u0;
    }

    if (0 < v_size[1]) {
      u0 = u1;
    } else {
      u0 = 0;
    }

    d_size[0] = (signed char)u0;
    u0--;
    for (u1 = 0; u1 <= u0; u1++) {
      d_data[u1] = v_data[u1 + v_size[0] * u1];
    }
  }
}

void diag(const double v_data[], const int v_size[2], double d_data[], int
          d_size[1])
{
  int u0;
  int k;
  u0 = v_size[0];
  if (u0 >= 2) {
    u0 = 2;
  }

  d_size[0] = (signed char)u0;
  u0--;
  for (k = 0; k <= u0; k++) {
    d_data[k] = v_data[k + v_size[0] * k];
  }
}
