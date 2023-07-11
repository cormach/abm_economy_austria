#include "rt_nonfinite.h"
#include "simulate_abm.h"
#include "anyNonFinite.h"

boolean_T anyNonFinite(const double x_data[], const int x_size[2])
{
  boolean_T p;
  int nx;
  int k;
  nx = x_size[0] << 1;
  p = true;
  for (k = 0; k < nx; k++) {
    if (p && ((!rtIsInf(x_data[k])) && (!rtIsNaN(x_data[k])))) {
      p = true;
    } else {
      p = false;
    }
  }

  return !p;
}

boolean_T b_anyNonFinite(const double x_data[], const int x_size[2])
{
  boolean_T p;
  int nx;
  int k;
  nx = x_size[0] * x_size[1];
  p = true;
  for (k = 0; k < nx; k++) {
    if (p && ((!rtIsInf(x_data[k])) && (!rtIsNaN(x_data[k])))) {
      p = true;
    } else {
      p = false;
    }
  }

  return !p;
}
