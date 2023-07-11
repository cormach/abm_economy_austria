#ifndef XAXPY_H
#define XAXPY_H
#include <stddef.h>
#include <stdlib.h>
#include "rtwtypes.h"
#include "simulate_abm_types.h"

extern void b_xaxpy(int n, double a, const double x_data[], int ix0, double
                    y_data[], int iy0);
extern void xaxpy(int n, double a, int ix0, double y_data[], int iy0);

#endif
