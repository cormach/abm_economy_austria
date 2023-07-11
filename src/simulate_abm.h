#ifndef SIMULATE_ABM_H
#define SIMULATE_ABM_H
#include <stddef.h>
#include <stdlib.h>
#include "rtwtypes.h"
#include "simulate_abm_types.h"

extern void simulate_abm(double year, double quarter, double seed, double
  predictors, double nominal_gdp[13], double real_gdp[13], double nominal_gva[13],
  double real_gva[13], double nominal_household_consumption[13], double
  real_household_consumption[13], double nominal_government_consumption[13],
  double real_government_consumption[13], double nominal_capitalformation[13],
  double real_capitalformation[13], double nominal_fixed_capitalformation[13],
  double real_fixed_capitalformation[13], double
  c_nominal_fixed_capitalformatio[13], double c_real_fixed_capitalformation_d[13],
  double nominal_exports[13], double real_exports[13], double nominal_imports[13],
  double real_imports[13], double operating_surplus[13], double
  compensation_employees[13], double wages[13], double taxes_production[13],
  double nominal_sector_gva[806], double real_sector_gva[806], double euribor[13]);

#endif
