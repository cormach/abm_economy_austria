# Performance Optimization Guide

## Mac GPU Acceleration Options

### Option 1: CPU Parallelization (Easiest) ✓

**Use multiprocessing to run simulations in parallel**

```bash
# Install (already have)
pip install numpy pandas

# Run
python run_parallel.py
```

**Expected speedup**: 8-10x on M1/M2 Mac (8-10 cores)

**Pros**:
- No additional dependencies
- Works immediately
- Perfect for Monte Carlo simulations

**Cons**:
- Doesn't use GPU
- Each simulation runs independently

---

### Option 2: JAX GPU Acceleration (Advanced)

**Use JAX to vectorize operations on Apple Silicon GPU**

```bash
# Install JAX for Mac
pip install jax jaxlib

# Run
python run_gpu.py
```

**Expected speedup**: 50-100x for vectorized operations

**Pros**:
- Uses Metal GPU on M1/M2
- JIT compilation
- Vectorized operations

**Cons**:
- Requires code refactoring
- ABM logic harder to vectorize
- Learning curve

---

### Option 3: Hybrid Approach (Recommended)

**Combine CPU parallelization + NumPy vectorization**

```python
# Vectorize agent operations
import numpy as np

# Instead of loops
for firm in firms:
    firm.produce()

# Use vectorized operations
firm_outputs = capital ** 0.3 * labor ** 0.7  # All firms at once
```

**Expected speedup**: 20-30x

---

## Practical Implementation

### 1. CPU Parallel (Immediate Use)

```bash
cd python
source ../.venv/bin/activate
python run_parallel.py
```

This will:
- Use all CPU cores (8-10 on Mac)
- Run 100 simulations in ~1-2 minutes
- Save aggregated results

### 2. Optimize Single Run

**Current bottlenecks**:
```python
# Slow: List comprehensions
firms = [a for a in self.schedule.agents if isinstance(a, Firm)]
for firm in firms:
    firm.step()

# Fast: Vectorized
firm_capitals = np.array([f.capital for f in firms])
firm_outputs = firm_capitals ** 0.3 * firm_labor ** 0.7
```

### 3. Profile Code

```python
import cProfile
import pstats

# Profile single run
cProfile.run('model.run(steps=13)', 'profile_stats')

# View results
p = pstats.Stats('profile_stats')
p.sort_stats('cumulative').print_stats(20)
```

---

## Benchmarks (Estimated)

### Current Implementation
- Single run: ~5-10 seconds
- 100 runs sequential: ~8-15 minutes
- 1000 runs sequential: ~1.5-2.5 hours

### With CPU Parallelization (8 cores)
- Single run: ~5-10 seconds
- 100 runs parallel: ~1-2 minutes ✓
- 1000 runs parallel: ~10-20 minutes ✓

### With GPU Acceleration (if fully vectorized)
- Single run: ~0.1-0.5 seconds
- 100 runs batch: ~5-10 seconds
- 1000 runs batch: ~1-2 minutes

---

## Quick Start

### Run 100 simulations in parallel:

```bash
cd python
source ../.venv/bin/activate
python run_parallel.py
```

### Run calibration in parallel:

```python
from run_parallel import calibrate_parallel

# Define parameter grid
param_grid = [
    (1.10, 0.70, 0.02),
    (1.15, 0.75, 0.03),
    (1.20, 0.80, 0.04),
]

# Calibrate using all cores
best_params, error = calibrate_parallel(param_grid, n_runs=50)
print(f"Best parameters: {best_params}")
```

---

## Advanced: Vectorize Agent Operations

```python
# agents_vectorized.py

import numpy as np

class VectorizedFirms:
    """All firms as numpy arrays"""
    
    def __init__(self, n_firms, n_sectors):
        self.n = n_firms
        self.capital = np.ones(n_firms) * 100.0
        self.labor = np.zeros(n_firms)
        self.sectors = np.arange(n_firms) % n_sectors
        self.prices = np.ones(n_firms)
        
    def produce(self):
        """Vectorized production for all firms"""
        return np.minimum(
            self.capital * 0.5,
            self.labor * 1.0
        )
    
    def set_prices(self, markup=1.15):
        """Vectorized pricing"""
        unit_costs = self.labor / np.maximum(self.produce(), 1)
        self.prices = markup * unit_costs
```

---

## Memory Optimization

For large-scale runs (10,000+ agents):

```python
# Use float32 instead of float64
firm_capital = np.ones(n_firms, dtype=np.float32)

# Pre-allocate arrays
results = np.zeros((n_steps, n_variables), dtype=np.float32)

# Use generators instead of lists
firms = (a for a in agents if isinstance(a, Firm))
```

---

## Recommended Workflow

1. **Development**: Single runs with small agent counts
2. **Testing**: 10 parallel runs to verify
3. **Calibration**: 100-1000 parallel runs with parameter grid
4. **Production**: 10,000+ runs for final results

---

## Installation

```bash
# Basic (CPU parallel)
pip install numpy pandas

# Advanced (GPU)
pip install jax jaxlib  # For M1/M2 Mac

# Profiling
pip install line_profiler memory_profiler
```

---

## Expected Performance

**Mac M1/M2 (8-10 cores)**:
- CPU parallel: 8-10x speedup ✓
- With vectorization: 20-30x speedup
- With GPU (JAX): 50-100x speedup (for vectorizable operations)

**Calibration time**:
- 100 parameter combinations × 50 runs = 5,000 simulations
- Sequential: ~7 hours
- Parallel (8 cores): ~50 minutes ✓
- GPU-accelerated: ~10 minutes (if fully vectorized)
