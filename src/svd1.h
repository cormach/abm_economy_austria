#ifndef SVD1_H
#define SVD1_H
#include <stddef.h>
#include <stdlib.h>
#include "rtwtypes.h"
#include "simulate_abm_types.h"

extern void b_svd(const double A_data[], const int A_size[2], double U_data[],
                  int U_size[2], double s_data[], int s_size[1], double V[4]);
extern void d_svd(const double A_data[], const int A_size[2], double U_data[],
                  int U_size[2], double s_data[], int s_size[1], double V_data[],
                  int V_size[2]);

#endif
