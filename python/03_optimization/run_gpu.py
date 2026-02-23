"""
GPU-accelerated ABM using JAX for vectorized operations
Requires: pip install jax jaxlib
"""

import jax
import jax.numpy as jnp
from jax import jit, vmap
import numpy as np
import pandas as pd


@jit
def compute_firm_production(capital, labor, capital_prod=0.5, labor_prod=1.0):
    """Vectorized firm production (Leontief)"""
    return jnp.minimum(capital * capital_prod, labor * labor_prod)


@jit
def compute_prices(unit_costs, markup=1.15):
    """Vectorized price setting"""
    return markup * unit_costs


@jit
def compute_consumption(income, wealth, mpc=0.75, wealth_effect=0.05):
    """Vectorized household consumption"""
    return mpc * (income + wealth_effect * wealth)


@jit
def update_sector_aggregates(firm_outputs, firm_prices, sector_ids, n_sectors=62):
    """Aggregate firm-level to sector-level using GPU"""
    sector_output = jnp.zeros(n_sectors)
    sector_prices = jnp.zeros(n_sectors)
    
    for s in range(n_sectors):
        mask = sector_ids == s
        sector_output = sector_output.at[s].set(jnp.sum(firm_outputs * mask))
        sector_prices = sector_prices.at[s].set(jnp.mean(jnp.where(mask, firm_prices, 0)))
    
    return sector_output, sector_prices


class GPUAcceleratedModel:
    """Vectorized model using JAX for GPU acceleration"""
    
    def __init__(self, n_firms=620, n_households=1000, n_sectors=62):
        self.n_firms = n_firms
        self.n_households = n_households
        self.n_sectors = n_sectors
        
        # Initialize on GPU
        self.firm_capital = jnp.ones(n_firms) * 100.0
        self.firm_labor = jnp.zeros(n_firms)
        self.firm_sectors = jnp.array([i % n_sectors for i in range(n_firms)])
        
        self.household_wealth = jnp.ones(n_households) * 100.0
        self.household_income = jnp.zeros(n_households)
        
        self.prices = jnp.ones(n_sectors)
        
    @partial(jit, static_argnums=(0,))
    def step(self, state):
        """Single time step - JIT compiled for GPU"""
        firm_capital, firm_labor, household_wealth, household_income, prices = state
        
        # Firm production (vectorized)
        production = compute_firm_production(firm_capital, firm_labor)
        
        # Pricing (vectorized)
        unit_costs = jnp.ones(self.n_firms)  # Simplified
        firm_prices = compute_prices(unit_costs)
        
        # Household consumption (vectorized)
        consumption = compute_consumption(household_income, household_wealth)
        
        # Update wealth
        household_wealth = household_wealth + household_income - consumption
        
        # Aggregate to sectors
        sector_output, sector_prices = update_sector_aggregates(
            production, firm_prices, self.firm_sectors, self.n_sectors
        )
        
        return (firm_capital, firm_labor, household_wealth, household_income, sector_prices)
    
    def run(self, steps=13):
        """Run simulation on GPU"""
        state = (
            self.firm_capital,
            self.firm_labor,
            self.household_wealth,
            self.household_income,
            self.prices
        )
        
        results = []
        for t in range(steps):
            state = self.step(state)
            
            # Extract results
            _, _, wealth, _, prices = state
            gdp = jnp.sum(wealth) * 0.1  # Simplified
            
            results.append({
                'period': t,
                'nominal_gdp': float(gdp),
                'mean_price': float(jnp.mean(prices))
            })
        
        return pd.DataFrame(results)


def run_gpu_batch(n_simulations=100):
    """Run multiple simulations in batch on GPU"""
    
    # Vectorize over simulations
    def run_one(seed):
        jax.random.PRNGKey(seed)
        model = GPUAcceleratedModel()
        return model.run(steps=13)
    
    # Use vmap to parallelize over seeds
    seeds = jnp.arange(n_simulations)
    
    # This runs all simulations in parallel on GPU
    results = vmap(run_one)(seeds)
    
    return results


if __name__ == "__main__":
    import time
    
    print("Checking JAX device...")
    print(f"JAX devices: {jax.devices()}")
    
    # Test GPU acceleration
    print("\nTesting GPU-accelerated model...")
    
    model = GPUAcceleratedModel(n_firms=620, n_households=1000)
    
    # Warmup (JIT compilation)
    print("Warming up JIT...")
    _ = model.run(steps=1)
    
    # Benchmark
    print("\nBenchmarking...")
    start = time.time()
    results = model.run(steps=13)
    elapsed = time.time() - start
    
    print(f"Single run: {elapsed:.4f}s")
    print(results)
    
    # Batch processing
    print("\nBatch processing 100 simulations...")
    start = time.time()
    batch_results = run_gpu_batch(n_simulations=100)
    elapsed = time.time() - start
    
    print(f"100 runs: {elapsed:.2f}s ({elapsed/100:.4f}s per run)")
