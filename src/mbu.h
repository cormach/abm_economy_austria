#ifndef MBU_H
#define MBU_H
#include <stddef.h>
#include <stdlib.h>
#include "rtwtypes.h"
#include "simulate_abm_types.h"

extern void mbu(double year, double quarter, double seed, double predictors,
                struct_T *TS);

#endif
