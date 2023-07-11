#ifndef RFVAR3_H
#define RFVAR3_H
#include <stddef.h>
#include <stdlib.h>
#include "rtwtypes.h"
#include "simulate_abm_types.h"

extern void b_rfvar3(const double ydata_data[], const int ydata_size[1], const
                     double xdata_data[], const int xdata_size[2], double
                     *var_By, double var_Bx_data[], int var_Bx_size[2], double
                     var_u_data[], int var_u_size[1], double var_xxi_data[], int
                     var_xxi_size[2], double *var_snglty);
extern void rfvar3(const double ydata_data[], const int ydata_size[1], const
                   double xdata_data[], const int xdata_size[1], double *var_By,
                   double var_Bx_data[], int var_Bx_size[2], double var_u_data[],
                   int var_u_size[1], double var_xxi[4], double *var_snglty);

#endif
