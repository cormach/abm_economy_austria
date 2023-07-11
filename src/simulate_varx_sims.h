#ifndef SIMULATE_VARX_SIMS_H
#define SIMULATE_VARX_SIMS_H
#include <stddef.h>
#include <stdlib.h>
#include "rtwtypes.h"
#include "simulate_abm_types.h"

extern double simulate_varx_sims(const double rate0_data[], const int
  rate0_size[1], const double exo_data[], const int exo_size[2]);

#endif
