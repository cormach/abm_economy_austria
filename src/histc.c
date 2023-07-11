#include "rt_nonfinite.h"
#include "simulate_abm.h"
#include "histc.h"
#include "simulate_abm_emxutil.h"

void histc(double X, const emxArray_real_T *edges, emxArray_real_T *N, double
           *BIN)
{
  unsigned int outsize_idx_1;
  int high_i;
  int low_i;
  int low_ip1;
  int mid_i;
  outsize_idx_1 = (unsigned int)edges->size[1];
  high_i = N->size[0] * N->size[1];
  N->size[0] = 1;
  N->size[1] = (int)outsize_idx_1;
  emxEnsureCapacity_real_T(N, high_i);
  low_i = (int)outsize_idx_1;
  for (high_i = 0; high_i < low_i; high_i++) {
    N->data[high_i] = 0.0;
  }

  low_i = 0;
  if (!rtIsNaN(X)) {
    if ((X >= edges->data[0]) && (X < edges->data[edges->size[1] - 1])) {
      high_i = edges->size[1];
      low_i = 1;
      low_ip1 = 2;
      while (high_i > low_ip1) {
        mid_i = (low_i >> 1) + (high_i >> 1);
        if (((low_i & 1) == 1) && ((high_i & 1) == 1)) {
          mid_i++;
        }

        if (X >= edges->data[mid_i - 1]) {
          low_i = mid_i;
          low_ip1 = mid_i + 1;
        } else {
          high_i = mid_i;
        }
      }
    }

    if (X == edges->data[edges->size[1] - 1]) {
      low_i = edges->size[1];
    }
  }

  if (low_i > 0) {
    N->data[low_i - 1] = 1.0;
  }

  *BIN = low_i;
}
