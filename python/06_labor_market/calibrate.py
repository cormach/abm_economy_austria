import numpy as np
import pandas as pd
from multiprocessing import Pool, cpu_count
from functools import partial
from model_labor import LaborMarketModel
import time


def evaluate_params(params, n_runs, targets):
    """Evaluate single parameter set"""
    productivity, mpc, wage, search_prob = params
    
    results = []
    for seed in range(n_runs):
        model = LaborMarketModel(2013, 2, seed)
        model.productivity = productivity
        model.mpc = mpc
        model.firm_wage[:] = wage
        model.search_prob = search_prob
        
        df = model.run(steps=13)
        results.append(df)
    
    all_results = pd.concat(results)
    mean_results = all_results.groupby('period').mean()
    
    # Compute error
    error = 0
    if 'nominal_gdp' in targets:
        target_gdp = targets['nominal_gdp']['mean'].values
        model_gdp = mean_results['output'].values
        error += np.sum((model_gdp - target_gdp) ** 2)
    
    return params, error, mean_results


def calibrate_stage(param_grid, n_runs, targets, n_workers):
    """Single calibration stage"""
    eval_func = partial(evaluate_params, n_runs=n_runs, targets=targets)
    
    with Pool(n_workers) as pool:
        results = pool.map(eval_func, param_grid)
    
    best_params, best_error, best_results = min(results, key=lambda x: x[1])
    return best_params, best_error, best_results, results


if __name__ == "__main__":
    n_workers = cpu_count()
    
    # Load targets
    targets = {}
    try:
        targets['nominal_gdp'] = pd.read_csv('../data/calibration_nominal_gdp.csv')
    except:
        print("Warning: No targets found")
    
    # Stage 1: Coarse grid
    print("="*60)
    print("STAGE 1: Coarse Grid")
    print("="*60)
    
    coarse_grid = [
        (prod, mpc, wage, search)
        for prod in [0.3, 0.5, 0.7]
        for mpc in [0.7, 0.75, 0.8]
        for wage in [50, 75, 100]
        for search in [0.2, 0.3, 0.4]
    ]
    
    print(f"Testing {len(coarse_grid)} combinations on {n_workers} cores...")
    start = time.time()
    best_params, best_error, best_results, all_results = calibrate_stage(
        coarse_grid, n_runs=50, targets=targets, n_workers=n_workers
    )
    print(f"Completed in {time.time()-start:.1f}s")
    print(f"Best: prod={best_params[0]:.2f}, mpc={best_params[1]:.2f}, wage={best_params[2]:.0f}, search={best_params[3]:.2f}")
    print(f"Error: {best_error:.2f}")
    
    # Stage 2: Fine grid
    print("\n" + "="*60)
    print("STAGE 2: Fine Grid")
    print("="*60)
    
    prod, mpc, wage, search = best_params
    fine_grid = [
        (prod + dp, mpc + dm, wage + dw, search + ds)
        for dp in np.linspace(-0.1, 0.1, 5)
        for dm in np.linspace(-0.05, 0.05, 5)
        for dw in np.linspace(-25, 25, 5)
        for ds in np.linspace(-0.1, 0.1, 5)
    ]
    
    print(f"Testing {len(fine_grid)} combinations on {n_workers} cores...")
    start = time.time()
    best_params, best_error, best_results, fine_results = calibrate_stage(
        fine_grid, n_runs=100, targets=targets, n_workers=n_workers
    )
    print(f"Completed in {time.time()-start:.1f}s")
    print(f"Best: prod={best_params[0]:.2f}, mpc={best_params[1]:.2f}, wage={best_params[2]:.0f}, search={best_params[3]:.2f}")
    print(f"Error: {best_error:.2f}")
    
    # Stage 3: Validation
    print("\n" + "="*60)
    print("STAGE 3: Validation")
    print("="*60)
    
    print(f"Running 1000 simulations...")
    start = time.time()
    _, _, validation_results, _ = calibrate_stage(
        [best_params], n_runs=1000, targets=targets, n_workers=n_workers
    )
    print(f"Completed in {time.time()-start:.1f}s")
    
    # Save
    results_df = pd.DataFrame([
        {'productivity': p[0], 'mpc': p[1], 'wage': p[2], 'search_prob': p[3], 'error': e}
        for p, e, _ in all_results + fine_results
    ])
    results_df.to_csv('calibration_results.csv', index=False)
    best_results.to_csv('best_results.csv', index=False)
    validation_results.to_csv('validation_results.csv', index=False)
    
    print("\n" + "="*60)
    print("CALIBRATION COMPLETE")
    print("="*60)
    print(f"Final: prod={best_params[0]:.3f}, mpc={best_params[1]:.3f}, wage={best_params[2]:.0f}, search={best_params[3]:.3f}")
