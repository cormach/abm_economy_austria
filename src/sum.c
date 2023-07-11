#include <string.h>
#include "rt_nonfinite.h"
#include "simulate_abm.h"
#include "sum.h"
#include "combineVectorElements.h"
#include "simulate_abm_emxutil.h"

double b_sum(const double x[62])
{
  double y;
  int k;
  y = x[0];
  for (k = 0; k < 61; k++) {
    y += x[k + 1];
  }

  return y;
}

double c_sum(const boolean_T x_data[], const int x_size[1])
{
  double y;
  int vlen;
  int k;
  vlen = x_size[0];
  if (x_size[0] == 0) {
    y = 0.0;
  } else {
    y = x_data[0];
    for (k = 2; k <= vlen; k++) {
      y += (double)x_data[k - 1];
    }
  }

  return y;
}

double d_sum(const emxArray_real_T *x)
{
  double y;
  int vlen;
  int k;
  vlen = x->size[0];
  if (x->size[0] == 0) {
    y = 0.0;
  } else {
    y = x->data[0];
    for (k = 2; k <= vlen; k++) {
      y += x->data[k - 1];
    }
  }

  return y;
}

void e_sum(const double x[3844], double y[62])
{
  int i;
  int xpageoffset;
  int k;
  for (i = 0; i < 62; i++) {
    xpageoffset = i * 62;
    y[i] = x[xpageoffset];
    for (k = 0; k < 61; k++) {
      y[i] += x[(xpageoffset + k) + 1];
    }
  }
}

double f_sum(const emxArray_real_T *x)
{
  return combineVectorElements(x);
}

void g_sum(const emxArray_real_T *x, emxArray_real_T *y)
{
  int npages;
  int i;
  int xpageoffset;
  int k;
  npages = x->size[1];
  i = y->size[0] * y->size[1];
  y->size[0] = 1;
  y->size[1] = x->size[1];
  emxEnsureCapacity_real_T(y, i);
  for (i = 0; i < npages; i++) {
    xpageoffset = i * 62;
    y->data[i] = x->data[xpageoffset];
    for (k = 0; k < 61; k++) {
      y->data[i] += x->data[(xpageoffset + k) + 1];
    }
  }
}

void h_sum(const emxArray_real_T *x, double y[62])
{
  int vlen;
  int j;
  int k;
  int xoffset;
  vlen = x->size[1];
  for (j = 0; j < 62; j++) {
    y[j] = x->data[j];
  }

  for (k = 2; k <= vlen; k++) {
    xoffset = (k - 1) * 62;
    for (j = 0; j < 62; j++) {
      y[j] += x->data[xoffset + j];
    }
  }
}

void sum(const double x[3844], double y[62])
{
  int k;
  int xoffset;
  int j;
  memcpy(&y[0], &x[0], 62U * sizeof(double));
  for (k = 0; k < 61; k++) {
    xoffset = (k + 1) * 62;
    for (j = 0; j < 62; j++) {
      y[j] += x[xoffset + j];
    }
  }
}
