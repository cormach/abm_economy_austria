#include "rt_nonfinite.h"
#include "simulate_abm.h"
#include "isfinite.h"

void b_isfinite(const double x[3844], boolean_T b[3844])
{
  int i14;
  for (i14 = 0; i14 < 3844; i14++) {
    b[i14] = ((!rtIsInf(x[i14])) && (!rtIsNaN(x[i14])));
  }
}
