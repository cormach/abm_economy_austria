# Model Comparison: 04_calibration vs 06_labor_market

## Key Differences

| Feature | 04_calibration (Hybrid) | 06_labor_market |
|---------|------------------------|-----------------|
| **Labor Market** | ❌ No matching | ✅ Worker-firm matching |
| **Employment** | Static (100% unemployed) | Dynamic (0% unemployed) |
| **Production** | Firms produce arbitrarily | Production = labor × productivity |
| **Wages** | No wage payments | Wages paid to employed workers |
| **Income** | Households have no income | Income from employment |
| **Feedback Loop** | ❌ None | ✅ Sales → expected demand → hiring |
| **Inventory** | ❌ No inventory | ✅ Production → inventory → sales |
| **Initial Conditions** | Random/zero | Loaded from sectoral GVA data |
| **GDP Calculation** | Consumption (always zero) | Sales (actual transactions) |

## Detailed Mechanism Comparison

### 04_calibration (Hybrid Model)

```python
step():
  1. firms.produce()           # Produce arbitrary amounts
  2. firms.set_prices()        # Set prices with markup
  3. households.consume()      # Try to consume (but no income!)
  4. households.update_wealth()
  → GDP = sum(consumption) = 0  # No income → no consumption
```

**Problem**: No connection between production and consumption. Households have no income source.

**Result**: 
- Unemployment: 100%
- GDP: 0
- Error: 100%

---

### 06_labor_market

```python
step():
  1. labor_market()            # Match workers to firms
     - Firms post vacancies based on expected_demand
     - Unemployed workers fill vacancies
     
  2. production()              # Produce using labor
     - output = labor × productivity
     - inventory += output
     
  3. sales()                   # Sell to households
     - Distribute consumption across firms
     - sales = min(demand, inventory)
     - inventory -= sales
     - expected_demand = 0.9 × sales + 0.1 × expected_demand
     
  4. wages()                   # Pay employed workers
     - income = wage (if employed)
     
  5. consumption()             # Households spend
     - consumption = income × mpc
     
  → GDP = sum(sales)           # Actual transactions
```

**Solution**: Complete feedback loop connects all parts of the economy.

**Result**:
- Unemployment: 0%
- GDP: 6,253
- Error: 92.2%

## The Feedback Loop

```
Expected Demand
      ↓
   Vacancies
      ↓
    Hiring
      ↓
  Production
      ↓
  Inventory
      ↓
    Sales ←─────┐
      ↓         │
    Wages       │
      ↓         │
   Income       │
      ↓         │
 Consumption    │
      ↓         │
    Demand ─────┘
```

**Critical insight**: `expected_demand = 0.9 × sales + 0.1 × expected_demand`

This adaptive expectation creates persistence and stability.

## Code Structure Comparison

### Hybrid Model (04)
- Uses external `VectorizedFirms` and `VectorizedHouseholds` classes
- Minimal state tracking
- No explicit connections between agents
- ~70 lines of code

### Labor Market Model (06)
- Self-contained (no external agent classes)
- Explicit state: labor, inventory, sales, expected_demand
- Direct worker-firm relationships (hh_employer array)
- Initialized from real data (sectoral GVA)
- ~150 lines of code

## Performance

| Metric | Hybrid (04) | Labor Market (06) |
|--------|-------------|-------------------|
| Calibration time | 27s | 48s |
| Simulations | 14,850 | 14,850 |
| Speed per run | 0.002s | 0.003s |
| Error | 100% | 92.2% |

Labor market model is **1.5× slower** but **8 percentage points more accurate**.

## What's Still Missing (Both Models)

Neither model includes:
1. **I-O linkages**: Firms buying from other firms
2. **Capital**: Investment and capital accumulation
3. **Banks**: Credit supply
4. **Government**: Taxes, transfers, spending
5. **Trade**: Exports and imports
6. **Prices**: Sectoral price differences
7. **Heterogeneity**: Firm/household differences beyond sector

## Next Development Step

To reduce error below 50%, add **I-O linkages**:

```python
def intermediate_demand():
    """Firms buy inputs from other sectors"""
    for firm in firms:
        sector = firm.sector
        for input_sector in range(n_sectors):
            input_needed = io_matrix[sector, input_sector] * firm.output
            # Buy from firms in input_sector
```

This would create the full production network described in Poledna et al. (2022).
