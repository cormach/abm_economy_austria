#include <string.h>
#include "rt_nonfinite.h"
#include "simulate_abm.h"
#include "mbu.h"

void simulate_abm(double year, double quarter, double seed, double predictors,
                  double nominal_gdp[13], double real_gdp[13], double
                  nominal_gva[13], double real_gva[13], double
                  nominal_household_consumption[13], double
                  real_household_consumption[13], double
                  nominal_government_consumption[13], double
                  real_government_consumption[13], double
                  nominal_capitalformation[13], double real_capitalformation[13],
                  double nominal_fixed_capitalformation[13], double
                  real_fixed_capitalformation[13], double
                  c_nominal_fixed_capitalformatio[13], double
                  c_real_fixed_capitalformation_d[13], double nominal_exports[13],
                  double real_exports[13], double nominal_imports[13], double
                  real_imports[13], double operating_surplus[13], double
                  compensation_employees[13], double wages[13], double
                  taxes_production[13], double nominal_sector_gva[806], double
                  real_sector_gva[806], double euribor[13])
{
  static struct_T expl_temp;
  double TS_firm_K_depreciation[13];
  double TS_price[806];
  int i0;
  int i1;
  int nominal_sector_gva_tmp;
  mbu(year, quarter, seed, predictors, &expl_temp);
  memcpy(&real_sector_gva[0], &expl_temp.sector_value_added[0], 806U * sizeof
         (double));
  memcpy(&nominal_household_consumption[0], &expl_temp.household_consumption[0],
         13U * sizeof(double));
  memcpy(&nominal_government_consumption[0], &expl_temp.government_consumption[0],
         13U * sizeof(double));
  memcpy(&nominal_capitalformation[0], &expl_temp.capitalformation[0], 13U *
         sizeof(double));
  memcpy(&nominal_fixed_capitalformation[0], &expl_temp.fixed_capitalformation[0],
         13U * sizeof(double));
  memcpy(&c_nominal_fixed_capitalformatio[0],
         &expl_temp.c_fixed_capitalformation_dwelli[0], 13U * sizeof(double));
  memcpy(&TS_firm_K_depreciation[0], &expl_temp.firm_K_depreciation[0], 13U *
         sizeof(double));
  memcpy(&nominal_imports[0], &expl_temp.imports[0], 13U * sizeof(double));
  memcpy(&nominal_exports[0], &expl_temp.exports[0], 13U * sizeof(double));
  memcpy(&compensation_employees[0], &expl_temp.cum_labor_cost[0], 13U * sizeof
         (double));
  memcpy(&TS_price[0], &expl_temp.price[0], 806U * sizeof(double));
  for (i0 = 0; i0 < 13; i0++) {
    euribor[i0] = expl_temp.euribor[i0];
    nominal_gdp[i0] = expl_temp.gdp[i0];
    nominal_gva[i0] = expl_temp.nominal_gva[i0];
    real_gva[i0] = expl_temp.real_gva[i0];
    real_gdp[i0] = expl_temp.gdp[i0] / expl_temp.nominal_gva[i0] *
      expl_temp.real_gva[i0];
    real_household_consumption[i0] = nominal_household_consumption[i0] /
      expl_temp.retail_price[i0];
    real_government_consumption[i0] = nominal_government_consumption[i0] /
      expl_temp.government_price[i0];
    real_capitalformation[i0] = nominal_capitalformation[i0] /
      expl_temp.capital_price[i0];
    real_fixed_capitalformation[i0] = nominal_fixed_capitalformation[i0] /
      expl_temp.capital_price[i0];
    c_real_fixed_capitalformation_d[i0] = c_nominal_fixed_capitalformatio[i0] /
      expl_temp.dwellings_price[i0];
    real_exports[i0] = nominal_exports[i0] / expl_temp.export_price[i0];
    real_imports[i0] = nominal_imports[i0] / expl_temp.import_price[i0];
    operating_surplus[i0] = expl_temp.operating_surplus[i0] +
      TS_firm_K_depreciation[i0];
    wages[i0] = compensation_employees[i0] - expl_temp.social_insurance_firm[i0];
    taxes_production[i0] = expl_temp.gov_subsidies[i0] - expl_temp.tax_y[i0];
  }

  for (i0 = 0; i0 < 62; i0++) {
    for (i1 = 0; i1 < 13; i1++) {
      nominal_sector_gva_tmp = i1 + 13 * i0;
      nominal_sector_gva[nominal_sector_gva_tmp] =
        real_sector_gva[nominal_sector_gva_tmp] * TS_price[i0 + 62 * i1];
    }
  }
}
