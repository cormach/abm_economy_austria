import numpy as np
import pandas as pd
from model import AustrianEconomyModel


def run_simulation(year, quarter, seed, predictors, n_steps=13):
    """Run single simulation"""
    model = AustrianEconomyModel(year, quarter, seed, predictors)
    results = model.run(steps=n_steps)
    return results


def run_parallel_simulations(year, quarter, base_seed, predictors, n_sims=100):
    """Run multiple simulations with different seeds"""
    all_results = []
    
    for i in range(n_sims):
        seed = base_seed + i
        print(f"Running simulation {i+1}/{n_sims} with seed {seed}")
        results = run_simulation(year, quarter, seed, predictors)
        results['simulation'] = i
        all_results.append(results)
    
    return pd.concat(all_results)


if __name__ == "__main__":
    year = 2024
    quarter = 1
    seed = 42
    predictors = 1
    
    # Single run
    print("Running single simulation...")
    results = run_simulation(year, quarter, seed, predictors)
    print("\nResults:")
    print(results)
    
    # Save results
    results.to_csv(f"results_{year}Q{quarter}_seed{seed}.csv")
    
    # Multiple runs for Monte Carlo
    # results_mc = run_parallel_simulations(year, quarter, seed, predictors, n_sims=10)
    # results_mc.to_csv(f"results_mc_{year}Q{quarter}.csv")
