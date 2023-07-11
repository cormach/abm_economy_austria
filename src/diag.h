#ifndef DIAG_H
#define DIAG_H
#include <stddef.h>
#include <stdlib.h>
#include "rtwtypes.h"
#include "simulate_abm_types.h"

extern void b_diag(const double v_data[], const int v_size[1], double d_data[],
                   int d_size[2]);
extern void c_diag(const double v_data[], const int v_size[2], double d_data[],
                   int d_size[1]);
extern void diag(const double v_data[], const int v_size[2], double d_data[],
                 int d_size[1]);

#endif
