#include "rt_nonfinite.h"
#include "simulate_abm.h"
#include "rng.h"
#include "simulate_abm_data.h"

void rng(double varargin_1)
{
  unsigned int seed;
  int mti;
  if (varargin_1 < 4.294967296E+9) {
    if (varargin_1 >= 0.0) {
      seed = (unsigned int)varargin_1;
    } else {
      seed = 0U;
    }
  } else if (varargin_1 >= 4.294967296E+9) {
    seed = MAX_uint32_T;
  } else {
    seed = 0U;
  }

  if (seed == 0U) {
    seed = 5489U;
  }

  state[0] = seed;
  for (mti = 0; mti < 623; mti++) {
    seed = ((seed ^ seed >> 30U) * 1812433253U + mti) + 1U;
    state[mti + 1] = seed;
  }

  state[624] = 624U;
}
