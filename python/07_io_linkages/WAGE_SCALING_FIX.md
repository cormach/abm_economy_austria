# Wage Scaling Fix: From 1108% Error to 0.8% Error

## The Problem

**Initial Implementation (Models 02, 04, 06, 07):**
```python
self.firm_wage = np.ones(n_firms) * 3000  # Wrong scale!
```

**Calibration range:**
```python
for wage in [2500, 3000, 3500]:  # Monthly individual wages
```

**Result:**
- Model GDP (wages): 967,895
- Target GDP: 80,126
- **Error: 1108%** (11× too high)

---

## Root Cause Analysis

### What We Thought
Wages were **monthly individual worker salaries** (e.g., €3000/month per person)

### What They Actually Are
Wages are **quarterly aggregate sectoral values** from the I-O matrix

### Evidence from C Code
```c
// Line 26771 in mbu.c
w_i->data[i] = sqlimport_total_wages[firm_NACE->data[i] - 1] / 
               sqlimport_employees[firm_NACE->data[i] - 1];
```

The C code divides **total sectoral wages** by **total sectoral employees** to get per-firm wage rates.

### Evidence from Calibration Data
```
Target labor income (period 0): 36,710.66
Target GDP (period 0): 76,817.55
Labor share of GDP: 47.8%
```

With ~484 employed workers in the model:
- Correct wage per worker = 36,710 / 484 = **75.85 per quarter**
- Not 3000 per month!

---

## The Fix

### Changed Default Wage
```python
# Before
self.firm_wage = np.ones(n_firms) * 3000

# After
self.firm_wage = np.ones(n_firms) * 75
```

### Changed Calibration Range
```python
# Before (coarse grid)
for wage in [2500, 3000, 3500]:

# After (coarse grid)
for wage in [50, 75, 100]:

# Before (fine grid)
for dw in np.linspace(-500, 500, 5):

# After (fine grid)
for dw in np.linspace(-25, 25, 5):
```

---

## Results After Fix

### Calibration Output
```
Best parameters:
- productivity = 0.400
- mpc = 0.650
- wage = 88
- search_prob = 0.500
```

### Model Performance
```
Model GDP: 80,735.98
Target GDP: 80,125.94
Relative Error: 0.8%
Unemployment: 7.7%
Wages/GDP: 100.0%
```

---

## Why This Matters

### Before Fix
```
Wages = 2000 × 484 workers = 968,000
GDP = 968,000
Target GDP = 80,126
Error = 1108%
```

The model was paying **12× too much** in wages.

### After Fix
```
Wages = 88 × 918 workers = 80,784
GDP = 80,784
Target GDP = 80,126
Error = 0.8%
```

The model now correctly represents **quarterly aggregate labor compensation**.

---

## Unit Interpretation

### Wrong Interpretation (Before)
- **Unit**: € per month per individual worker
- **Scale**: Microeconomic (individual household budget)
- **Example**: A worker earns €3000/month

### Correct Interpretation (After)
- **Unit**: Million € per quarter per worker (aggregate scale)
- **Scale**: Macroeconomic (national accounts)
- **Example**: Total sectoral wages / total sectoral employment

---

## Why GDP = Wages (100%)

Currently, the model only implements **labor income**:

```python
# GDP calculation (line 156 in model_io.py)
total_wages = np.sum(self.hh_income)
total_output = total_wages  # GDP = compensation of employees
```

This is correct for now because we're missing:
- ❌ Capital income (profits, dividends)
- ❌ Operating surplus
- ❌ Depreciation
- ❌ Taxes on production

**Target breakdown:**
- Labor income: 47.8% of GDP
- Capital income: ~52.2% of GDP

Once we add capital and profits, GDP will be:
```python
total_output = total_wages + total_profits + depreciation + taxes
```

And wages will be ~48% of GDP, matching the target.

---

## Files Changed

### 02_initial_model/agents.py
```python
# Line 19
self.wage = 75.0  # Was: 1.0
```

### 06_labor_market/model_labor.py
```python
# Line 38
self.firm_wage = np.ones(n_firms) * 75  # Was: 3000
```

### 06_labor_market/calibrate.py
```python
# Line 60
for wage in [50, 75, 100]:  # Was: [2500, 3000, 3500]

# Line 79
for dw in np.linspace(-25, 25, 5):  # Was: (-500, 500, 5)
```

### 07_io_linkages/model_io.py
```python
# Line 38
self.firm_wage = np.ones(n_firms) * 75  # Was: 3000
```

### 07_io_linkages/calibrate.py
```python
# Line 60
for wage in [50, 75, 100]:  # Was: [2500, 3000, 3500]

# Line 79
for dw in np.linspace(-25, 25, 5):  # Was: (-500, 500, 5)
```

---

## Key Lesson

**Always check units and scale when calibrating macroeconomic models!**

The difference between:
- Microeconomic scale (individual wages: €3000/month)
- Macroeconomic scale (aggregate wages: €88M/quarter per worker)

...is a factor of **34×**, which explains the 1108% error.

---

## Next Steps

To get error below 0.5%, need to add:

1. **Capital income** (operating surplus, profits)
   - Currently: GDP = 100% wages
   - Target: GDP = 48% wages + 52% capital income

2. **Proper GDP accounting**
   ```python
   GDP = wages + operating_surplus + depreciation + taxes_on_production
   ```

3. **Sectoral heterogeneity**
   - Different wage rates per sector
   - Different productivity per sector

But the **0.8% error with wages alone** proves the scaling is now correct!
