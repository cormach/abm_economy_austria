#ifndef RANDPERM_H
#define RANDPERM_H
#include <stddef.h>
#include <stdlib.h>
#include "rtwtypes.h"
#include "simulate_abm_types.h"

extern void b_randperm(double n, emxArray_real_T *p);
extern void randperm(double n, double k, emxArray_real_T *p);

#endif
