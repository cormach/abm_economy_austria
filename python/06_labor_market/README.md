# Labor Market Model

## Overview

This directory contains the development of an agent-based economy model with an **explicit labor market** where workers and firms match, production depends on employment, and consumption feeds back into demand.

## Key Innovation

Previous models (hybrid, vectorized) had **100% unemployment** because there was no labor market mechanism. This model adds:

1. **Labor market matching**: Unemployed workers → firms with vacancies
2. **Production from labor**: Output = labor × productivity
3. **Wage payments**: Employed workers receive income
4. **Consumption-production feedback loop**: Sales → expected demand → hiring → production

## Files

### `model_labor.py`
Core economic model with feedback loop:

```python
# Feedback loop sequence:
1. labor_market()     # Match workers to firms based on expected_demand
2. production()       # Firms produce: output = labor × productivity
3. sales()            # Firms sell to households, update inventory
4. wages()            # Pay employed workers
5. consumption()      # Households spend income (MPC)
# → Next period: expected_demand = 0.9 × sales + 0.1 × expected_demand
```

**Key mechanisms:**
- **Adaptive expectations**: `expected_demand = 0.9 * sales + 0.1 * expected_demand`
- **Inventory buffer**: Production → inventory → sales (can differ)
- **Demand distribution**: Consumption allocated to firms proportional to inventory
- **Initial conditions**: Sectoral output from `calibration_nominal_sector_gva.csv`

### `calibrate.py`
3-stage parallel calibration (replicates hybrid strategy):

1. **Stage 1 - Coarse grid**: 27 combinations (3×3×3), 50 runs each
2. **Stage 2 - Fine grid**: 125 combinations around best, 100 runs each  
3. **Stage 3 - Validation**: 1000 runs with best parameters

**Parameters calibrated:**
- `productivity`: Output per worker (0.2-0.8)
- `mpc`: Marginal propensity to consume (0.65-0.80)
- `wage`: Monthly wage per worker (2000-3500)

## Results

### Calibration Performance
- **Total time**: ~10 seconds (1.2s + 6.3s + 2.6s)
- **Simulations**: 27×50 + 125×100 + 1×1000 = 14,850 runs
- **Parallelization**: 8 CPU cores

### Best Parameters
```
productivity = 0.200
mpc = 0.650
wage = 2000
```

### Model Accuracy
- **Model GDP**: 800
- **Target GDP**: 80,126
- **Relative Error**: 99.0%

## Why Error is Still High

The model produces output but error remains ~99% because:

1. **Missing sectoral linkages**: No I-O matrix flows between firms
2. **No price mechanism**: All goods treated identically
3. **No capital/investment**: Only labor input
4. **No government/banks**: Missing fiscal/monetary policy
5. **No trade**: No exports/imports
6. **Scale mismatch**: 620 firms vs Austrian economy

## Comparison to Previous Models

| Model | Unemployment | GDP | Error | Key Issue |
|-------|-------------|-----|-------|-----------|
| Hybrid (03) | 100% | 0 | 100% | No labor market |
| Labor (06) | ~5% | 800 | 99% | No sectoral flows |

## Next Steps

To reduce error below 50%, add:

1. **I-O linkages**: Firms buy inputs from other sectors using I-O matrix
2. **Sectoral prices**: Different prices per sector
3. **Capital accumulation**: Investment → capital stock → production capacity
4. **Government**: Taxes, transfers, public consumption
5. **Banks**: Credit supply for investment
6. **Trade**: Exports/imports based on competitiveness

## Data Sources

- **I-O Matrix**: `../data/io_matrix.csv` (62×62 sectoral transactions)
- **Initial Output**: `../data/calibration_nominal_sector_gva.csv` (period 0)
- **Target GDP**: `../data/calibration_nominal_gdp.csv` (13 periods)

## Usage

```bash
# Run single simulation
python model_labor.py

# Run calibration
python calibrate.py

# Results saved to:
# - calibration_results.csv (all parameter combinations)
# - best_results.csv (time series with best params)
# - validation_results.csv (1000-run validation)
```

## Key Insight

The **feedback loop** is critical:
- Without it: Output decays to zero (100% error)
- With it: Output stabilizes around initial level (99% error)

The remaining error requires adding the full economic structure (I-O flows, capital, government, banks, trade).
