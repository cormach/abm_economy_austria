# Calibration Methodology for Austrian Economy ABM

## Overview

This document explains how to calibrate the Python/Mesa implementation using the extracted MATLAB simulation data (16,000 runs) and empirical Austrian economic data.

## Calibration Data Sources

### 1. **Extracted MATLAB Simulations** ✓
- **16,000 simulation runs** from original C implementation
- **23 economic variables** with full distributions
- **13 quarterly periods** (3.25 years forecast)
- **Format**: Mean, std, and percentiles (p5, p25, p50, p75, p95)

### 2. **Input-Output Matrix** ✓
- **62×62 sectoral transaction matrix**
- Represents Austrian economy structure
- Source: Statistics Austria

### 3. **Missing Empirical Data** ❌
- Firm-level microdata
- Household wealth distribution
- Historical time series for VAR estimation

---

## Calibration Strategy

### Phase 1: Structural Calibration (Using I-O Matrix)

**Objective**: Match the sectoral structure of the Austrian economy

```python
# Load I-O matrix
io_matrix = pd.read_csv('data/io_matrix.csv', index_col=0)

# Derive firm distribution per sector
sector_output = io_matrix.sum(axis=1)  # Total output per sector
n_firms_per_sector = (sector_output / sector_output.mean() * 10).astype(int)

# Derive initial capital per sector
capital_per_sector = sector_output * 0.3  # Capital-output ratio ~0.3
```

**Key Parameters**:
- Number of firms per sector (from I-O output shares)
- Initial capital stock (from I-O total output)
- Technical coefficients (I-O normalized by columns)

---

### Phase 2: Behavioral Calibration (Using MATLAB Distributions)

**Objective**: Match aggregate dynamics from original model

#### 2.1 Firm Behavior

```python
# Load calibration targets
gdp_target = pd.read_csv('data/calibration_nominal_gdp.csv')
gva_target = pd.read_csv('data/calibration_nominal_gva.csv')

# Calibrate markup parameter
def calibrate_markup():
    markups = np.linspace(1.05, 1.30, 50)
    best_markup = None
    min_error = float('inf')
    
    for markup in markups:
        model = AustrianEconomyModel(markup=markup)
        results = model.run(steps=13)
        
        # Compare to target
        error = np.mean((results['nominal_gva'] - gva_target['mean'])**2)
        
        if error < min_error:
            min_error = error
            best_markup = markup
    
    return best_markup
```

**Parameters to Calibrate**:
- Markup: 1.05 - 1.30 (target: match GVA dynamics)
- Inventory target: 0.1 - 0.3 (target: match production volatility)
- Price adjustment speed: 0.1 - 0.5 (target: match inflation)

#### 2.2 Household Behavior

```python
# Load consumption target
consumption_target = pd.read_csv('data/calibration_nominal_household_consumption.csv')

# Calibrate consumption propensity
def calibrate_consumption():
    propensities = np.linspace(0.6, 0.9, 30)
    
    for mpc in propensities:
        model = AustrianEconomyModel(consumption_propensity=mpc)
        results = model.run(steps=13)
        
        # Compare consumption path
        error = np.mean((results['total_consumption'] - consumption_target['mean'])**2)
        # ... select best
```

**Parameters to Calibrate**:
- Consumption propensity: 0.6 - 0.9 (target: match consumption/GDP ratio)
- Wealth effect: 0.01 - 0.10 (target: match consumption volatility)
- Job search intensity: 0.1 - 0.5 (target: match unemployment rate)

#### 2.3 Bank Behavior

```python
# Calibrate credit supply
euribor_target = pd.read_csv('data/calibration_euribor.csv')

# Parameters:
# - Interest rate spread: 0.01 - 0.05
# - Max leverage ratio: 2.0 - 5.0
# - Default threshold: 0.0 - 0.1
```

#### 2.4 Government Behavior

```python
# Load fiscal targets
gov_consumption = pd.read_csv('data/calibration_nominal_government_consumption.csv')

# Parameters:
# - Tax rates (VAT, income, corporate)
# - Unemployment benefit replacement rate
# - Government consumption rule
```

---

### Phase 3: Validation (Statistical Matching)

**Objective**: Ensure Python model replicates MATLAB distributions

#### 3.1 Moment Matching

```python
def validate_model(n_runs=100):
    """Run model multiple times and compare to MATLAB distributions"""
    
    results = []
    for seed in range(n_runs):
        model = AustrianEconomyModel(seed=seed)
        df = model.run(steps=13)
        results.append(df)
    
    # Aggregate results
    all_results = pd.concat(results)
    
    # Compare moments
    variables = ['nominal_gdp', 'real_gdp', 'nominal_gva', 'unemployment']
    
    for var in variables:
        matlab_target = pd.read_csv(f'data/calibration_{var}.csv')
        
        python_mean = all_results.groupby('period')[var].mean()
        python_std = all_results.groupby('period')[var].std()
        
        # Statistical tests
        mean_error = np.mean((python_mean - matlab_target['mean'])**2)
        std_error = np.mean((python_std - matlab_target['std'])**2)
        
        print(f"{var}: Mean RMSE={np.sqrt(mean_error):.2f}, Std RMSE={np.sqrt(std_error):.2f}")
```

#### 3.2 Distribution Matching

