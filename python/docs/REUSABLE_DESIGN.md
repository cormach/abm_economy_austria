# Reusable Economic ABM Framework

## Design Philosophy

Create **generic base classes** that can be configured for any economy, rather than hard-coding country-specific logic.

---

## Architecture

```
economic_base_classes.py (Generic)
├── EconomicAgent (Abstract base)
├── Firm (Reusable)
├── Household (Reusable)
├── Bank (Reusable)
├── Government (Reusable)
└── EconomicModel (Reusable)

Country-Specific Configurations
├── austria_config.py
├── us_config.py
├── uk_config.py
└── custom_config.py
```

---

## Key Design Patterns

### 1. **Parameterization over Hard-coding**

**Bad** (Hard-coded):
```python
class AustrianFirm(Agent):
    def __init__(self, unique_id, model):
        self.markup = 1.15  # Hard-coded for Austria
        self.vat_rate = 0.20  # Hard-coded
```

**Good** (Parameterized):
```python
class Firm(Agent):
    def __init__(self, unique_id, model, **kwargs):
        self.markup = kwargs.get('markup', 1.15)  # Default, but configurable
        self.capital_share = kwargs.get('capital_share', 0.3)
```

### 2. **Composition over Inheritance**

**Bad** (Deep inheritance):
```python
class AustrianFirm(EuropeanFirm):
    class EuropeanFirm(Firm):
        class Firm(Agent):
```

**Good** (Flat with configuration):
```python
class Firm(EconomicAgent):
    # Single class, configured via parameters
    pass

# Usage
austrian_firm = Firm(id, model, **austria_params)
us_firm = Firm(id, model, **us_params)
```

### 3. **Data Injection**

**Bad** (Embedded data):
```python
class AustrianModel(Model):
    def __init__(self):
        self.io_matrix = [[...]]  # Embedded Austrian I-O matrix
```

**Good** (Injected data):
```python
class EconomicModel(Model):
    def __init__(self, io_matrix=None):
        self.io_matrix = io_matrix  # Inject any country's I-O matrix
```

### 4. **Configuration Files**

**Structure**:
```python
def load_country_config(country):
    return {
        'io_matrix': 'path/to/data.csv',
        'n_sectors': 62,
        'firm_params': {...},
        'household_params': {...},
        'government_params': {...}
    }
```

---

## Usage Examples

### Example 1: Austrian Economy

```python
from economic_base_classes import EconomicModel, load_country_config
import pandas as pd

# Load Austrian configuration
config = load_country_config('Austria')
io_matrix = pd.read_csv(config['io_matrix'], index_col=0).values

# Create model
model = EconomicModel(
    country='Austria',
    io_matrix=io_matrix,
    n_firms=620,
    n_households=1000,
    parameters=config
)

# Run
results = model.run(steps=13)
```

### Example 2: US Economy

```python
# Load US configuration
config = load_country_config('US')
io_matrix = pd.read_csv(config['io_matrix'], index_col=0).values

# Create model (same class, different config!)
model = EconomicModel(
    country='US',
    io_matrix=io_matrix,
    n_firms=1000,
    n_households=5000,
    parameters=config
)

results = model.run(steps=13)
```

### Example 3: Custom Economy

```python
# Define custom parameters
custom_config = {
    'firm_params': {
        'markup': 1.25,
        'capital_share': 0.4,
    },
    'household_params': {
        'mpc': 0.80,
        'wealth_effect': 0.10,
    },
    'government_params': {
        'vat_rate': 0.15,
        'income_tax_rate': 0.30,
    }
}

# Create model
model = EconomicModel(
    country='MyCountry',
    io_matrix=my_io_matrix,
    parameters=custom_config
)
```

---

## Country-Specific Configuration Files

### austria_config.py

```python
AUSTRIA_CONFIG = {
    'io_matrix': 'data/io_matrix.csv',
    'n_sectors': 62,
    
    'firm_params': {
        'markup': 1.15,
        'capital_share': 0.3,
        'labor_share': 0.7,
        'initial_capital': 100.0,
    },
    
    'household_params': {
        'mpc': 0.75,
        'wealth_effect': 0.05,
        'initial_wealth': 100.0,
    },
    
    'bank_params': {
        'max_leverage': 3.0,
        'interest_spread': 0.03,
        'initial_equity': 100.0,
    },
    
    'government_params': {
        'vat_rate': 0.20,
        'income_tax_rate': 0.25,
        'corporate_tax_rate': 0.25,
        'ub_replacement_rate': 0.5,
    }
}
```

### us_config.py

