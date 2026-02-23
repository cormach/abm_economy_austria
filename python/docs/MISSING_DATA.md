# Missing Data Requirements

Based on the Poledna et al. (2022) paper appendices, the following data is needed:

## ✓ Already Extracted:
1. **Input-Output Matrix** (62×62) - Inter-sectoral transactions ✓
2. **Basic parameters** - Markup, tax rates, etc. ✓

## ❌ Missing Data (Need to Find):

### 1. **Initial Firm Distribution** (Appendix A)
- Number of firms per sector (62 sectors)
- Firm size distribution (employees, capital)
- Initial capital stock per sector
- Initial inventory levels
- **Source**: Austrian business register or WIFO data

### 2. **Labor Market Data** (Appendix B)
- Number of workers per sector
- Wage distribution by sector
- Unemployment rate (initial)
- Labor productivity by sector
- **Source**: Austrian labor force statistics

### 3. **Household Data** (Appendix D)
- Number of households
- Income distribution
- Wealth distribution
- Consumption patterns by income quintile
- **Source**: HFCS (Household Finance and Consumption Survey)

### 4. **Financial Data** (Appendix E)
- Bank balance sheets
- Firm leverage ratios by sector
- Interest rate spreads
- Default rates by sector
- **Source**: OeNB (Austrian National Bank)

### 5. **Government Data** (Appendix F)
- Government debt/GDP ratio
- Tax revenue by type (VAT, income, corporate)
- Government expenditure breakdown
- Social security contributions
- **Source**: Statistics Austria, Eurostat

### 6. **Time Series for Calibration**
- Quarterly GDP (1995-2020)
- Sectoral value added
- Unemployment rate
- Inflation (HICP)
- EURIBOR 3-month rate
- **Source**: Eurostat, ECB, Statistics Austria

### 7. **Behavioral Parameters** (Need Estimation)
- Consumption propensity by wealth quintile
- Firm markup by sector
- Wage adjustment speed
- Price adjustment speed
- Inventory target ratios
- **Source**: Econometric estimation from micro data

### 8. **Network Structure**
- Firm-firm trading network (who buys from whom)
- Bank-firm credit network
- **Source**: Derived from I-O matrix + firm-level data

## Priority Order:

### HIGH PRIORITY (Model won't run properly without):
1. Firm size distribution per sector
2. Employment per sector
3. Wage levels per sector
4. Initial capital stock

### MEDIUM PRIORITY (Affects realism):
5. Household wealth distribution
6. Bank balance sheet data
7. Government fiscal data

### LOW PRIORITY (Can use defaults):
8. Behavioral parameters (can calibrate)
9. Network structure (can generate)

## Suggested Data Sources:

1. **Statistics Austria** (www.statistik.at)
   - National accounts
   - Labor force survey
   - Business register

2. **OeNB** (www.oenb.at)
   - Financial stability reports
   - HFCS data
   - Credit statistics

3. **Eurostat**
   - Quarterly national accounts
   - Sectoral accounts
   - Government finance

4. **WIFO** (Austrian Institute of Economic Research)
   - Sectoral data
   - Economic forecasts

## Next Steps:

1. Contact Statistics Austria for firm-level data access
2. Download public Eurostat quarterly data
3. Request OeNB HFCS microdata (if available)
4. Use I-O matrix to estimate initial distributions as fallback