```python
def compare_distributions(variable='nominal_gdp', period=12):
    """Compare full distribution at specific time period"""
    
    # MATLAB distribution
    matlab_data = pd.read_csv(f'data/calibration_{variable}.csv')
    matlab_percentiles = matlab_data.iloc[period][['p5', 'p25', 'p50', 'p75', 'p95']]
    
    # Python distribution
    python_results = []
    for seed in range(1000):
        model = AustrianEconomyModel(seed=seed)
        df = model.run(steps=13)
        python_results.append(df[variable].iloc[period])
    
    python_percentiles = np.percentile(python_results, [5, 25, 50, 75, 95])
    
    # Compare
    print(f"MATLAB: {matlab_percentiles.values}")
    print(f"Python: {python_percentiles}")
```

---

## Calibration Workflow

### Step 1: Initial Setup
```bash
# Load all calibration data
python read_matlab_calibration.py

# Verify data extraction
ls data/calibration_*.csv
```

### Step 2: Structural Parameters (Fixed)
```python
# These come directly from data
io_matrix = load_io_matrix()
n_sectors = 62
time_periods = 13
```

### Step 3: Behavioral Parameters (Optimize)

**Method**: Grid search or optimization

```python
from scipy.optimize import minimize

def objective_function(params):
    """
    params = [markup, consumption_propensity, interest_spread, ...]
    Returns: Sum of squared errors across all target variables
    """
    
    markup, mpc, spread = params
    
    # Run model
    model = AustrianEconomyModel(
        markup=markup,
        consumption_propensity=mpc,
        interest_spread=spread
    )
    results = model.run(steps=13)
    
    # Load targets
    gdp_target = pd.read_csv('data/calibration_nominal_gdp.csv')
    consumption_target = pd.read_csv('data/calibration_nominal_household_consumption.csv')
    
    # Compute errors
    gdp_error = np.sum((results['nominal_gdp'] - gdp_target['mean'])**2)
    cons_error = np.sum((results['total_consumption'] - consumption_target['mean'])**2)
    
    return gdp_error + cons_error

# Optimize
initial_params = [1.15, 0.75, 0.02]
bounds = [(1.05, 1.30), (0.6, 0.9), (0.01, 0.05)]

result = minimize(objective_function, initial_params, bounds=bounds, method='L-BFGS-B')
optimal_params = result.x
```

### Step 4: Validation

```python
# Run 1000 simulations with calibrated parameters
validate_model(n_runs=1000)

# Check if within 95% confidence interval of MATLAB results
for var in ['nominal_gdp', 'real_gdp', 'unemployment']:
    check_confidence_interval(var)
```

---

## Success Criteria

### Primary Targets (Must Match)
1. **GDP path**: Mean within 5% of MATLAB
2. **GVA path**: Mean within 5% of MATLAB
3. **Consumption/GDP ratio**: Within 2 percentage points
4. **Unemployment rate**: Within 1 percentage point

### Secondary Targets (Should Match)
5. **GDP volatility**: Std within 20% of MATLAB
6. **Sectoral GVA**: Correlation > 0.9 with MATLAB
7. **Distribution shape**: Percentiles within 10% of MATLAB

### Tertiary Targets (Nice to Have)
8. **Trade balance**: Exports/Imports ratio similar
9. **Government debt**: Growth rate similar
10. **Interest rate**: Level and volatility similar

---

## Practical Implementation

### Quick Calibration Script

```python
# calibrate_model.py

import pandas as pd
import numpy as np
from model import AustrianEconomyModel

# Load all targets
targets = {}
for var in ['nominal_gdp', 'real_gdp', 'nominal_gva', 'nominal_household_consumption']:
    targets[var] = pd.read_csv(f'data/calibration_{var}.csv')

# Grid search over key parameters
best_params = None
best_error = float('inf')

for markup in [1.10, 1.15, 1.20]:
    for mpc in [0.70, 0.75, 0.80]:
        for spread in [0.02, 0.03, 0.04]:
            
            # Run model
            model = AustrianEconomyModel(
                year=2013, quarter=2, seed=42, predictors=0,
                markup=markup, consumption_propensity=mpc, interest_spread=spread
            )
            results = model.run(steps=13)
            
            # Compute total error
            error = 0
            for var in targets:
                if var in results.columns:
                    error += np.sum((results[var] - targets[var]['mean'])**2)
            
            if error < best_error:
                best_error = error
                best_params = (markup, mpc, spread)
                print(f"New best: {best_params}, error={error:.2f}")

print(f"\nOptimal parameters: {best_params}")
```

---

## Expected Outcomes

After calibration, the Python model should:

1. **Replicate aggregate dynamics** from MATLAB within 5-10%
2. **Match distributional properties** (volatility, skewness)
3. **Preserve sectoral structure** from I-O matrix
4. **Generate realistic forecasts** for 13 quarters ahead

---

## Limitations & Caveats

1. **Agent heterogeneity**: MATLAB model may have more detailed agent types
2. **Network structure**: Python uses simplified matching vs. actual networks
3. **Computational scale**: MATLAB runs ~10,000 firms, Python starts with ~600
4. **Stochastic processes**: Random number generation may differ slightly

---

## Next Steps

1. ✅ Extract MATLAB calibration data (DONE)
2. ✅ Load I-O matrix (DONE)
3. ⏳ Implement calibration script
4. ⏳ Run grid search over parameters
5. ⏳ Validate against MATLAB distributions
6. ⏳ Document final calibrated parameters
7. ⏳ Create validation report

---

## References

- Poledna et al. (2022) - Original paper methodology
- MATLAB simulation outputs - 16,000 runs for validation
- Statistics Austria I-O tables - Structural calibration