```python
US_CONFIG = {
    'io_matrix': 'data/us_io_matrix.csv',
    'n_sectors': 71,
    
    'firm_params': {
        'markup': 1.20,
        'capital_share': 0.35,  # Higher capital share
        'labor_share': 0.65,
    },
    
    'household_params': {
        'mpc': 0.70,  # Lower MPC
        'wealth_effect': 0.08,  # Higher wealth effect
    },
    
    'government_params': {
        'vat_rate': 0.0,  # No federal VAT
        'income_tax_rate': 0.22,
        'corporate_tax_rate': 0.21,
    }
}
```

---

## Extending for New Economies

### Step 1: Prepare Data

```python
# 1. Get I-O matrix for your country
io_matrix = load_io_matrix('path/to/country_io.csv')

# 2. Estimate parameters from data
markup = estimate_markup(country_data)
mpc = estimate_mpc(country_data)
```

### Step 2: Create Configuration

```python
MY_COUNTRY_CONFIG = {
    'io_matrix': 'data/my_country_io.csv',
    'n_sectors': 50,
    
    'firm_params': {
        'markup': 1.18,
        # ... other params
    },
    
    'household_params': {
        'mpc': 0.78,
        # ... other params
    },
    
    'government_params': {
        'vat_rate': 0.18,
        # ... other params
    }
}
```

### Step 3: Run Model

```python
model = EconomicModel(
    country='MyCountry',
    io_matrix=io_matrix,
    parameters=MY_COUNTRY_CONFIG
)

results = model.run(steps=13)
```

---

## Advanced: Custom Agent Behaviors

If you need country-specific behaviors, extend the base classes:

```python
from economic_base_classes import Firm

class GermanFirm(Firm):
    """German firms with codetermination"""
    
    def __init__(self, unique_id, model, **kwargs):
        super().__init__(unique_id, model, **kwargs)
        self.worker_council = True
    
    def set_price(self):
        """German-specific pricing with worker input"""
        super().set_price()
        
        # Adjust for worker council influence
        if self.worker_council and self.labor > 10:
            self.price *= 0.98  # Slight reduction
```

---

## Benefits of This Design

### 1. **Reusability**
- Same code for Austria, US, UK, etc.
- No duplication

### 2. **Maintainability**
- Fix bug once, applies to all countries
- Update algorithm once

### 3. **Comparability**
- Same model structure across countries
- Fair comparisons

### 4. **Extensibility**
- Easy to add new countries
- Easy to add new agent types

### 5. **Testability**
- Test base classes once
- Test configurations separately

---

## File Structure

```
project/
├── economic_base_classes.py      # Generic classes
├── configs/
│   ├── austria_config.py
│   ├── us_config.py
│   ├── uk_config.py
│   └── germany_config.py
├── data/
│   ├── austria_io_matrix.csv
│   ├── us_io_matrix.csv
│   └── uk_io_matrix.csv
├── models/
│   ├── austria_model.py          # Uses base classes + config
│   ├── us_model.py
│   └── comparative_model.py      # Compare countries
└── tests/
    ├── test_base_classes.py
    └── test_configs.py
```

---

## Comparative Analysis Example

```python
from economic_base_classes import EconomicModel, load_country_config

# Run multiple countries
countries = ['Austria', 'US', 'UK']
results = {}

for country in countries:
    config = load_country_config(country)
    io_matrix = load_io_matrix(config['io_matrix'])
    
    model = EconomicModel(
        country=country,
        io_matrix=io_matrix,
        parameters=config
    )
    
    results[country] = model.run(steps=13)

# Compare
import matplotlib.pyplot as plt

for country, df in results.items():
    plt.plot(df.index, df['GDP'], label=country)

plt.legend()
plt.title('GDP Comparison Across Countries')
plt.savefig('country_comparison.png')
```

---

## Best Practices

1. **Keep base classes generic** - No country-specific logic
2. **Use kwargs extensively** - Maximum flexibility
3. **Provide sensible defaults** - Model works out-of-box
4. **Document parameters** - Clear what each does
5. **Validate configurations** - Check for required parameters
6. **Version configurations** - Track parameter changes
7. **Test thoroughly** - Base classes must be robust

---

## Migration Path

### From Country-Specific to Generic

**Before** (Austria-specific):
```python
class AustrianEconomyModel(Model):
    def __init__(self):
        self.vat_rate = 0.20
        self.io_matrix = load_austrian_io()
        # ... hard-coded Austrian logic
```

**After** (Generic):
```python
class EconomicModel(Model):
    def __init__(self, country, io_matrix, parameters):
        self.country = country
        self.io_matrix = io_matrix
        self.vat_rate = parameters['government_params']['vat_rate']
        # ... generic logic
```

**Usage**:
```python
# Austria
austria_model = EconomicModel('Austria', austria_io, austria_config)

# US
us_model = EconomicModel('US', us_io, us_config)
```

---

## Summary

**Key Principles**:
1. ✓ Generic base classes
2. ✓ Configuration over code
3. ✓ Data injection
4. ✓ Parameterization
5. ✓ Composition over inheritance

**Result**: One codebase, many economies!
