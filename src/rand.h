#ifndef RAND_H
#define RAND_H
#include <stddef.h>
#include <stdlib.h>
#include "rtwtypes.h"
#include "simulate_abm_types.h"

extern void b_rand(double varargin_1, emxArray_real_T *r);
extern double c_rand(void);
extern void d_rand(double varargin_2, emxArray_real_T *r);
extern void genrand_uint32_vector(unsigned int mt[625], unsigned int u[2]);
extern double genrandu(unsigned int mt[625]);

#endif
