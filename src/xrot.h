#ifndef XROT_H
#define XROT_H
#include <stddef.h>
#include <stdlib.h>
#include "rtwtypes.h"
#include "simulate_abm_types.h"

extern void b_xrot(int n, double x_data[], int ix0, int iy0, double c, double s);
extern void xrot(double x[4], int iy0, double c, double s);

#endif
