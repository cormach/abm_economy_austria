#include "rt_nonfinite.h"
#include "simulate_abm.h"
#include "nullAssignment.h"
#include "simulate_abm_emxutil.h"

void nullAssignment(emxArray_real_T *x, const emxArray_boolean_T *idx)
{
  int nxin;
  int k0;
  int i31;
  int k;
  int nxout;
  nxin = x->size[1];
  k0 = 0;
  i31 = idx->size[1];
  for (k = 0; k < i31; k++) {
    k0 += idx->data[k];
  }

  nxout = x->size[1] - k0;
  k0 = -1;
  for (k = 0; k < nxin; k++) {
    if ((k + 1 > idx->size[1]) || (!idx->data[k])) {
      k0++;
      x->data[k0] = x->data[k];
    }
  }

  if (1 > nxout) {
    x->size[1] = 0;
  } else {
    i31 = x->size[0] * x->size[1];
    x->size[1] = nxout;
    emxEnsureCapacity_real_T(x, i31);
  }
}
