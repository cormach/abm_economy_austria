# Labor Market: Original (02) vs Hybrid (04) vs Labor Market (06)

## Original Model (02_initial_model)

**Has labor market:**
```python
Household.search_job():
  - Workers search for jobs (30% probability)
  - Random matching to any firm
  - Firm.labor += 1
  - household.employed = True
  
Firm.produce():
  - output = min(capital × 0.5, labor × 1.0)  # Leontief production
  - Production constrained by labor
  
Firm.pay_wages():
  - wage_bill = wage × labor
  - deposits -= wage_bill
```

**Bug**: Wages deducted from firm but never credited to workers → workers have zero income

---

## Hybrid Model (04_calibration)

**No labor market at all:**
```python
# No search_job()
# No hiring
# No firing
# No wage payments
# No employment tracking

households.employed = [False, False, False, ...]  # Always unemployed
households.wage_income = 0                         # Always zero
```

**Result**: 100% unemployment, zero income, zero consumption

---

## Labor Market Model (06_labor_market)

**Full labor market:**
```python
labor_market():
  - Firms post vacancies based on expected_demand
  - target_labor = expected_demand / productivity
  - vacancies = target_labor - current_labor
  - Match unemployed workers to vacancies
  
production():
  - output = labor × productivity
  
wages():
  - For each employed worker:
      worker.income = firm.wage  # ✅ Actually pays workers!
```

**Result**: 0% unemployment, workers have income, consumption happens

---

## What Hybrid (04) is Missing vs Original (02)

| Feature | Original (02) | Hybrid (04) | Labor Market (06) |
|---------|---------------|-------------|-------------------|
| **Job search** | ✅ Random (30% prob) | ❌ None | ✅ Vacancy-based |
| **Hiring** | ✅ Random firm | ❌ None | ✅ Based on demand |
| **Employment tracking** | ✅ employed flag | ❌ Always False | ✅ employed flag |
| **Wage payment** | ⚠️ Deducted but not paid | ❌ None | ✅ Paid to workers |
| **Production constraint** | ✅ min(capital, labor) | ❌ Unconstrained | ✅ labor × productivity |
| **Firing** | ❌ None | ❌ None | ❌ None |
| **Vacancies** | ❌ None | ❌ None | ✅ Based on demand |

---

## Why Hybrid Was Created

The hybrid model (04) was designed for **speed optimization** (vectorization, parallelization), not economic realism. It stripped out:

1. Labor market matching
2. Wage payments
3. Employment dynamics
4. Production constraints

This made it **30× faster** but **economically broken** (100% unemployment, 100% error).

---

## Summary

**Original (02)**: Has labor market structure but broken wage payment  
**Hybrid (04)**: No labor market at all (optimization focus)  
**Labor Market (06)**: Complete working labor market with feedback loop

The progression:
1. **02**: Attempted labor market (buggy)
2. **04**: Removed for speed (broken economy)
3. **06**: Rebuilt correctly (working economy)
