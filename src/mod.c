#include <math.h>
#include "rt_nonfinite.h"
#include "simulate_abm.h"
#include "mod.h"

double b_mod(double x, double y)
{
  double r;
  r = x;
  if (rtIsNaN(x) || rtIsNaN(y) || rtIsInf(x)) {
    r = rtNaN;
  } else if (x == 0.0) {
    r = 0.0;
  } else if (rtIsInf(y)) {
    if (x < 0.0) {
      r = y;
    }
  } else {
    r = fmod(x, y);
    if (r == 0.0) {
      r = 0.0;
    } else {
      if (x < 0.0) {
        r += y;
      }
    }
  }

  return r;
}
