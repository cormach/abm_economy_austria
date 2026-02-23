# Gaps: C Implementation vs Python 07

## C Model Structure (from struct_T)

The C code has **100+ variables** tracking:

### Production & Output
- ✅ `sector_value_added[806]` (62 sectors × 13 periods)
- ✅ `sector_Ytot[806]` - Total sectoral output
- ❌ `sector_material_cost[806]` - Intermediate input costs
- ❌ `sector_K[806]` - Capital stock by sector
- ❌ `sector_employees[806]` - Employment by sector

### Demand Components
- ✅ `household_consumption[13]`
- ✅ `sector_household_consumption[806]`
- ❌ `government_consumption[13]`
- ❌ `sector_government_consumption[806]`
- ❌ `firm_consumption[13]` - Intermediate consumption
- ❌ `sector_firm_consumption[806]`
- ❌ `capitalformation[13]` - Investment
- ❌ `fixed_capitalformation[13]`
- ❌ `sector_firm_K_formation[806]`
- ❌ `firm_K_depreciation[13]`

### Trade
- ❌ `exports[13]`
- ❌ `sector_exports[806]`
- ❌ `imports[13]`
- ❌ `sector_imports[806]`
- ❌ `export_demand[13]`
- ❌ `import_demand[13]`
- ❌ `trade_balance[13]`

### Prices
- ❌ `price[806]` - Sectoral prices (62 × 13)
- ❌ `retail_price[13]` - Consumer price index
- ❌ `capital_price[13]` - Investment goods price
- ❌ `government_price[13]`
- ❌ `export_price[13]`
- ❌ `import_price[13]`
- ❌ `infla_rate[13]`

### Labor Market
- ❌ `cum_labor_cost[13]` - Total wage bill
- ❌ `sector_wages[806]` - Wages by sector
- ❌ `sector_social_contributions[806]` - Payroll taxes
- ❌ `Un[13]` - Unemployment rate
- ❌ `income[13]` - Household income

### Financial Sector
- ❌ `euribor[13]` - Interest rate
- ❌ `interestrate[13]`
- ❌ `wealthF[13]` - Firm wealth
- ❌ `wealthHH[13]` - Household wealth
- ❌ `baddebt[13]`
- ❌ `credits[13]`
- ❌ `debits[13]`
- ❌ `defaults[13]`
- ❌ `debtF[13]` - Firm debt
- ❌ `cashF[13]` - Firm cash

### Profits & Equity
- ❌ `profits[13]` - Total profits
- ❌ `sector_profits[806]` - Sectoral profits
- ❌ `gross_profits[13]`
- ❌ `profitsB[13]` - Bank profits
- ❌ `operating_surplus[13]`
- ❌ `dividends_income_gross[13]`
- ❌ `dividends_income_net[13]`

### Government
- ❌ `government_debt[13]`
- ❌ `government_deficit[13]`
- ❌ `government_revenue[13]`
- ❌ `government_expenditure[13]`
- ❌ `gov_subsidies[13]`
- ❌ `gov_vat[13]` - VAT revenue
- ❌ `gov_firmtax[13]` - Corporate tax
- ❌ `gov_cgt[13]` - Capital gains tax
- ❌ `gov_inctax[13]` - Income tax
- ❌ `gov_pp[13]` - Pension payments
- ❌ `gov_ub[13]` - Unemployment benefits
- ❌ `gov_sb[13]` - Social benefits
- ❌ `gov_interest[13]` - Interest on debt
- ❌ `tax_y[13]` - Production taxes
- ❌ `taxes_less_subsidies_prod[806]`

### GDP Components
- ✅ `gdp[13]`
- ✅ `nominal_gva[13]`
- ✅ `real_gva[13]`

---

## What Python 07 Has

```python
# Firms (620 firms)
firm_sector[620]           # Sector assignment
firm_labor[620]            # Employment
firm_output[620]           # Production
firm_inventory[620]        # Stock
firm_sales[620]            # Sales
firm_expected_demand[620]  # Expectations
firm_wage[620]             # Wage rate

# Households (1000 households)
hh_employed[1000]          # Employment status
hh_employer[1000]          # Employer ID
hh_income[1000]            # Wage income
hh_wealth[1000]            # Savings

# I-O Matrix
io_matrix[62×62]           # Sectoral linkages
```

---

## Critical Gaps in Python 07

### 1. **Capital & Investment** ❌
- No capital stock (`sector_K`)
- No investment (`capitalformation`)
- No depreciation (`firm_K_depreciation`)
- Production only from labor, not capital

### 2. **Prices** ❌
- All prices = 1 (no price dynamics)
- No sectoral prices (`price[806]`)
- No inflation (`infla_rate`)
- No real vs nominal distinction

### 3. **Government** ❌
- No taxes (VAT, income tax, corporate tax)
- No transfers (unemployment benefits, pensions)
- No government consumption
- No fiscal policy

### 4. **Banks** ❌
- No credit/loans
- No interest rates (euribor)
- No defaults/bad debt
- No monetary policy

### 5. **Trade** ❌
- No exports
- No imports
- No trade balance
- Closed economy assumption

### 6. **Intermediate Inputs** ⚠️
- I-O matrix loaded but **not used correctly**
- Firms don't actually buy inputs from other firms
- No material costs (`sector_material_cost`)
- No firm-to-firm transactions

### 7. **Profits & Dividends** ❌
- No profit calculation
- No dividends to households
- No capital income
- Only wage income

### 8. **Wealth Dynamics** ⚠️
- Household wealth exists but not used
- No firm wealth (`wealthF`)
- No balance sheets
- No stock-flow consistency

---

## Why Error is Still 92.7%

The model is missing **~80% of the economic mechanisms**:

| Component | Weight in GDP | Status |
|-----------|---------------|--------|
| Household consumption | ~55% | ✅ Partial (no capital income) |
| Government consumption | ~20% | ❌ Missing |
| Investment | ~20% | ❌ Missing |
| Net exports | ~5% | ❌ Missing |
| **Total implemented** | **~30%** | |

Even with I-O linkages, we're only modeling **labor → production → wages → consumption**. 

The C model has:
- Capital accumulation
- Multi-sector prices
- Government fiscal policy
- Banking/credit
- International trade
- Full stock-flow accounting

---

## Next Steps to Reduce Error

**Priority 1: Capital & Investment** (would add ~20% of GDP)
```python
firm_capital[620]
firm_investment[620]
firm_depreciation[620]
production = f(labor, capital)  # Cobb-Douglas
```

**Priority 2: Government** (would add ~20% of GDP)
```python
gov_consumption
gov_taxes (VAT, income, corporate)
gov_transfers (unemployment benefits)
gov_debt
```

**Priority 3: Prices** (would improve accuracy)
```python
sector_prices[62]
inflation_rate
real vs nominal GDP
```

**Priority 4: Trade** (would add ~5% of GDP)
```python
exports[62]
imports[62]
trade_balance
```

**Priority 5: Banks** (would add credit dynamics)
```python
firm_loans[620]
interest_rates
defaults
```

---

## Estimated Error Reduction

| Model | Components | Expected Error |
|-------|-----------|----------------|
| Current (07) | Labor + I-O | 92.7% |
| + Capital | Labor + I-O + Investment | ~75% |
| + Government | + Fiscal policy | ~60% |
| + Prices | + Inflation dynamics | ~50% |
| + Trade | + Exports/imports | ~45% |
| + Banks | Full model | ~30-40% |

To match the C model's accuracy, need **all 6 components**.
