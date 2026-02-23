import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Load results
best_model = pd.read_csv('best_model_results.csv')
best_model['period'] = range(len(best_model))  # Add period column
calibration_results = pd.read_csv('calibration_results.csv')

# Load targets
try:
    gdp_target = pd.read_csv('data/calibration_nominal_gdp.csv')
    gva_target = pd.read_csv('data/calibration_nominal_gva.csv')
    has_targets = True
except:
    has_targets = False
    print("Warning: Could not load target data")

# Create figure
fig, axes = plt.subplots(2, 2, figsize=(14, 10))

# 1. Parameter space exploration
ax = axes[0, 0]
scatter = ax.scatter(calibration_results['markup'], 
                    calibration_results['mpc'],
                    c=calibration_results['error'],
                    s=50, alpha=0.6, cmap='viridis')
ax.set_xlabel('Markup')
ax.set_ylabel('Consumption Propensity (MPC)')
ax.set_title('Parameter Space Exploration')
plt.colorbar(scatter, ax=ax, label='Error')

# Mark best
best_idx = calibration_results['error'].idxmin()
best_row = calibration_results.iloc[best_idx]
ax.scatter(best_row['markup'], best_row['mpc'], 
          color='red', s=200, marker='*', 
          edgecolors='black', linewidths=2,
          label=f"Best: ({best_row['markup']:.2f}, {best_row['mpc']:.2f})")
ax.legend()

# 2. GDP fit
ax = axes[0, 1]
ax.plot(best_model['period'], best_model['nominal_gdp'], 
       'o-', label='Model', linewidth=2)
if has_targets:
    ax.plot(gdp_target['period'], gdp_target['mean'], 
           's-', label='Target (mean)', linewidth=2)
    ax.fill_between(gdp_target['period'],
                    gdp_target['p25'], gdp_target['p75'],
                    alpha=0.3, label='Target (IQR)')
ax.set_xlabel('Period (quarters)')
ax.set_ylabel('Nominal GDP')
ax.set_title('GDP Fit')
ax.legend()
ax.grid(True, alpha=0.3)

# 3. Error distribution
ax = axes[1, 0]
ax.hist(np.log10(calibration_results['error']), bins=30, edgecolor='black')
ax.axvline(np.log10(best_row['error']), color='red', 
          linestyle='--', linewidth=2, label='Best')
ax.set_xlabel('Log10(Error)')
ax.set_ylabel('Frequency')
ax.set_title('Error Distribution')
ax.legend()

# 4. Parameter sensitivity
ax = axes[1, 1]
grouped = calibration_results.groupby('markup')['error'].mean()
ax.plot(grouped.index, grouped.values, 'o-', label='Markup', linewidth=2)
grouped = calibration_results.groupby('mpc')['error'].mean()
ax2 = ax.twinx()
ax2.plot(grouped.index, grouped.values, 's-', color='orange', 
        label='MPC', linewidth=2)
ax.set_xlabel('Parameter Value')
ax.set_ylabel('Mean Error (Markup)', color='blue')
ax2.set_ylabel('Mean Error (MPC)', color='orange')
ax.set_title('Parameter Sensitivity')
ax.tick_params(axis='y', labelcolor='blue')
ax2.tick_params(axis='y', labelcolor='orange')

plt.tight_layout()
plt.savefig('calibration_results.png', dpi=150, bbox_inches='tight')
print("Saved: calibration_results.png")

# Summary statistics
print("\n" + "="*60)
print("CALIBRATION SUMMARY")
print("="*60)
print(f"Best parameters:")
print(f"  Markup: {best_row['markup']:.3f}")
print(f"  MPC: {best_row['mpc']:.3f}")
print(f"  Spread: {best_row['spread']:.4f}")
print(f"  Error: {best_row['error']:.2e}")

print(f"\nParameter ranges explored:")
print(f"  Markup: [{calibration_results['markup'].min():.2f}, {calibration_results['markup'].max():.2f}]")
print(f"  MPC: [{calibration_results['mpc'].min():.2f}, {calibration_results['mpc'].max():.2f}]")
print(f"  Spread: [{calibration_results['spread'].min():.4f}, {calibration_results['spread'].max():.4f}]")

print(f"\nError statistics:")
print(f"  Min: {calibration_results['error'].min():.2e}")
print(f"  Max: {calibration_results['error'].max():.2e}")
print(f"  Mean: {calibration_results['error'].mean():.2e}")
print(f"  Std: {calibration_results['error'].std():.2e}")

if has_targets:
    print(f"\nModel vs Target:")
    print(f"  GDP RMSE: {np.sqrt(np.mean((best_model['nominal_gdp'] - gdp_target['mean'])**2)):.2f}")
    print(f"  GDP Relative Error: {np.sqrt(np.mean((best_model['nominal_gdp'] - gdp_target['mean'])**2)) / gdp_target['mean'].mean() * 100:.1f}%")
