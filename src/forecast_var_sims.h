#ifndef FORECAST_VAR_SIMS_H
#define FORECAST_VAR_SIMS_H
#include <stddef.h>
#include <stdlib.h>
#include "rtwtypes.h"
#include "simulate_abm_types.h"

extern void forecast_var_sims(const double rate0_data[], const int rate0_size[1],
  double Y[12]);

#endif
