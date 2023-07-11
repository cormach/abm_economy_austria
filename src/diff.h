#ifndef DIFF_H
#define DIFF_H
#include <stddef.h>
#include <stdlib.h>
#include "rtwtypes.h"
#include "simulate_abm_types.h"

extern void b_diff(const double x[13], double y[12]);
extern void diff(const double x_data[], const int x_size[1], double y_data[],
                 int y_size[1]);

#endif
