# Austrian Economy ABM - Python/Mesa Implementation

Minimal Python rewrite of the Austrian economy agent-based model using Mesa framework.

## Installation

```bash
pip install -r requirements.txt
```

## Usage

```python
from model import AustrianEconomyModel

# Run simulation
model = AustrianEconomyModel(year=2024, quarter=1, seed=42, predictors=1)
results = model.run(steps=13)
print(results)
```

Or use the runner script:

```bash
python run_simulation.py
```

## Structure

- `model.py` - Main Mesa model class
- `agents.py` - Agent classes (Firm, Household, Bank, Government)
- `forecasting.py` - VAR/VARX forecasting module
- `run_simulation.py` - Simulation runner

## Key Differences from C Implementation

1. **Simplified agents**: Core economic behaviors without full C complexity
2. **Mesa framework**: Uses Mesa's scheduling and data collection
3. **NumPy/SciPy**: Replaces custom C linear algebra
4. **Modular design**: Easy to extend and modify

## Outputs

- GDP (nominal & real)
- Unemployment rate
- Inflation
- EURIBOR rate
- Sector-level data (via datacollector)
