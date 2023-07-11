#include "rt_nonfinite.h"
#include "simulate_abm.h"
#include "normrnd.h"
#include "simulate_abm_emxutil.h"
#include "randn.h"

void normrnd(double varargin_2, emxArray_real_T *r)
{
  int unnamed_idx_1_tmp;
  double dv518[2];
  int i15;
  unnamed_idx_1_tmp = (int)varargin_2;
  dv518[0] = 1.0;
  dv518[1] = unnamed_idx_1_tmp;
  randn(dv518, r);
  i15 = r->size[0] * r->size[1];
  r->size[0] = 1;
  r->size[1] = unnamed_idx_1_tmp;
  emxEnsureCapacity_real_T(r, i15);
  for (i15 = 0; i15 < unnamed_idx_1_tmp; i15++) {
    r->data[i15] = r->data[i15] * 0.0 + 1.0;
  }
}
