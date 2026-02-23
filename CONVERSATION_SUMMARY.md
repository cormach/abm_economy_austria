# Conversation Summary

## Project Setup
- **Project**: C implementation of Austrian economy ABM based on Poledna et al. (2022), wants Python/Mesa rewrite. Extracted calibration data from 16,000 MATLAB files and I-O matrix from C code.

## Model Evolution (02-10)
Built 10 progressive model versions from initial Mesa implementation to government sector with emergent productivity exploration.

## Key Milestones

### Wage Scaling Fix
Discovered wages were in wrong units (monthly individual vs quarterly aggregate). Changed from 2000-3500 to 50-100, reducing error from 1108% to 0.8%.

### Capital Income (08)
Added operating surplus calculation. Changed productivity from physical output (0.2-0.4) to value multiplier (1.9). GDP error: 0.1%.

### Government Sector (09)
Implemented full tax system with Austrian empirical rates (income tax 21.3%, VAT 15.3%, social insurance 17.1%+21.2%). Corrected GDP calculation from mixing income/expenditure approaches. GDP error: 2.7%.

### Emergent Productivity (10)
Analyzed sectoral productivity distribution (0.01-12.67 range, aggregate 1.93). Created exploration plan to replicate C code's emergent productivity property.

## Files and Code Summary

- **`/Users/cormachollingsworth/projects/abm_economy_austria/src/mbu.c`**: 32,364 line C code with 100+ variables, contains I-O matrix, production function, tax calculations. Key formulas: `operating_surplus = sum(pi + interest - Y_cum)`, `GDP = sum(Y_) + tax_adjustments`, tax rates hard-coded (lines 25003-25006).
- **`python/data/io_matrix.csv`**: 62×62 I-O matrix, sum=67,272.83
- **`python/data/calibration_*.csv`**: 23 CSV files with mean/std/percentiles from 16,000 MATLAB runs. Key values: GDP=80,126, Compensation=38,306 (47.8%), Operating surplus=31,653 (39.5%), Taxes on production=1,240 (1.6%), Gov consumption=15,934 (19.9%)
- **`python/02_initial_model/`**: Original Mesa implementation with Firm/Household/Bank/Government agents. Bug: wages deducted but never credited to workers.
- **`python/06_labor_market/model_labor.py`**: Added labor market matching, production from labor, wage payments, consumption-production feedback loop. Unemployment 7.7%, error 92.4% with search_prob=0.3.
- **`python/07_io_linkages/model_io.py`**: Added I-O matrix intermediate demand. Initially had wrong GDP calculation (sales instead of value added).
- **`python/08_capital_income/model_capital.py`**: Production function: `output = labor_value × productivity`. Operating surplus: `0.82 × (output - wages)` (later corrected to 0.961). Best params: productivity=1.9, mpc=0.65, wage=88. GDP error: 0.1%.
- **`python/09_government/model_gov.py`**: Full tax system with empirical rates. Corrected operating surplus: `gross_surplus × 0.961`, taxes on production: `gross_surplus × 0.039`. GDP calculation: `wages + profits + taxes_on_production + taxes_on_products`. Best params: productivity=1.6, mpc=0.7, wage=50, search=0.5. GDP error: 2.7%.
- **`python/10_emergent_productivity/explore_productivity.py`**: Analysis showing empirical productivity=1.93 (not 1.6), sectoral range 0.01-12.67, median 1.007. Top sectors: 44 (12.67), 27 (7.64), 29 (7.49).

## Key Insights

- **INSIGHT**: Wage scale was wrong - should be quarterly aggregate (50-100) not monthly individual (2000-3500). This 34× difference caused 1108% error.
- **INSIGHT**: GDP ≠ GVA. Formula: `GDP = GVA + taxes_on_products` where `GVA = compensation + operating_surplus + taxes_on_production`. Gap is 8,533 (taxes on products like VAT).
- **INSIGHT**: Operating surplus calculation: 0.82 factor was wrong. Correct: `operating_surplus = 96.1% × (output - wages)`, `taxes_on_production = 3.9% × (output - wages)`.
- **INSIGHT**: Productivity multiplier is empirically fixed at 1.93 in Austrian data, not a calibrated parameter. It's an emergent property from sectoral structure.
- **INSIGHT**: C code uses Austrian empirical tax rates: income tax 21.3%, VAT 15.3%, social insurance firm 21.2%, social insurance worker 17.1%, corporate tax ~25%.
- **INSIGHT**: Sectoral productivity varies 127× (0.01 to 12.67). Aggregate productivity emerges from weighted average of sectoral productivities.
- **INSIGHT**: Model 09 error increased from 0.1% to 2.7% because model 08 was "lucky" - accidentally matched GDP by missing taxes. Model 09 is more accurate with proper accounting.

## Most Recent Topic

**Topic**: Exploring emergent productivity to replicate C code's approach where productivity=1.93 emerges from structure rather than being a parameter

**Progress**: 
1. Created `10_emergent_productivity/` directory
2. Analyzed sectoral productivity distribution from empirical data
3. Found aggregate productivity = 1.93, sectoral range 0.01-12.67, median 1.007
4. Identified that C code likely uses initial sectoral data directly rather than productivity parameter
5. Created comprehensive exploration plan to reverse-engineer C code's production function

**Tools Used**:
- **executeBash**: Created `10_emergent_productivity/` directory
- **fsWrite (explore_productivity.py)**: Created analysis script that loads sectoral GVA and compensation data, calculates sectoral productivity distribution, identifies top/bottom sectors, analyzes time series stability, generates visualization showing productivity distribution and sorted values, concludes empirical productivity is 1.93 not 1.6 (20.7% difference)
- **executeBash (run explore_productivity.py)**: Executed analysis showing: aggregate productivity=1.931, sectoral std=2.318, range 0.010-12.666, top sectors are 44/27/29, productivity stable over time (1.91-1.95), saved visualization to sectoral_productivity.png
- **fsWrite (model_emergent.py)**: Copied government model, added sectoral productivity loading from data (`sector_productivity = sector_output / (total_comp / n_sectors)`), modified production function to use sectoral productivity per firm's sector instead of single parameter
- **fsWrite (EXPLORATION_PLAN.md)**: Created systematic 7-step exploration plan: (1) find initial conditions in C code, (2) analyze production function, (3) test sectoral productivity in Python, (4) compare approaches, (5) reverse-engineer C code logic, (6) implement emergent productivity with 3 options (direct sectoral, Cobb-Douglas with sectoral TFP, bootstrap from initial conditions), (7) validate with tests for aggregate/sectoral/time series matching. Success criteria: <3% GDP error without calibrating productivity, match sectoral distribution, productivity emerges naturally.
