#ifndef SVD_H
#define SVD_H
#include <stddef.h>
#include <stdlib.h>
#include "rtwtypes.h"
#include "simulate_abm_types.h"

extern void c_svd(const double A_data[], const int A_size[2], double U_data[],
                  int U_size[2], double S_data[], int S_size[2], double V_data[],
                  int V_size[2]);
extern void svd(const double A_data[], const int A_size[2], double U_data[], int
                U_size[2], double S_data[], int S_size[2], double V[4]);

#endif
