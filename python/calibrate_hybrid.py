import numpy as np
import pandas as pd
from multiprocessing import Pool, cpu_count
from functools import partial
from model_hybrid import HybridEconomyModel
import time


def evaluate_params(params, year, quarter, n_runs, targets):
    """Evaluate single parameter set with multiple runs"""
    markup, mpc, spread = params
    
    # Run multiple simulations
    results = []
    for seed in range(n_runs):
        model = HybridEconomyModel(year, quarter, seed, 0, 
                                   markup=markup, mpc=mpc, spread=spread)
        df = model.run(steps=13)
        results.append(df)
    
    # Aggregate
    all_results = pd.concat(results)
    mean_results = all_results.groupby('period').mean()
    
    # Compute error vs targets
    total_error = 0
    weights = {'nominal_gdp': 1.0, 'nominal_gva': 1.0, 'unemployment': 10.0}
    
    for var, weight in weights.items():
        if var in targets and var in mean_results.columns:
            target_mean = targets[var]['mean'].values
            model_mean = mean_results[var].values
            error = np.sum((model_mean - target_mean) ** 2)
            total_error += weight * error
    
    return params, total_error, mean_results


def calibrate_parallel(param_grid, year=2013, quarter=2, n_runs=50, n_workers=None):
    """Parallel calibration using all CPU cores"""
    
    if n_workers is None:
        n_workers = cpu_count()
    
    print(f"Calibrating on {n_workers} cores...")
    print(f"Parameter combinations: {len(param_grid)}")
    print(f"Runs per combination: {n_runs}")
    print(f"Total simulations: {len(param_grid) * n_runs}")
    
    # Load targets
    targets = {}
    for var in ['nominal_gdp', 'nominal_gva']:
        try:
            targets[var] = pd.read_csv(f'data/calibration_{var}.csv')
        except:
            print(f"Warning: Could not load {var} target")
    
    # Partial function
    eval_func = partial(evaluate_params, 
                       year=year, quarter=quarter, 
                       n_runs=n_runs, targets=targets)
    
    # Parallel evaluation
    start = time.time()
    with Pool(n_workers) as pool:
        results = pool.map(eval_func, param_grid)
    
    elapsed = time.time() - start
    
    # Find best
    best_params, best_error, best_results = min(results, key=lambda x: x[1])
    
    print(f"\nCalibration completed in {elapsed:.1f}s")
    print(f"Best parameters: markup={best_params[0]:.3f}, mpc={best_params[1]:.3f}, spread={best_params[2]:.4f}")
    print(f"Best error: {best_error:.2f}")
    
    # Save results
    results_df = pd.DataFrame([
        {'markup': p[0], 'mpc': p[1], 'spread': p[2], 'error': e}
        for p, e, _ in results
    ])
    results_df.to_csv('calibration_results.csv', index=False)
    
    best_results.to_csv('best_model_results.csv', index=False)
    
    return best_params, best_error, results_df


def create_param_grid(coarse=True):
    """Create parameter grid for search"""
    
    if coarse:
        # Coarse grid (27 combinations)
        markups = [1.10, 1.15, 1.20]
        mpcs = [0.70, 0.75, 0.80]
        spreads = [0.02, 0.03, 0.04]
    else:
        # Fine grid (125 combinations)
        markups = np.linspace(1.10, 1.20, 5)
        mpcs = np.linspace(0.70, 0.80, 5)
        spreads = np.linspace(0.02, 0.04, 5)
    
    # Create all combinations
    param_grid = [
        (m, p, s) 
        for m in markups 
        for p in mpcs 
        for s in spreads
    ]
    
    return param_grid


def validate_calibration(best_params, n_runs=1000):
    """Validate calibrated model with large sample"""
    
    print(f"\nValidating with {n_runs} runs...")
    
    markup, mpc, spread = best_params
    
    # Run many simulations
    results = []
    for seed in range(n_runs):
        model = HybridEconomyModel(2013, 2, seed, 0,
                                   markup=markup, mpc=mpc, spread=spread)
        df = model.run(steps=13)
        df['seed'] = seed
        results.append(df)
    
    all_results = pd.concat(results)
    
    # Compute statistics
    stats = all_results.groupby('period').agg({
        'nominal_gdp': ['mean', 'std', lambda x: np.percentile(x, 5), 
                       lambda x: np.percentile(x, 95)],
        'nominal_gva': ['mean', 'std'],
        'unemployment': ['mean', 'std']
    })
    
    stats.to_csv('validation_statistics.csv')
    
    # Compare to targets
    for var in ['nominal_gdp', 'nominal_gva']:
        try:
            target = pd.read_csv(f'data/calibration_{var}.csv')
            model_mean = all_results.groupby('period')[var].mean()
            
            rmse = np.sqrt(np.mean((model_mean.values - target['mean'].values) ** 2))
            rel_error = rmse / target['mean'].mean() * 100
            
            print(f"{var}: RMSE={rmse:.2f}, Relative Error={rel_error:.2f}%")
        except:
            print(f"Could not validate {var}")
    
    return all_results


if __name__ == "__main__":
    
    # Stage 1: Coarse grid search
    print("="*60)
    print("STAGE 1: Coarse Grid Search")
    print("="*60)
    
    param_grid = create_param_grid(coarse=True)
    best_params, best_error, results = calibrate_parallel(
        param_grid, n_runs=50, n_workers=None
    )
    
    # Stage 2: Fine grid around best
    print("\n" + "="*60)
    print("STAGE 2: Fine Grid Search")
    print("="*60)
    
    # Create fine grid around best parameters
    m, p, s = best_params
    fine_grid = [
        (m + dm, p + dp, s + ds)
        for dm in np.linspace(-0.05, 0.05, 5)
        for dp in np.linspace(-0.05, 0.05, 5)
        for ds in np.linspace(-0.005, 0.005, 5)
    ]
    
    best_params_fine, best_error_fine, results_fine = calibrate_parallel(
        fine_grid, n_runs=100, n_workers=None
    )
    
    # Stage 3: Validation
    print("\n" + "="*60)
    print("STAGE 3: Validation")
    print("="*60)
    
    validation_results = validate_calibration(best_params_fine, n_runs=1000)
    
    print("\n" + "="*60)
    print("CALIBRATION COMPLETE")
    print("="*60)
    print(f"Final parameters: {best_params_fine}")
    print(f"Files saved:")
    print("  - calibration_results.csv")
    print("  - best_model_results.csv")
    print("  - validation_statistics.csv")
