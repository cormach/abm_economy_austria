#include "rt_nonfinite.h"
#include "simulate_abm.h"
#include "randpl.h"
#include "simulate_abm_emxutil.h"
#include "rand.h"

void randpl(double n, emxArray_real_T *x)
{
  emxArray_real_T *a;
  int nx;
  int loop_ub;
  unsigned int a_idx_0;
  emxInit_real_T(&a, 1);
  b_rand(n, a);
  nx = a->size[0];
  emxEnsureCapacity_real_T(a, nx);
  loop_ub = a->size[0];
  for (nx = 0; nx < loop_ub; nx++) {
    a->data[nx] = 1.0 - a->data[nx];
  }

  a_idx_0 = (unsigned int)a->size[0];
  nx = x->size[0];
  x->size[0] = (int)a_idx_0;
  emxEnsureCapacity_real_T(x, nx);
  a_idx_0 = (unsigned int)a->size[0];
  nx = (int)a_idx_0;
  for (loop_ub = 0; loop_ub < nx; loop_ub++) {
    x->data[loop_ub] = 1.0 / a->data[loop_ub];
  }

  emxFree_real_T(&a);
}
