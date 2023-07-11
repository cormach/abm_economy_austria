#ifndef SIMULATE_ABM_TYPES_H
#define SIMULATE_ABM_TYPES_H
#include "rtwtypes.h"
#ifndef struct_emxArray_real_T_81x4
#define struct_emxArray_real_T_81x4

struct emxArray_real_T_81x4
{
  double data[324];
  int size[2];
};

#endif

#ifndef typedef_emxArray_real_T_81x4
#define typedef_emxArray_real_T_81x4

typedef struct emxArray_real_T_81x4 emxArray_real_T_81x4;

#endif

#ifndef struct_sFevw8JZtia8UdhLVtIuNHF_tag
#define struct_sFevw8JZtia8UdhLVtIuNHF_tag

struct sFevw8JZtia8UdhLVtIuNHF_tag
{
  emxArray_real_T_81x4 f1;
};

#endif

#ifndef typedef_cell_wrap_1
#define typedef_cell_wrap_1

typedef struct sFevw8JZtia8UdhLVtIuNHF_tag cell_wrap_1;

#endif

#ifndef struct_emxArray_boolean_T
#define struct_emxArray_boolean_T

struct emxArray_boolean_T
{
  boolean_T *data;
  int *size;
  int allocatedSize;
  int numDimensions;
  boolean_T canFreeData;
};

#endif

#ifndef typedef_emxArray_boolean_T
#define typedef_emxArray_boolean_T

typedef struct emxArray_boolean_T emxArray_boolean_T;

#endif

#ifndef struct_emxArray_int32_T
#define struct_emxArray_int32_T

struct emxArray_int32_T
{
  int *data;
  int *size;
  int allocatedSize;
  int numDimensions;
  boolean_T canFreeData;
};

#endif

#ifndef typedef_emxArray_int32_T
#define typedef_emxArray_int32_T

typedef struct emxArray_int32_T emxArray_int32_T;

#endif

#ifndef struct_emxArray_int8_T
#define struct_emxArray_int8_T

struct emxArray_int8_T
{
  signed char *data;
  int *size;
  int allocatedSize;
  int numDimensions;
  boolean_T canFreeData;
};

#endif

#ifndef typedef_emxArray_int8_T
#define typedef_emxArray_int8_T

typedef struct emxArray_int8_T emxArray_int8_T;

#endif

#ifndef struct_emxArray_real_T
#define struct_emxArray_real_T

struct emxArray_real_T
{
  double *data;
  int *size;
  int allocatedSize;
  int numDimensions;
  boolean_T canFreeData;
};

#endif

#ifndef typedef_emxArray_real_T
#define typedef_emxArray_real_T

typedef struct emxArray_real_T emxArray_real_T;

#endif

#ifndef struct_emxArray_uint32_T
#define struct_emxArray_uint32_T

struct emxArray_uint32_T
{
  unsigned int *data;
  int *size;
  int allocatedSize;
  int numDimensions;
  boolean_T canFreeData;
};

#endif

#ifndef typedef_emxArray_uint32_T
#define typedef_emxArray_uint32_T

typedef struct emxArray_uint32_T emxArray_uint32_T;

#endif

#ifndef typedef_struct_T
#define typedef_struct_T

typedef struct {
  double Ytot[13];
  double sector_value_added[806];
  double sector_Ytot[806];
  double household_consumption[13];
  double sector_household_consumption[806];
  double government_consumption[13];
  double sector_government_consumption[806];
  double firm_consumption[13];
  double sector_firm_consumption[806];
  double capitalformation[13];
  double fixed_capitalformation[13];
  double c_fixed_capitalformation_dwelli[13];
  double sector_firm_K_formation[806];
  double firm_K_depreciation[13];
  double sector_firm_K_depreciation[806];
  double imports[13];
  double sector_imports[806];
  double exports[13];
  double sector_exports[806];
  double sector_material_cost[806];
  double growth_rate[13];
  double total_demand_dom[13];
  double total_demand_dom_imp[13];
  double sector_household_demand[806];
  double sector_firm_demand[806];
  double government_demand[13];
  double sector_government_demand[806];
  double sector_K_demand[806];
  double sector_K_demand_value[806];
  double export_demand[13];
  double sector_export_demand[806];
  double import_demand[13];
  double nominal_import_supply[13];
  double real_import_supply[13];
  double dHH[806];
  double dCF[806];
  double dG_normc[806];
  double dE_normc[806];
  double dI_normc[806];
  double cum_labor_cost[13];
  double wealthHH[13];
  double dwellings[13];
  double cum_labor_cost_net[13];
  double dividends_income_gross[13];
  double dividends_income_net[13];
  double income[13];
  double price[806];
  double retail_price[13];
  double capital_price[13];
  double dwellings_price[13];
  double government_price[13];
  double export_price[13];
  double import_price[13];
  double infla_rate[13];
  double euribor[13];
  double Un[13];
  double interestrate[13];
  double wealthF[13];
  double baddebt[13];
  double totE[13];
  double credits[13];
  double debits[13];
  double defaults[13];
  double profits[13];
  double sector_profits[806];
  double gross_profits[13];
  double profitsB[13];
  double cashF[13];
  double trade_balance[13];
  double debtF[13];
  double sector_employees[806];
  double sector_K[806];
  double sector_wages[806];
  double sector_social_contributions[806];
  double sales[13];
  double gdp[13];
  double assets[13];
  double nominal_gva[13];
  double real_gva[13];
  double operating_surplus[13];
  double social_insurance_firm[13];
  double social_insurance_worker[13];
  double gov_subsidies[13];
  double gov_vat[13];
  double gov_vat_lost[13];
  double gov_firmtax[13];
  double gov_cgt[13];
  double gov_inctax[13];
  double gov_pp[13];
  double gov_ub[13];
  double gov_sb[13];
  double gov_interest[13];
  double government_debt[13];
  double government_deficit[13];
  double tax_export[13];
  double tax_cf[13];
  double tax_y[13];
  double tax_gov[13];
  double taxes_less_subsidies_prod[806];
  double government_revenue[13];
  double government_expenditure[13];
  double supply_chain_disruption[13];
} struct_T;

#endif
#endif
