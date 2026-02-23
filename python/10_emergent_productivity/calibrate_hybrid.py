import numpy as np
import pandas as pd
from model_hybrid import HybridProductivityModel
from itertools import product


def evaluate_model(mpc, search_prob, wage, seed=42):
    """Run model and return GDP error"""
    model = HybridProductivityModel(2013, 2, seed)
    model.mpc = mpc
    model.search_prob = search_prob
    model.firm_wage = np.ones(model.n_firms) * wage
    
    results = model.run(steps=13)
    
    # Target GDP from calibration data
    target_gdp = 80126
    final_gdp = results['output'].iloc[-1]
    error_pct = abs(final_gdp - target_gdp) / target_gdp * 100
    
    return {
        'mpc': mpc,
        'search_prob': search_prob,
        'wage': wage,
        'final_gdp': final_gdp,
        'target_gdp': target_gdp,
        'error_pct': error_pct,
        'emergent_productivity': results['emergent_productivity'].iloc[-1]
    }


if __name__ == "__main__":
    # Load target
    gdp_data = pd.read_csv('../data/calibration_nominal_gdp.csv')
    target_gdp = gdp_data.iloc[0, 1]
    
    print(f"Target GDP: {target_gdp:.1f}")
    print("\n=== Grid Search Calibration ===\n")
    
    # Parameter grid - expanded to lower values
    mpc_values = [0.50, 0.55, 0.60, 0.65, 0.70]
    search_values = [0.2, 0.3, 0.4]
    wage_values = [30, 35, 40, 45, 50]
    
    results = []
    total = len(mpc_values) * len(search_values) * len(wage_values)
    count = 0
    
    for mpc, search, wage in product(mpc_values, search_values, wage_values):
        count += 1
        result = evaluate_model(mpc, search, wage)
        results.append(result)
        
        if count % 10 == 0:
            print(f"Progress: {count}/{total} ({count/total*100:.0f}%)")
    
    # Convert to DataFrame and sort by error
    df = pd.DataFrame(results)
    df = df.sort_values('error_pct')
    
    print("\n=== Top 10 Parameter Combinations ===")
    print(df.head(10).to_string(index=False))
    
    # Best parameters
    best = df.iloc[0]
    print(f"\n=== Best Parameters ===")
    print(f"MPC: {best['mpc']:.2f}")
    print(f"Search Probability: {best['search_prob']:.2f}")
    print(f"Wage: {best['wage']:.0f}")
    print(f"Final GDP: {best['final_gdp']:.1f}")
    print(f"Target GDP: {best['target_gdp']:.1f}")
    print(f"Error: {best['error_pct']:.2f}%")
    print(f"Emergent Productivity: {best['emergent_productivity']:.3f}")
    
    # Save results
    df.to_csv('calibration_results.csv', index=False)
    print("\nResults saved to calibration_results.csv")
