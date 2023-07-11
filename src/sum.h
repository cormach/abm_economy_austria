#ifndef SUM_H
#define SUM_H
#include <stddef.h>
#include <stdlib.h>
#include "rtwtypes.h"
#include "simulate_abm_types.h"

extern double b_sum(const double x[62]);
extern double c_sum(const boolean_T x_data[], const int x_size[1]);
extern double d_sum(const emxArray_real_T *x);
extern void e_sum(const double x[3844], double y[62]);
extern double f_sum(const emxArray_real_T *x);
extern void g_sum(const emxArray_real_T *x, emxArray_real_T *y);
extern void h_sum(const emxArray_real_T *x, double y[62]);
extern void sum(const double x[3844], double y[62]);

#endif
