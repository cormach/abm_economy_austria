import numpy as np
import pandas as pd
from multiprocessing import Pool, cpu_count
from functools import partial
from model import AustrianEconomyModel


def run_single_simulation(seed, year, quarter, predictors):
    """Run single simulation - must be top-level function for multiprocessing"""
    model = AustrianEconomyModel(year, quarter, seed, predictors)
    results = model.run(steps=13)
    results['seed'] = seed
    return results


def run_parallel_simulations(year, quarter, base_seed, predictors, n_sims=100, n_workers=None):
    """Run simulations in parallel using all CPU cores"""
    
    if n_workers is None:
        n_workers = cpu_count()
    
    print(f"Running {n_sims} simulations on {n_workers} cores...")
    
    # Create seeds
    seeds = [base_seed + i for i in range(n_sims)]
    
    # Partial function with fixed parameters
    run_func = partial(run_single_simulation, year=year, quarter=quarter, predictors=predictors)
    
    # Run in parallel
    with Pool(n_workers) as pool:
        results = pool.map(run_func, seeds)
    
    # Combine results
    return pd.concat(results, ignore_index=True)


def calibrate_parallel(param_grid, year=2013, quarter=2, n_runs=50):
    """Parallel parameter calibration"""
    
    def evaluate_params(params):
        markup, mpc, spread = params
        
        # Run multiple simulations with these parameters
        results = []
        for seed in range(n_runs):
            model = AustrianEconomyModel(year, quarter, seed, 0)
            model.params['MARKUP'] = markup
            model.params['PROPENSITY_TO_CONSUME'] = mpc
            model.params['INTEREST_RATE_SPREAD'] = spread
            
            df = model.run(steps=13)
            results.append(df)
        
        # Compute error vs targets
        all_results = pd.concat(results)
        mean_gdp = all_results.groupby(all_results.index)['nominal_gdp'].mean()
        
        target = pd.read_csv('data/calibration_nominal_gdp.csv')
        error = np.sum((mean_gdp.values - target['mean'].values)**2)
        
        return params, error
    
    # Parallel evaluation
    n_workers = cpu_count()
    print(f"Calibrating on {n_workers} cores...")
    
    with Pool(n_workers) as pool:
        results = pool.map(evaluate_params, param_grid)
    
    # Find best
    best_params, best_error = min(results, key=lambda x: x[1])
    return best_params, best_error


if __name__ == "__main__":
    import time
    
    # Test parallel execution
    year = 2013
    quarter = 2
    seed = 42
    predictors = 0
    n_sims = 100
    
    print(f"Available CPU cores: {cpu_count()}")
    
    # Sequential baseline
    print("\nSequential execution...")
    start = time.time()
    results_seq = []
    for i in range(10):
        model = AustrianEconomyModel(year, quarter, seed + i, predictors)
        results_seq.append(model.run(steps=13))
    seq_time = time.time() - start
    print(f"10 runs: {seq_time:.2f}s ({seq_time/10:.2f}s per run)")
    
    # Parallel
    print("\nParallel execution...")
    start = time.time()
    results_par = run_parallel_simulations(year, quarter, seed, predictors, n_sims=10)
    par_time = time.time() - start
    print(f"10 runs: {par_time:.2f}s ({par_time/10:.2f}s per run)")
    print(f"Speedup: {seq_time/par_time:.2f}x")
    
    # Full run
    print(f"\nRunning {n_sims} simulations in parallel...")
    start = time.time()
    results = run_parallel_simulations(year, quarter, seed, predictors, n_sims=n_sims)
    elapsed = time.time() - start
    
    print(f"Completed in {elapsed:.2f}s ({elapsed/n_sims:.3f}s per run)")
    print(f"Results shape: {results.shape}")
    
    # Save
    results.to_csv(f'results_parallel_{year}Q{quarter}_{n_sims}runs.csv', index=False)
    print(f"Saved: results_parallel_{year}Q{quarter}_{n_sims}runs.csv")
