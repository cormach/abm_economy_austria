#include "rt_nonfinite.h"
#include "simulate_abm.h"
#include "simulate_abm_initialize.h"
#include "eml_rand_mt19937ar_stateful.h"

void simulate_abm_initialize(void)
{
  rt_InitInfAndNaN(8U);
  c_eml_rand_mt19937ar_stateful_i();
}
