# How the Input-Output Matrix is Used in the ABM

## Overview
The 62×62 I-O matrix represents inter-sectoral transactions in the Austrian economy.
Each cell [i,j] shows how much sector j purchases from sector i as intermediate inputs.

## Key Uses in mbu.c:

### 1. **Supply Chain Network** (normc.c)
```python
# Normalize columns to get technical coefficients
io_normalized = io_matrix / io_matrix.sum(axis=0)
# Each column shows input requirements per unit of output
```

### 2. **Demand Propagation**
```python
# Firm demand for inputs from other sectors
sector_demand = io_normalized @ sector_output
# If sector j produces 100 units, it needs inputs from all sectors i
```

### 3. **Material Costs**
```python
# Calculate intermediate consumption costs
material_costs = (io_normalized.T @ prices) * production
# Firms buy inputs at current prices
```

### 4. **Sector Interdependencies**
```python
# Identify critical sectors (high connectivity)
sector_importance = io_matrix.sum(axis=0) + io_matrix.sum(axis=1)
# Sectors with high values are economic hubs
```

### 5. **Supply Chain Disruptions**
```python
# Simulate shock to sector k
disrupted_io = io_matrix.copy()
disrupted_io[:, k] *= 0.5  # 50% reduction in sector k
# Propagates through supply chains
```

## In the Model Flow:

1. **Initialization**: Load I-O matrix to define sector structure
2. **Production**: Firms use I-O coefficients to determine input needs
3. **Trade**: Inter-firm transactions follow I-O patterns
4. **Prices**: Input costs affect output prices via I-O linkages
5. **Shocks**: Disruptions propagate through I-O network

## Example Usage:
```python
import pandas as pd
import numpy as np

# Load I-O matrix
io_matrix = pd.read_csv('data/io_matrix.csv', index_col=0)

# Technical coefficients (input per unit output)
A = io_matrix / io_matrix.sum(axis=0)

# Leontief inverse (total requirements)
I = np.eye(62)
L = np.linalg.inv(I - A)

# If final demand = 100 for sector 0, total output needed:
final_demand = np.zeros(62)
final_demand[0] = 100
total_output = L @ final_demand
```

## Key Insight:
The I-O matrix captures the **real structure** of the Austrian economy,
making the ABM empirically grounded rather than purely theoretical.
