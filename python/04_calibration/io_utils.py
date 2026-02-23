import pandas as pd
import numpy as np


def load_io_matrix(filepath='data/io_matrix.csv'):
    """Load I-O matrix from CSV"""
    return pd.read_csv(filepath, index_col=0)


def compute_technical_coefficients(io_matrix):
    """
    Compute technical coefficients matrix A
    A[i,j] = input from sector i per unit output of sector j
    """
    return io_matrix / io_matrix.sum(axis=0)


def compute_leontief_inverse(A):
    """
    Compute Leontief inverse L = (I - A)^-1
    Shows total output needed from all sectors for 1 unit final demand
    """
    I = np.eye(len(A))
    return np.linalg.inv(I - A)


def compute_sector_linkages(io_matrix):
    """
    Compute backward and forward linkages
    Backward: how much sector buys from others
    Forward: how much sector sells to others
    """
    backward = io_matrix.sum(axis=0)  # Column sums
    forward = io_matrix.sum(axis=1)   # Row sums
    
    return pd.DataFrame({
        'backward_linkage': backward,
        'forward_linkage': forward,
        'total_linkage': backward + forward
    }).sort_values('total_linkage', ascending=False)


def propagate_demand(io_matrix, final_demand):
    """
    Calculate total output needed given final demand vector
    
    Args:
        io_matrix: 62x62 I-O matrix
        final_demand: array of length 62 (final demand per sector)
    
    Returns:
        total_output: array of length 62 (total production needed)
    """
    A = compute_technical_coefficients(io_matrix)
    L = compute_leontief_inverse(A)
    return L @ final_demand


def simulate_supply_shock(io_matrix, sector_idx, shock_magnitude=0.5):
    """
    Simulate supply shock to a sector
    
    Args:
        io_matrix: 62x62 I-O matrix
        sector_idx: index of shocked sector
        shock_magnitude: fraction of capacity remaining (0.5 = 50% reduction)
    
    Returns:
        shocked_matrix: modified I-O matrix
    """
    shocked = io_matrix.copy()
    shocked[:, sector_idx] *= shock_magnitude
    shocked[sector_idx, :] *= shock_magnitude
    return shocked


if __name__ == "__main__":
    # Example usage
    io = load_io_matrix()
    
    print("I-O Matrix shape:", io.shape)
    print("\nTop 10 most connected sectors:")
    print(compute_sector_linkages(io).head(10))
    
    # Simulate demand shock
    final_demand = np.zeros(62)
    final_demand[0] = 100  # 100 units demand in sector 1
    
    total_output = propagate_demand(io.values, final_demand)
    print(f"\nDemand of 100 in Sector_1 requires total output: {total_output.sum():.2f}")
