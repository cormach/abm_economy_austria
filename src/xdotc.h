#ifndef XDOTC_H
#define XDOTC_H
#include <stddef.h>
#include <stdlib.h>
#include "rtwtypes.h"
#include "simulate_abm_types.h"

extern double xdotc(int n, const double x_data[], int ix0, const double y_data[],
                    int iy0);

#endif
