#include "rt_nonfinite.h"
#include "simulate_abm.h"
#include "mean.h"
#include "combineVectorElements.h"

double mean(const emxArray_real_T *x)
{
  return combineVectorElements(x) / (double)x->size[1];
}
