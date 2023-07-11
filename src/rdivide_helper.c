#include "rt_nonfinite.h"
#include "simulate_abm.h"
#include "rdivide_helper.h"
#include "simulate_abm_emxutil.h"

void b_rdivide_helper(const emxArray_real_T *y, emxArray_real_T *z)
{
  int i20;
  int loop_ub;
  i20 = z->size[0] * z->size[1];
  z->size[0] = 1;
  z->size[1] = y->size[1];
  emxEnsureCapacity_real_T(z, i20);
  loop_ub = y->size[0] * y->size[1];
  for (i20 = 0; i20 < loop_ub; i20++) {
    z->data[i20] = 1.0 / y->data[i20];
  }
}

void rdivide_helper(const emxArray_real_T *x, const emxArray_real_T *y,
                    emxArray_real_T *z)
{
  int i19;
  int loop_ub;
  i19 = z->size[0] * z->size[1];
  z->size[0] = 1;
  z->size[1] = x->size[1];
  emxEnsureCapacity_real_T(z, i19);
  loop_ub = x->size[0] * x->size[1];
  for (i19 = 0; i19 < loop_ub; i19++) {
    z->data[i19] = x->data[i19] / y->data[i19];
  }
}
