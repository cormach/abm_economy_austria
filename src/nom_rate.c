#include <math.h>
#include <string.h>
#include "rt_nonfinite.h"
#include "simulate_abm.h"
#include "nom_rate.h"

void nom_rate(const double rate_data[], const int rate_size[2], double
              nom_rate_data[], int nom_rate_size[2])
{
  int loop_ub;
  int k;
  nom_rate_size[0] = rate_size[0];
  loop_ub = rate_size[0] * rate_size[1];
  if (0 <= loop_ub - 1) {
    memcpy(&nom_rate_data[0], &rate_data[0], (unsigned int)(loop_ub * (int)
            sizeof(double)));
  }

  loop_ub = rate_size[0] << 1;
  for (k = 0; k < loop_ub; k++) {
    nom_rate_data[k] = exp(nom_rate_data[k]);
  }

  nom_rate_size[1] = 2;
  loop_ub--;
  for (k = 0; k <= loop_ub; k++) {
    nom_rate_data[k]--;
  }
}
