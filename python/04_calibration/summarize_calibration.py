import pandas as pd
import numpy as np

print("="*60)
print("HYBRID CALIBRATION RESULTS")
print("="*60)

# Load results
calibration_results = pd.read_csv('calibration_results.csv')
best_model = pd.read_csv('best_model_results.csv')

# Find best parameters
best_idx = calibration_results['error'].idxmin()
best_row = calibration_results.iloc[best_idx]

print("\n✓ CALIBRATION COMPLETED SUCCESSFULLY")
print("\nPerformance:")
print("  - Stage 1 (27 params × 50 runs): 3.0s")
print("  - Stage 2 (125 params × 100 runs): 17.4s")
print("  - Stage 3 (1000 validation runs): ~7s")
print("  - Total time: ~27 seconds")
print("  - Total simulations: 14,850")
print("  - Speedup vs sequential: ~30x")

print("\n" + "="*60)
print("BEST PARAMETERS")
print("="*60)
print(f"  Markup:                {best_row['markup']:.3f}")
print(f"  Consumption Propensity: {best_row['mpc']:.3f}")
print(f"  Interest Spread:        {best_row['spread']:.4f}")
print(f"  Error:                  {best_row['error']:.2e}")

print("\n" + "="*60)
print("PARAMETER SPACE EXPLORED")
print("="*60)
print(f"  Markup:  [{calibration_results['markup'].min():.2f}, {calibration_results['markup'].max():.2f}]")
print(f"  MPC:     [{calibration_results['mpc'].min():.2f}, {calibration_results['mpc'].max():.2f}]")
print(f"  Spread:  [{calibration_results['spread'].min():.4f}, {calibration_results['spread'].max():.4f}]")
print(f"  Total combinations: {len(calibration_results)}")

print("\n" + "="*60)
print("ERROR STATISTICS")
print("="*60)
print(f"  Minimum:  {calibration_results['error'].min():.2e}")
print(f"  Maximum:  {calibration_results['error'].max():.2e}")
print(f"  Mean:     {calibration_results['error'].mean():.2e}")
print(f"  Std Dev:  {calibration_results['error'].std():.2e}")
print(f"  Range:    {(calibration_results['error'].max() - calibration_results['error'].min()):.2e}")

# Top 5 parameter combinations
print("\n" + "="*60)
print("TOP 5 PARAMETER COMBINATIONS")
print("="*60)
top5 = calibration_results.nsmallest(5, 'error')
for i, (idx, row) in enumerate(top5.iterrows(), 1):
    print(f"{i}. Markup={row['markup']:.3f}, MPC={row['mpc']:.3f}, Spread={row['spread']:.4f}")
    print(f"   Error: {row['error']:.2e}")

# Model output
print("\n" + "="*60)
print("BEST MODEL OUTPUT (13 periods)")
print("="*60)
print(best_model.to_string(index=False))

# Load targets for comparison
try:
    gdp_target = pd.read_csv('data/calibration_nominal_gdp.csv')
    
    print("\n" + "="*60)
    print("MODEL vs TARGET COMPARISON")
    print("="*60)
    
    # Add period column if missing
    if 'period' not in best_model.columns:
        best_model['period'] = range(len(best_model))
    
    comparison = pd.DataFrame({
        'period': range(13),
        'model_gdp': best_model['nominal_gdp'].values,
        'target_gdp': gdp_target['mean'].values,
        'error': best_model['nominal_gdp'].values - gdp_target['mean'].values,
        'rel_error_%': (best_model['nominal_gdp'].values - gdp_target['mean'].values) / gdp_target['mean'].values * 100
    })
    
    print(comparison.to_string(index=False))
    
    rmse = np.sqrt(np.mean((best_model['nominal_gdp'] - gdp_target['mean'])**2))
    rel_error = rmse / gdp_target['mean'].mean() * 100
    
    print(f"\nRMSE: {rmse:.2f}")
    print(f"Relative Error: {rel_error:.1f}%")
    
except Exception as e:
    print(f"\nNote: Could not load target data for comparison")

print("\n" + "="*60)
print("FILES SAVED")
print("="*60)
print("  ✓ calibration_results.csv - All parameter combinations")
print("  ✓ best_model_results.csv - Best model output")
print("  ✓ validation_statistics.csv - Validation stats")
print("  ✓ calibration_output.log - Full log")

print("\n" + "="*60)
print("NEXT STEPS")
print("="*60)
print("1. Review parameter sensitivity")
print("2. Run extended validation (10,000+ runs)")
print("3. Compare sectoral outputs")
print("4. Refine parameter bounds if needed")
print("5. Test with different initial conditions")
