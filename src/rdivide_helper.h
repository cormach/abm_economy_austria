#ifndef RDIVIDE_HELPER_H
#define RDIVIDE_HELPER_H
#include <stddef.h>
#include <stdlib.h>
#include "rtwtypes.h"
#include "simulate_abm_types.h"

extern void b_rdivide_helper(const emxArray_real_T *y, emxArray_real_T *z);
extern void rdivide_helper(const emxArray_real_T *x, const emxArray_real_T *y,
  emxArray_real_T *z);

#endif
