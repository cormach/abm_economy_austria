#ifndef ANYNONFINITE_H
#define ANYNONFINITE_H
#include <stddef.h>
#include <stdlib.h>
#include "rtwtypes.h"
#include "simulate_abm_types.h"

extern boolean_T anyNonFinite(const double x_data[], const int x_size[2]);
extern boolean_T b_anyNonFinite(const double x_data[], const int x_size[2]);

#endif
