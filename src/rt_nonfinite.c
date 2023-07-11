#include "rt_nonfinite.h"
#include <math.h>

real_T rtInf;
real_T rtMinusInf;
real_T rtNaN;
real32_T rtInfF;
real32_T rtMinusInfF;
real32_T rtNaNF;

#if defined(_MSC_VER) && _MSC_VER == 1800

#pragma warning(disable: 4756 56)

#endif

void rt_InitInfAndNaN(size_t realSize)
{
  (void)realSize;
  rtNaN = nan("");
  rtNaNF = nanf("");
  rtInf = (real_T)INFINITY;
  rtInfF = (real32_T)INFINITY;
  rtMinusInf = -(real_T)INFINITY;
  rtMinusInfF = -(real32_T)INFINITY;

#if defined(_MSC_VER) && _MSC_VER == 1800

#pragma warning(default: 4756 56)

#endif

}

boolean_T rtIsInf(real_T value)
{
  return (isinf(value) ? 1U : 0U);
}

boolean_T rtIsInfF(real32_T value)
{
  return (isinf((real_T)value) ? 1U : 0U);
}

boolean_T rtIsNaN(real_T value)
{
  return (isnan(value) ? 1U : 0U);
}

boolean_T rtIsNaNF(real32_T value)
{
  return (isnan((real_T)value) ? 1U : 0U);
}
