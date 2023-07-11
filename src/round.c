#include "rt_nonfinite.h"
#include "simulate_abm.h"
#include "round.h"
#include "simulate_abm_rtwutil.h"

void b_round(emxArray_real_T *x)
{
  int nx;
  int k;
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = rt_roundd_snf(x->data[k]);
  }
}
