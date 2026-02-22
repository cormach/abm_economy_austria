import scipy.io
import numpy as np
import pandas as pd
import os
from pathlib import Path


def read_mat_file(filepath):
    """Read MATLAB .mat file and return contents"""
    mat = scipy.io.loadmat(filepath)
    return {k: v for k, v in mat.items() if not k.startswith('__')}


def parse_filename(filename):
    """Parse filename: predictor_yearQquarter_seed.mat"""
    parts = filename.stem.split('_')
    if len(parts) >= 3:
        return {
            'predictor': int(parts[0]),
            'period': parts[1],
            'seed': int(parts[2])
        }
    return None


def extract_all_calibration_data(directory='../matlab_calibration_files'):
    """Extract data from all MATLAB files"""
    mat_files = list(Path(directory).glob('*.mat'))
    print(f"Found {len(mat_files)} MATLAB files")
    
    # Read first file to understand structure
    sample = read_mat_file(mat_files[0])
    print(f"\nVariables in {mat_files[0].name}:")
    for key, value in sample.items():
        if isinstance(value, np.ndarray):
            print(f"  {key}: shape={value.shape}, dtype={value.dtype}")
    
    return mat_files, sample


def aggregate_time_series(directory='../matlab_calibration_files', output_dir='data'):
    """Aggregate all simulation runs into summary statistics"""
    os.makedirs(output_dir, exist_ok=True)
    
    mat_files = list(Path(directory).glob('*.mat'))
    
    # Variables to extract
    variables = [
        'nominal_gdp', 'real_gdp', 'nominal_gva', 'real_gva',
        'nominal_household_consumption', 'real_household_consumption',
        'nominal_government_consumption', 'real_government_consumption',
        'nominal_capitalformation', 'real_capitalformation',
        'nominal_fixed_capitalformation', 'real_fixed_capitalformation',
        'nominal_exports', 'real_exports', 'nominal_imports', 'real_imports',
        'operating_surplus', 'compensation_employees', 'wages',
        'taxes_production', 'euribor'
    ]
    
    # Collect all runs
    all_runs = {var: [] for var in variables}
    metadata = []
    
    for mat_file in mat_files:
        meta = parse_filename(mat_file)
        if meta is None:
            continue
            
        data = read_mat_file(mat_file)
        metadata.append(meta)
        
        for var in variables:
            if var in data:
                all_runs[var].append(data[var].flatten())
    
    print(f"Processed {len(metadata)} files")
    
    # Save aggregated data
    for var in variables:
        if all_runs[var]:
            arr = np.array(all_runs[var])
            
            df = pd.DataFrame({
                'period': range(arr.shape[1]),
                'mean': arr.mean(axis=0),
                'std': arr.std(axis=0),
                'p5': np.percentile(arr, 5, axis=0),
                'p25': np.percentile(arr, 25, axis=0),
                'p50': np.percentile(arr, 50, axis=0),
                'p75': np.percentile(arr, 75, axis=0),
                'p95': np.percentile(arr, 95, axis=0),
            })
            
            output_file = f"{output_dir}/calibration_{var}.csv"
            df.to_csv(output_file, index=False)
            print(f"Saved: {output_file}")
    
    # Save metadata
    df_meta = pd.DataFrame(metadata)
    df_meta.to_csv(f"{output_dir}/simulation_metadata.csv", index=False)
    print(f"Saved: {output_dir}/simulation_metadata.csv")
    
    return all_runs


def extract_sectoral_data(directory='../matlab_calibration_files', output_dir='data'):
    """Extract sector-level data (13x62 matrices)"""
    os.makedirs(output_dir, exist_ok=True)
    
    mat_files = list(Path(directory).glob('*.mat'))
    
    # Read first file
    sample = read_mat_file(mat_files[0])
    
    # Extract sectoral variables
    if 'nominal_sector_gva' in sample:
        sector_data = sample['nominal_sector_gva']
        
        df = pd.DataFrame(
            sector_data,
            columns=[f'Sector_{i+1}' for i in range(sector_data.shape[1])]
        )
        df.insert(0, 'period', range(len(df)))
        
        df.to_csv(f"{output_dir}/calibration_nominal_sector_gva.csv", index=False)
        print(f"Saved: {output_dir}/calibration_nominal_sector_gva.csv")
    
    if 'real_sector_gva' in sample:
        sector_data = sample['real_sector_gva']
        
        df = pd.DataFrame(
            sector_data,
            columns=[f'Sector_{i+1}' for i in range(sector_data.shape[1])]
        )
        df.insert(0, 'period', range(len(df)))
        
        df.to_csv(f"{output_dir}/calibration_real_sector_gva.csv", index=False)
        print(f"Saved: {output_dir}/calibration_real_sector_gva.csv")


if __name__ == "__main__":
    print("Reading MATLAB calibration files...\n")
    
    # Explore structure
    mat_files, sample = extract_all_calibration_data()
    
    print("\n" + "="*60)
    print("Aggregating time series data...")
    all_runs = aggregate_time_series()
    
    print("\n" + "="*60)
    print("Extracting sectoral data...")
    extract_sectoral_data()
    
    print("\n" + "="*60)
    print(f"Extracted {len(all_runs)} variables")
    print("Done!")
