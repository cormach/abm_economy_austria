#ifndef XNRM2_H
#define XNRM2_H
#include <stddef.h>
#include <stdlib.h>
#include "rtwtypes.h"
#include "simulate_abm_types.h"

extern double b_xnrm2(int n, const double x_data[], int ix0);
extern double c_xnrm2(int n, const double x_data[], int ix0);
extern double xnrm2(int n, const double x_data[], int ix0);

#endif
