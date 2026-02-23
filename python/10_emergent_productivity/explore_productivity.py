"""
Explore emergent productivity from sectoral data

Instead of using a single productivity parameter, derive productivity
from the actual Austrian sectoral data (employment, output, wages).
"""

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

# Load sectoral data
sector_gva = pd.read_csv('../data/calibration_nominal_sector_gva.csv')
comp = pd.read_csv('../data/calibration_compensation_employees.csv')

# Period 0
sector_output = sector_gva.iloc[0, 1:].values  # 62 sectors
total_comp = comp.iloc[0, 1]

print("="*60)
print("EMERGENT PRODUCTIVITY ANALYSIS")
print("="*60)

# 1. Aggregate productivity
total_output = np.sum(sector_output)
aggregate_productivity = total_output / total_comp

print(f"\n1. Aggregate Level:")
print(f"   Total output (GVA): {total_output:.2f}")
print(f"   Total compensation: {total_comp:.2f}")
print(f"   Implied productivity: {aggregate_productivity:.3f}")
print(f"   → Output = Compensation × {aggregate_productivity:.3f}")

# 2. Sectoral productivity distribution
sector_productivity = sector_output / (total_comp / 62)  # Assume equal employment
print(f"\n2. Sectoral Productivity:")
print(f"   Mean: {np.mean(sector_productivity):.3f}")
print(f"   Std: {np.std(sector_productivity):.3f}")
print(f"   Min: {np.min(sector_productivity):.3f}")
print(f"   Max: {np.max(sector_productivity):.3f}")
print(f"   Median: {np.median(sector_productivity):.3f}")

# 3. Top and bottom sectors
top_5 = np.argsort(sector_productivity)[-5:]
bottom_5 = np.argsort(sector_productivity)[:5]

print(f"\n3. Top 5 Productive Sectors:")
for i, idx in enumerate(top_5[::-1], 1):
    print(f"   {i}. Sector {idx+1}: {sector_productivity[idx]:.3f}")

print(f"\n4. Bottom 5 Productive Sectors:")
for i, idx in enumerate(bottom_5, 1):
    print(f"   {i}. Sector {idx+1}: {sector_productivity[idx]:.3f}")

# 4. What if we use sectoral productivity?
print(f"\n5. Model Implications:")
print(f"   Current model: Single productivity = 1.6")
print(f"   Empirical: Aggregate productivity = {aggregate_productivity:.3f}")
print(f"   Difference: {(aggregate_productivity - 1.6) / 1.6 * 100:.1f}%")
print()
print(f"   If we use sectoral productivity:")
print(f"   - Each sector has different output/wage ratio")
print(f"   - Aggregate emerges from sectoral composition")
print(f"   - More realistic but more complex")

# 5. Visualize distribution
plt.figure(figsize=(12, 5))

plt.subplot(1, 2, 1)
plt.hist(sector_productivity, bins=20, edgecolor='black', alpha=0.7)
plt.axvline(aggregate_productivity, color='red', linestyle='--', 
            label=f'Aggregate: {aggregate_productivity:.2f}')
plt.axvline(1.6, color='blue', linestyle='--', label='Model: 1.6')
plt.xlabel('Productivity (Output/Compensation)')
plt.ylabel('Number of Sectors')
plt.title('Sectoral Productivity Distribution')
plt.legend()
plt.grid(alpha=0.3)

plt.subplot(1, 2, 2)
plt.plot(sorted(sector_productivity), marker='o', markersize=3)
plt.axhline(aggregate_productivity, color='red', linestyle='--', 
            label=f'Aggregate: {aggregate_productivity:.2f}')
plt.axhline(1.6, color='blue', linestyle='--', label='Model: 1.6')
plt.xlabel('Sector (sorted by productivity)')
plt.ylabel('Productivity')
plt.title('Sectoral Productivity (Sorted)')
plt.legend()
plt.grid(alpha=0.3)

plt.tight_layout()
plt.savefig('sectoral_productivity.png', dpi=150, bbox_inches='tight')
print(f"\n6. Saved visualization: sectoral_productivity.png")

# 6. Time series analysis
print(f"\n7. Productivity Over Time:")
for period in range(min(5, len(sector_gva))):
    sector_out = sector_gva.iloc[period, 1:].values
    total_out = np.sum(sector_out)
    total_comp_p = comp.iloc[period, 1]
    prod = total_out / total_comp_p
    print(f"   Period {period}: {prod:.3f}")

print("\n" + "="*60)
print("CONCLUSION")
print("="*60)
print(f"The empirical productivity is {aggregate_productivity:.3f}, not 1.6")
print(f"This {(aggregate_productivity - 1.6) / 1.6 * 100:.1f}% difference explains")
print(f"why the model generates different output levels.")
print()
print("Options:")
print("1. Use aggregate productivity = 1.93 (simple)")
print("2. Use sectoral productivity (complex but realistic)")
print("3. Calibrate to match empirical aggregate (current approach)")
