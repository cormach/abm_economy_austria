#include <math.h>
#include "rt_nonfinite.h"
#include "simulate_abm.h"
#include "exp.h"

void b_exp(emxArray_real_T *x)
{
  int nx;
  int k;
  nx = x->size[1];
  for (k = 0; k < nx; k++) {
    x->data[k] = exp(x->data[k]);
  }
}
