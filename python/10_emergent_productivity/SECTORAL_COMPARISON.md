# Sectoral Implementation Comparison: C Code vs Python

## C Code Implementation

### Data Structure
- **Sectoral arrays**: 806 elements = 62 sectors × 13 time periods
- **Key arrays**:
  - `sector_firm_demand[806]`
  - `sector_firm_consumption[806]`
  - `sector_firm_K_formation[806]` (capital formation)
  - `sector_firm_K_depreciation[62]`
  - `sector_value_added[806]`
  - `sector_household_consumption[806]`
  - `sector_government_consumption[806]`

### Sectoral Assignment Approach
The C code uses **aggregate sectoral data** rather than individual firm-sector assignments:
- Sectors are tracked as **aggregate time series** (62 sectors × 13 periods)
- No explicit "firm_sector" array - firms are implicitly distributed
- Sectoral calculations use **indexing**: `[period + 13 * sector]`
- Data loaded from SQL imports (empirical Austrian data)

### Key Insight from C Code
**The C code does NOT assign individual firms to sectors**. Instead:
1. It works with **62 aggregate sectors** directly
2. Each sector has time series data (13 periods)
3. Firm-level behavior aggregates UP to sectoral totals
4. Productivity emerges from sectoral I-O relationships and initial conditions

## Python Implementation (Current)

### Data Structure
```python
self.n_firms = 620  # 10 firms per sector
self.firm_sector = np.array([...])  # Each firm assigned to 1 of 62 sectors
self.firm_output = np.zeros(n_firms)
self.firm_capital = np.zeros(n_firms)
```

### Sectoral Assignment Approach
```python
# Assign firms to sectors based on employment shares
sector_employment_share = initial_output / total_output
sector_firm_counts = np.round(sector_employment_share * n_firms).astype(int)
self.firm_sector = np.repeat(np.arange(self.n_sectors), sector_firm_counts)

# Initialize from sectoral data
for s in range(self.n_sectors):
    sector_firms = np.where(self.firm_sector == s)[0]
    if len(sector_firms) > 0:
        self.firm_output[sector_firms] = initial_output[s] / len(sector_firms)
        self.firm_capital[sector_firms] = sector_capital[s] / len(sector_firms)
```

## Key Differences

| Aspect | C Code | Python Code |
|--------|--------|-------------|
| **Firm-Sector Link** | Implicit (aggregate) | Explicit (firm_sector array) |
| **Granularity** | 62 sectors | 620 firms in 62 sectors |
| **Data Structure** | Sectoral time series [806] | Firm-level arrays [620] |
| **Initialization** | Direct sectoral data | Divide sectoral data by firm count |
| **Productivity** | Sectoral I-O relationships | Firm-level with sectoral productivity |

## Implications

### What C Code Does Right
1. **No artificial firm distribution** - works directly with sectoral aggregates
2. **Empirical fidelity** - uses actual Austrian sectoral data
3. **Emergent productivity** - arises from I-O matrix and sectoral structure
4. **Time series** - tracks 13 periods of sectoral evolution

### What Python Code Does Differently
1. **Micro-foundation** - explicit firm agents (Mesa framework)
2. **Distributional assumptions** - assumes uniform distribution within sectors
3. **Scalability** - can vary firm count independently
4. **Agent interactions** - firms can have heterogeneous behavior

## Recommendations

### Option 1: Hybrid Approach (Recommended)
Keep firm-level agents but initialize more carefully:
```python
# Use empirical sectoral distributions
sector_productivity = empirical_gva / empirical_compensation  # By sector
sector_capital_intensity = empirical_capital / empirical_output  # By sector
sector_employment_share = empirical_employment / total_employment  # By sector

# Assign firms proportionally
for s in range(n_sectors):
    sector_firms = np.where(self.firm_sector == s)[0]
    # Initialize with sectoral characteristics + small random variation
    self.firm_productivity[sector_firms] = sector_productivity[s] * np.random.normal(1, 0.1, len(sector_firms))
```

### Option 2: Pure Sectoral (Like C Code)
Eliminate firm_sector array entirely:
```python
# Work directly with 62 sectors
self.sector_output = initial_output  # [62]
self.sector_capital = initial_capital  # [62]
self.sector_employment = initial_employment  # [62]
self.sector_productivity = sector_output / sector_employment  # [62]
```

### Option 3: Calibrated Micro (Current + Validation)
Keep current approach but validate:
```python
# After initialization, check aggregate matches empirical
for s in range(n_sectors):
    sector_firms = np.where(self.firm_sector == s)[0]
    assert np.isclose(
        self.firm_output[sector_firms].sum(),
        initial_output[s],
        rtol=0.01
    )
```

## Conclusion

**The C code's "emergent productivity" comes from:**
1. Using empirical sectoral data directly (not dividing by firm count)
2. I-O matrix relationships between sectors
3. Time series evolution (13 periods)
4. No artificial micro-level distribution assumptions

**To replicate in Python:**
- Either work at sectoral level (like C code)
- Or carefully calibrate firm-level initialization to match sectoral aggregates
- Key: productivity = 1.93 should emerge from weighted sectoral productivities, not be a parameter
