#ifndef NOM_RATE_H
#define NOM_RATE_H
#include <stddef.h>
#include <stdlib.h>
#include "rtwtypes.h"
#include "simulate_abm_types.h"

extern void nom_rate(const double rate_data[], const int rate_size[2], double
                     nom_rate_data[], int nom_rate_size[2]);

#endif
