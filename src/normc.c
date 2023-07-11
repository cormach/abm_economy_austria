#include "rt_nonfinite.h"
#include "simulate_abm.h"
#include "normc.h"
#include "isfinite.h"
#include "sum.h"

void b_normc(const double xi[3844], double yi[3844])
{
  double dv517[62];
  int i13;
  boolean_T bv1[3844];
  int i;
  int trueCount;
  int yi_tmp;
  boolean_T b1;
  short tmp_data[3844];
  e_sum(xi, dv517);
  for (i13 = 0; i13 < 62; i13++) {
    for (i = 0; i < 62; i++) {
      yi_tmp = i + 62 * i13;
      yi[yi_tmp] = xi[yi_tmp] * (1.0 / dv517[i13]);
    }
  }

  b_isfinite(yi, bv1);
  trueCount = 0;
  for (i = 0; i < 3844; i++) {
    b1 = !bv1[i];
    bv1[i] = b1;
    if (b1) {
      trueCount++;
    }
  }

  yi_tmp = 0;
  for (i = 0; i < 3844; i++) {
    if (bv1[i]) {
      tmp_data[yi_tmp] = (short)(i + 1);
      yi_tmp++;
    }
  }

  for (i13 = 0; i13 < trueCount; i13++) {
    yi[tmp_data[i13] - 1] = 0.0;
  }
}

void normc(const double xi[62], double yi[62])
{
  double n;
  int i;
  int trueCount;
  int partialTrueCount;
  double d1;
  boolean_T b0;
  boolean_T bv0[62];
  signed char tmp_data[62];
  n = xi[0];
  for (i = 0; i < 61; i++) {
    n += xi[i + 1];
  }

  n = 1.0 / n;
  trueCount = 0;
  for (i = 0; i < 62; i++) {
    d1 = xi[i] * n;
    yi[i] = d1;
    b0 = (rtIsInf(d1) || rtIsNaN(d1));
    bv0[i] = b0;
    if (b0) {
      trueCount++;
    }
  }

  partialTrueCount = 0;
  for (i = 0; i < 62; i++) {
    if (bv0[i]) {
      tmp_data[partialTrueCount] = (signed char)(i + 1);
      partialTrueCount++;
    }
  }

  for (i = 0; i < trueCount; i++) {
    yi[tmp_data[i] - 1] = 0.0;
  }
}
