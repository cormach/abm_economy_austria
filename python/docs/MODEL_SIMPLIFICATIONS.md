# Model Simplifications: Hybrid vs Original

## Overview

The hybrid model achieves 30x speedup but currently has 95.9% error because it's missing key economic mechanisms from the original C/MATLAB implementation.

---

## Critical Simplifications (Causing High Error)

### 1. **Labor Market - MISSING** ❌

**Original Model** (from agents.py):
```python
def seek_employment(self):
    if not self.employed:
        firms = [a for a in self.model.schedule.agents if isinstance(a, Firm)]
        if firms:
            firm = self.random.choice(firms)
            firm.employees.append(self)
            self.employer = firm
            self.employed = True
```

**Hybrid Model** (agents_vectorized.py):
```python
self.employed = np.zeros(n_households, dtype=bool)  # Always False!
# No job matching implemented
```

**Impact**: 
- Unemployment = 100% (should be ~5%)
- No wage income → no consumption → GDP near zero
- **This is the PRIMARY cause of 95.9% error**

---

### 2. **Production Function - OVERSIMPLIFIED** ❌

**Original Model**:
```python
def produce(self):
    labor = len(self.employees)
    self.production = self.capital ** 0.3 * labor ** 0.7  # Cobb-Douglas
    self.value_added = self.production * self.price
```

**Hybrid Model**:
```python
def produce(self, capital_prod=0.5, labor_prod=1.0):
    self.output = np.minimum(self.capital * capital_prod, 
                             self.labor * labor_prod)  # Leontief
    # But self.labor = 0 always! → output = 0
```

**Impact**:
- GVA = 0 (should be ~80,000)
- No production → no sales → no income

---

### 3. **Input-Output Linkages - MISSING** ❌

**Original Model**:
```python
# Firms buy inputs from other sectors
io_costs = self.model.io_matrix[self.sector, :].sum()
self.unit_cost = (self.wage * self.labor + io_costs) / max(self.production, 1)
```

**Hybrid Model**:
```python
# No I-O matrix usage
unit_costs = self.labor / np.maximum(self.output, 1.0)  # Simplified
```

**Impact**:
- No supply chain effects
- No sectoral interdependencies
- Missing ~40% of production costs

---

### 4. **Wage Dynamics - MISSING** ❌

**Original Model**:
```python
def pay_wages(self):
    for emp in self.employees:
        emp.income += self.wage
```

**Hybrid Model**:
```python
# No wage payment mechanism
self.income = np.zeros(n_households)  # Always zero!
```

**Impact**:
- No household income
- Consumption based only on wealth drawdown
- Wealth depletes → consumption → 0

---

### 5. **Banks & Credit - MISSING** ❌

**Original Model**:
```python
class Bank(Agent):
    def process_loans(self):
        # Basel-style lending
        if firm.debt < firm.capital * 0.5:
            loan_amount = firm.capital * 0.2
            firm.debt += loan_amount
```

**Hybrid Model**:
```python
# No banks at all!
```

**Impact**:
- No credit creation
- No investment financing
- No interest rate effects

---

### 6. **Government - MISSING** ❌

**Original Model**:
```python
class Government(Agent):
    def collect_taxes(self):
        self.revenue = sum(f.profits * 0.25 for f in firms)
    
    def spend(self):
        self.expenditure = self.revenue * 1.1
```

**Hybrid Model**:
```python
# No government at all!
```

**Impact**:
- No fiscal policy
- No automatic stabilizers
- Missing ~20% of GDP (government consumption)

---

### 7. **Trade - OVERSIMPLIFIED** ❌

**Original Model**:
```python
def compute_exports(self):
    # Based on sectoral competitiveness
    return sum(sector_output * export_share for sector in sectors)

def compute_imports(self):
    # Based on domestic demand and I-O requirements
    return sum(sector_demand * import_share for sector in sectors)
```

**Hybrid Model**:
```python
# Not implemented at all
```

**Impact**:
- No external sector
- Missing ~50% of GDP (exports)

---

### 8. **Expectations & Dynamics - MISSING** ❌

**Original Model**:
```python
def plan_production(self):
    # Adaptive expectations
    self.expected_demand = 0.9 * self.actual_sales + 0.1 * self.expected_demand
```

**Hybrid Model**:
```python
# No expectations, no dynamics
```

**Impact**:
- No business cycle
- No adjustment mechanisms

---

## What IS Implemented in Hybrid Model ✓

