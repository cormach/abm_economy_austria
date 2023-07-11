#ifndef SIMS_SIMULATE_H
#define SIMS_SIMULATE_H
#include <stddef.h>
#include <stdlib.h>
#include "rtwtypes.h"
#include "simulate_abm_types.h"

extern void sims_simulate(double b_y0, double By, const double Bx_data[], const
  int Bx_size[2], const double xdata_data[], const int xdata_size[2], double
  sigma, double yhat[2]);

#endif
