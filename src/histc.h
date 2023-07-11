#ifndef HISTC_H
#define HISTC_H
#include <stddef.h>
#include <stdlib.h>
#include "rtwtypes.h"
#include "simulate_abm_types.h"

extern void histc(double X, const emxArray_real_T *edges, emxArray_real_T *N,
                  double *BIN);

#endif