1. **Vectorized operations** - NumPy arrays for speed
2. **Basic structure** - Firms, households exist as arrays
3. **Consumption function** - `consumption = mpc * (income + wealth_effect * wealth)`
4. **Price setting** - `price = markup * unit_cost`
5. **Wealth accumulation** - `wealth += income - consumption`
6. **Sectoral aggregation** - Firm-level → sector-level

---

## Why the Simplifications?

### Speed vs Accuracy Tradeoff

**Hybrid Model**:
- ✓ 30x faster (0.007s per run)
- ✓ Vectorized operations
- ✗ Missing key mechanisms

**Original Model**:
- ✓ Complete economic mechanisms
- ✓ Matches MATLAB targets
- ✗ Slower (agent-by-agent operations)

---

## How to Fix the High Error

### Priority 1: Add Labor Market (Will fix ~80% of error)

```python
class VectorizedHouseholds:
    def seek_jobs(self, firms, n_jobs_available):
        """Vectorized job matching"""
        unemployed = ~self.employed
        n_unemployed = unemployed.sum()
        
        if n_unemployed > 0 and n_jobs_available > 0:
            # Random matching
            n_matches = min(n_unemployed, n_jobs_available)
            unemployed_idx = np.where(unemployed)[0]
            hired_idx = np.random.choice(unemployed_idx, n_matches, replace=False)
            self.employed[hired_idx] = True
```

### Priority 2: Add Wage Payments

```python
class VectorizedFirms:
    def pay_wages(self, households, wage=1.0):
        """Pay wages to employed households"""
        for i, employed in enumerate(households.employed):
            if employed:
                households.income[i] += wage
```

### Priority 3: Add I-O Matrix

```python
class HybridEconomyModel:
    def __init__(self, ...):
        self.io_matrix = pd.read_csv('data/io_matrix.csv').values
    
    def compute_input_costs(self):
        """Compute intermediate input costs using I-O matrix"""
        sector_output = self.firms.aggregate_by_sector(self.n_sectors)[0]
        input_costs = self.io_matrix @ sector_output
        return input_costs
```

### Priority 4: Add Banks

```python
class VectorizedBanks:
    def __init__(self, n_banks):
        self.equity = np.ones(n_banks) * 100.0
        self.loans = np.zeros(n_banks)
    
    def lend(self, firms, max_leverage=3.0):
        """Vectorized lending"""
        can_borrow = (firms.loans / firms.equity) < max_leverage
        loan_amounts = np.where(can_borrow, firms.equity * 0.1, 0)
        firms.loans += loan_amounts
        firms.deposits += loan_amounts
```

### Priority 5: Add Government

```python
class VectorizedGovernment:
    def __init__(self):
        self.revenue = 0.0
        self.expenditure = 0.0
    
    def collect_taxes(self, firms, households, tax_rate=0.25):
        """Vectorized tax collection"""
        corporate_tax = np.sum(np.maximum(firms.profits, 0) * tax_rate)
        income_tax = np.sum(households.income * tax_rate)
        self.revenue = corporate_tax + income_tax
    
    def spend(self):
        self.expenditure = self.revenue * 1.1
```

---

## Expected Error Reduction

| Fix | Expected Error | Time to Implement |
|-----|---------------|-------------------|
| Current | 95.9% | - |
| + Labor market | ~50% | 30 min |
| + Wage payments | ~30% | 15 min |
| + I-O matrix | ~15% | 30 min |
| + Banks | ~10% | 20 min |
| + Government | ~5% | 20 min |
| + Trade | ~3% | 30 min |
| + Full dynamics | <5% | 2 hours |

---

## Recommendation

**Implement in order**:
1. Labor market (biggest impact)
2. Wage payments (enables consumption)
3. I-O matrix (realistic costs)
4. Banks (credit dynamics)
5. Government (fiscal policy)

**Estimated time**: 2-3 hours to get error < 10%

**Alternative**: Use the full `model.py` with Mesa agents, which already has all mechanisms but is slower. Then optimize that model with vectorization.

---

## Summary

The hybrid model is a **proof of concept** showing:
- ✓ Vectorization works (30x speedup)
- ✓ Parallel calibration works (27s for 14,850 sims)
- ✓ Framework is correct

But it's missing **core economic mechanisms**:
- ❌ Labor market (unemployment = 100% instead of 5%)
- ❌ Production (output = 0 instead of 80,000)
- ❌ Income flows (wages = 0)
- ❌ Banks, government, trade

**Next step**: Add these mechanisms while keeping vectorization for speed.
