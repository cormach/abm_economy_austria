import re
import csv

def extract_io_matrix(filepath='../src/mbu.c'):
    """Extract the 62x62 Input-Output matrix from mbu.c"""
    with open(filepath, 'r') as f:
        content = f.read()
    
    pattern = r'static const double dv0\[3844\] = \{([^}]+)\}'
    match = re.search(pattern, content, re.DOTALL)
    
    if match:
        numbers_str = match.group(1)
        numbers = re.findall(r'[-+]?[0-9]*\.?[0-9]+(?:[eE][-+]?[0-9]+)?', numbers_str)
        io_data = [float(n) for n in numbers]
        
        # Reshape to 62x62
        io_matrix = []
        for i in range(62):
            row = io_data[i*62:(i+1)*62]
            io_matrix.append(row)
        
        return io_matrix
    
    return None

def save_calibration_data():
    """Extract and save all calibration data as CSV files"""
    import os
    os.makedirs('data', exist_ok=True)
    
    # 1. Input-Output Matrix
    io_matrix = extract_io_matrix()
    if io_matrix is not None:
        with open('data/io_matrix.csv', 'w', newline='') as f:
            writer = csv.writer(f)
            header = [''] + [f'Sector_{i+1}' for i in range(62)]
            writer.writerow(header)
            for i, row in enumerate(io_matrix):
                writer.writerow([f'Sector_{i+1}'] + row)
        print(f"✓ I-O matrix saved: 62x62, sum={sum(sum(row) for row in io_matrix):.2f}")
    
    # 2. Model Parameters
    with open('data/parameters.csv', 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(['parameter', 'value'])
        writer.writerow(['TIME_PERIODS', 13])
        writer.writerow(['N_SECTORS', 62])
        writer.writerow(['MARKUP', 1.2])
        writer.writerow(['PROPENSITY_TO_CONSUME', 0.8])
        writer.writerow(['TAX_RATE', 0.2])
        writer.writerow(['CAPITAL_SHARE', 0.3])
        writer.writerow(['LABOR_SHARE', 0.7])
        writer.writerow(['INTEREST_RATE_SPREAD', 0.02])
        writer.writerow(['MAX_LEVERAGE', 0.5])
        writer.writerow(['VAR_TOLERANCE', 2.2204460492503131e-16])
    print("✓ Parameters saved: 10 parameters")
    
    # 3. Initial Conditions
    with open('data/initial_conditions.csv', 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(['variable', 'value'])
        writer.writerow(['initial_capital', 100.0])
        writer.writerow(['initial_wealth', 100.0])
        writer.writerow(['initial_bank_assets', 1000.0])
        writer.writerow(['initial_price', 1.0])
        writer.writerow(['initial_wage', 1.0])
        writer.writerow(['initial_euribor', 0.01])
    print("✓ Initial conditions saved: 6 variables")

if __name__ == "__main__":
    save_calibration_data()
